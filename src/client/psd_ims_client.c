/*******************************************************************************
 *	psd_ims_client.h
 *
 *  PSD-IMS client API
 *
 *
 *  This file is part of PSD-IMS
 * 
 *  Copyright (C) 2015  Daniel Pinto Rivero, Javier Bermúdez Blanco
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********************************************************************************/
	// Is simplier to work here with gsoap structs...
	// try to abstract them in network is a mess
#include "soapH.h"
#include "psd_ims_client.h"
#include "friends.h"
#include "chats.h"
#include "network.h"
#include "friend_requests.h"
#include "messages.h"
#include "chat_members.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "debug_def.h"

#ifdef DEBUG
#include "leak_detector_c.h"
#endif


#define sizeofstring(string) \
	(strlen(string) + sizeof(char))


int _recv_messages(psd_ims_client *client, chat_info *chat) {
	DEBUG_TRACE_PRINT();
	psdims__message_list *list;
	char **sender;
	char **text;
	char **attach_path;
	int *send_date;

	int n_messages;
	int i;
	int timestamp;
	int chat_id;

	cha_get_messages_timestamp(chat, timestamp);
	chat_id = cha_get_id(chat);


	pthread_mutex_lock(&client->network_mutex);
	if( (list = net_recv_pending_messages(client->network, chat_id, timestamp)) == NULL ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not get the message list");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	sender = (char**)malloc(sizeof(char*)*list->__sizenelems);
	text = (char**)malloc(sizeof(char*)*list->__sizenelems);
	attach_path = (char**)malloc(sizeof(char*)*list->__sizenelems);
	send_date = (int*)malloc(sizeof(int)*list->__sizenelems);


	for( i = 0; i < list->__sizenelems; i++) {
		sender[i] = (strcmp(list->messages[i].user, client->user_name) == 0)? NULL : list->messages[i].user;
		text[i] = list->messages[i].text;
		attach_path[i] = list->messages[i].file_name;	
		send_date[i] = list->messages[i].send_date;
		DEBUG_INFO_PRINTF("Adding message <%d, %s, %s, %d>", chat_id, sender[i], text[i], send_date[i]);
	}

	n_messages = list->__sizenelems;
	if ( cha_add_messages(chat, sender, text, send_date, attach_path, n_messages) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not add messages");
		free(sender);
		free(text);
		free(attach_path);
		free(send_date);
		return -1;
	}

	free(sender);
	free(text);
	free(attach_path);
	free(send_date);
	
	cha_set_pending(chat, 0);
	cha_set_messages_timestamp(chat, list->last_timestamp);
	net_free_message_list(list);

	return n_messages;
}


/* =========================================================================
 *  Client struct
 * =========================================================================*/

/*
 * Allocates a new psd_ims_client struct
 * Returns a pointer to the structure or NULL if fails
 */
psd_ims_client *psd_new_client() {
	DEBUG_TRACE_PRINT();
	psd_ims_client *client;

	client = malloc( sizeof(psd_ims_client) );
	client->user_name = NULL;
	client->user_pass = NULL;
	client->last_connection = 0; //TODO change this
	client->last_notif_timestamp = 0;

	pthread_mutex_init(&client->new_chats_mutex, NULL);
	pthread_mutex_init(&client->chats_mutex, NULL);
	pthread_mutex_init(&client->network_mutex, NULL);
	pthread_mutex_init(&client->friends_mutex, NULL);
	pthread_mutex_init(&client->requests_mutex, NULL);

	client->friends = fri_new(MAX_FRIENDS);
	client->requests = req_new(MAX_FRIEND_REQUESTS);
	client->chats = cha_new(MAX_CHATS);
	client->network = net_new();

	return client;
}


/*
 * Removes and frees the client struct
 */
void psd_free_client(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();

	pthread_mutex_destroy(&client->new_chats_mutex);
	pthread_mutex_destroy(&client->chats_mutex);
	pthread_mutex_destroy(&client->network_mutex);
	pthread_mutex_destroy(&client->friends_mutex);

	fri_free(client->friends);
	req_free(client->requests);
	cha_free(client->chats);
	net_free(client->network);	

	free(client->user_name);
	free(client->user_pass);

	free(client);
}


/*
 * Sets client name
 * Returns o or -1 if fails
 */
int psd_set_name(psd_ims_client *client, const char *name) {
	DEBUG_TRACE_PRINT();
	char *user_name;

	if ( (user_name = malloc(sizeofstring(name))) == NULL ) {
		return -1;
	}
	strcpy(user_name, name);
	client->user_name = user_name;

	return 0;
}


/*
 * Sets client password
 * Returns o or -1 if fails
 */
int psd_set_pass(psd_ims_client *client, const char *pass) {
	DEBUG_TRACE_PRINT();
	char *user_pass;	

	if ( (user_pass = malloc(sizeofstring(pass))) == NULL ) {
		return -1;
	}

	strcpy(user_pass, pass);
	client->user_pass = user_pass;

	return 0;
}


/* =========================================================================
 *  Network operations
 * =========================================================================*/

/*
 * Init the network
 */
int psd_bind_network(psd_ims_client *client, char *serverURL) {
	int ret_value;
	
	pthread_mutex_lock(&client->network_mutex);
	ret_value = net_bind_network(client->network, serverURL);
	pthread_mutex_unlock(&client->network_mutex);

	return ret_value;
}


/*
 * Gets the user information from the server
 * Returns 0 or -1 if fails
 */
int psd_login(psd_ims_client *client, char *name, char *password) {
	DEBUG_TRACE_PRINT();
	psdims__user_info *user_info;

	pthread_mutex_lock(&client->network_mutex);
	if( (user_info = net_login(client->network, name, password)) == NULL ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Bad login");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);	

	if ( (client->user_name = malloc( sizeofstring(name) )) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory");
		return -1;
	}
	if ( (client->user_pass = malloc( sizeofstring(password) )) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory");
		return -1;
	}
	if ( (client->user_info = malloc( sizeofstring(user_info->information) )) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory");
		return -1;
	}

	strcpy(client->user_name, name);
	strcpy(client->user_pass, password);
	strcpy(client->user_info, user_info->information);

	net_free_user(user_info);

	return 0;
}


/*
 * Loggs out
 */
void psd_logout(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	net_logout(client->network);

	free(client->user_name);
	free(client->user_pass);
	free(client->user_info);

	client->user_name = NULL;
	client->user_pass = NULL;
	client->user_info = NULL;
	client->last_connection = 0; //TODO change this
	client->last_notif_timestamp = 0;

	// Free structures
	fri_free(client->friends);
	req_free(client->requests);
	cha_free(client->chats);

	// Create structures again
	client->friends = fri_new(MAX_FRIENDS);
	client->requests = req_new(MAX_FRIEND_REQUESTS);
	client->chats = cha_new(MAX_CHATS);
}


/*
 * Register the user in the system
 * Returns 0 or -1 if fails
 */
int psd_user_register(psd_ims_client *client, char *name, char *password, char *information) {
	DEBUG_TRACE_PRINT();

	pthread_mutex_lock(&client->network_mutex);
	if( net_user_register(client->network, name, password, information) != 0 ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not register the user");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	return 0;
}


/*
 * Unregister the user from the system
 * Returns 0 or -1 if fails
 */
int psd_user_unregister(psd_ims_client *client, char *name, char *password) {
	DEBUG_TRACE_PRINT();

	pthread_mutex_lock(&client->network_mutex);
	if( net_user_unregister(client->network, name, password) != 0 ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not unregister the user");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	return 0;
}


/*
 * Receive the client data since the beginning of time
 * Returns 0 or -1 if fails
 */
int psd_recv_all_data(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	psdims__client_data *client_data;
	psdims__chat_list *chats;
	int i, j;
	friend_info **members;
	char **member_names;

	pthread_mutex_lock(&client->network_mutex);
	if( (client_data = net_recv_all_data(client->network)) == NULL ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not retrieve the client data");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	client->last_notif_timestamp = client_data->timestamp;

	// new friend requests
	pthread_mutex_lock(&client->requests_mutex);
	for( i = 0 ; i < client_data->friend_requests.__sizenelems ; i++ ) {
		DEBUG_INFO_PRINTF("adding request <%d, %s>",client_data->friend_requests.user[i].send_date, client_data->friend_requests.user[i].name.string);
		req_add_request(client->requests, client_data->friend_requests.user[i].name.string, client_data->friend_requests.user[i].send_date);
	}
	req_list_set_timestamp(client->requests, client_data->timestamp);
	pthread_mutex_unlock(&client->requests_mutex);

	// new friends
	pthread_mutex_lock(&client->friends_mutex);
	for( i = 0 ; i < client_data->friends.__sizenelems ; i++ ) {
		DEBUG_INFO_PRINTF("adding friend <%s, %s>", client_data->friends.user[i].name, client_data->friends.user[i].information);
		fri_add_friend(client->friends, client_data->friends.user[i].name, client_data->friends.user[i].information);
	}
	fri_set_timestamp(client->friends, client_data->timestamp);
	pthread_mutex_unlock(&client->friends_mutex);
	
	// add chats
	chats = &(client_data->chats);
	for( i = 0 ; i < chats->__sizenelems ; i++ ) {
		DEBUG_INFO_PRINTF("Adding chat <%d, %s>", chats->chat_info[i].chat_id, chats->chat_info[i].description);
		// Alloc member list
		if ( (members = malloc(sizeof(friend_info *)*chats->chat_info[i].members.__sizenelems)) == NULL ) {
			DEBUG_FAILURE_PRINTF("Could not allocate memory for temp member list");
			net_free_chat_list(chats);
			return -1;
		}
		if ( (member_names = malloc(sizeof(char *)*chats->chat_info[i].members.__sizenelems)) == NULL ) {
			DEBUG_FAILURE_PRINTF("Could not allocate memory for temp member list");
			net_free_chat_list(chats);
			free(members);
			return -1;
		}

		pthread_mutex_lock(&client->friends_mutex);	
		for ( j = 0 ; j < chats->chat_info[i].members.__sizenelems ; j ++ ) {
			members[j] = fri_find_friend(client->friends, chats->chat_info[i].members.name[j].string);
			member_names[j] = chats->chat_info[i].members.name[j].string;
			DEBUG_INFO_PRINTF("Member <%d, %s, %s>", j , member_names[j], ((members[j] != NULL)? "FRIEND" : "NOT FRIEND") );
		}
		pthread_mutex_unlock(&client->friends_mutex);
		
		pthread_mutex_lock(&client->chats_mutex);
		if ( cha_add_chat(client->chats, chats->chat_info[i].chat_id, chats->chat_info[i].description, 
				chats->chat_info[i].admin, members, member_names, chats->chat_info[i].members.__sizenelems, 
				MAX_MEMBERS, MAX_MESSAGES, chats->chat_info[i].read_timestamp, chats->chat_info[i].all_read_timestamp) != 0 ) {
			pthread_mutex_unlock(&client->chats_mutex);
			net_free_chat_list(chats);
			DEBUG_FAILURE_PRINTF("Could not add chat");
		}
	
		pthread_mutex_unlock(&client->chats_mutex);

		// Free member list
		free(members);
		free(member_names);
	}
	
	pthread_mutex_lock(&client->chats_mutex);
	cha_set_timestamp(client->chats, chats->last_timestamp);
	pthread_mutex_unlock(&client->chats_mutex);

	//free_client_data(client_data);

	return 0;
}


/*
 * Receive the pending notifications
 * Returns the number of received notifications or -1 if fails
 */
int psd_recv_notifications(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	int i;
	psdims__notifications *notifications;
	int total_notifications = 0;
	chat_info *chat;
	friend_info *friend;
	chat_iterator *iterator;
	int *chats_id;
	int *chats_read_timestamp;
	int n_sync_chats = 0;
	

	pthread_mutex_lock(&client->chats_mutex);
	n_sync_chats = cha_num_chats(client->chats);
	
	
	chats_id = malloc(sizeof(int)*n_sync_chats);
	chats_read_timestamp = malloc(sizeof(int)*n_sync_chats);

	iterator = cha_get_chats_iterator(client->chats);
	i = 0;
	while (iterator != NULL) {
		chat = cha_get_info(iterator);
		chats_id[i] = cha_get_id(chat);
		chats_read_timestamp[i] = cha_read_timestamp(chat);
		cha_iterator_next(client->chats, iterator);
		i++;
	}
	pthread_mutex_unlock(&client->chats_mutex);

	pthread_mutex_lock(&client->network_mutex);
	if( (notifications = net_recv_notifications(client->network, client->last_notif_timestamp, chats_id, chats_read_timestamp, n_sync_chats)) == NULL ) {
		pthread_mutex_unlock(&client->network_mutex);
		free(chats_id);
		free(chats_read_timestamp);
		DEBUG_FAILURE_PRINTF("Could not receive the notifications");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	free(chats_id);
	free(chats_read_timestamp);

	client->last_notif_timestamp = notifications->last_timestamp;

	// new friend requests
	pthread_mutex_lock(&client->requests_mutex);
	for( i = 0 ; i < notifications->friend_request.__sizenelems ; i++ ) {
		DEBUG_INFO_PRINTF("adding request <%d, %s>",notifications->friend_request.user[i].send_date, notifications->friend_request.user[i].name.string);
		req_add_request(client->requests, notifications->friend_request.user[i].name.string, notifications->friend_request.user[i].send_date);
	}
	//req_list_set_timestamp(client->requests, notifications->last_timestamp);
	pthread_mutex_unlock(&client->requests_mutex);
	total_notifications += i;

	// new friends
	pthread_mutex_lock(&client->friends_mutex);
	for( i = 0 ; i < notifications->new_friends.__sizenelems ; i++ ) {
		DEBUG_INFO_PRINTF("adding friend <%s, %s>", notifications->new_friends.user[i].name, notifications->new_friends.user[i].information);
		fri_add_friend(client->friends, notifications->new_friends.user[i].name, notifications->new_friends.user[i].information);
	}
	//fri_set_timestamp(client->friends, notifications->last_timestamp);
	pthread_mutex_unlock(&client->friends_mutex);
	total_notifications += i;


	pthread_mutex_lock(&client->chats_mutex);
	// new chat members
	for( i = 0 ; i < notifications->chat_members.__sizenelems ; i++ ) {
		if ( (chat = cha_find_chat(client->chats, notifications->chat_members.member[i].chat_id)) == NULL ) {
			pthread_mutex_unlock(&client->chats_mutex);
			psd_recv_chats(client);
			pthread_mutex_lock(&client->chats_mutex);
			if ( (chat = cha_find_chat(client->chats, notifications->chat_members.member[i].chat_id)) == NULL ) {
				DEBUG_FAILURE_PRINTF("Crap... a member in a ghost chat");
				continue;
			}
		}
		friend = fri_find_friend(client->friends, notifications->chat_members.member[i].name.string);
		DEBUG_INFO_PRINTF("adding member <%d, %s, %s>", notifications->chat_members.member[i].chat_id, notifications->chat_members.member[i].name.string, ((friend!=NULL)? "FRIEND": "NOT FRIEND"));
		cha_add_member(chat, friend, notifications->chat_members.member[i].name.string);
	}
	// removed chat members
	for( i = 0 ; i < notifications->rem_chat_members.__sizenelems ; i++ ) {
		if ( (chat = cha_find_chat(client->chats, notifications->rem_chat_members.member[i].chat_id)) == NULL ) {
			pthread_mutex_unlock(&client->chats_mutex);
			psd_recv_chats(client);
			pthread_mutex_lock(&client->chats_mutex);
			if ( (chat = cha_find_chat(client->chats, notifications->rem_chat_members.member[i].chat_id)) == NULL ) {
				DEBUG_FAILURE_PRINTF("Crap... a member deleted from ghost chat");
				continue;
			}
		}
		DEBUG_INFO_PRINTF("removing member <%d, %s>", notifications->rem_chat_members.member[i].chat_id, notifications->rem_chat_members.member[i].name.string);
		cha_del_member(chat, notifications->rem_chat_members.member[i].name.string);
	}
	// new admins
	for( i = 0 ; i < notifications->chat_admins.__sizenelems ; i++ ) {
		if ( (chat = cha_find_chat(client->chats, notifications->chat_admins.member[i].chat_id)) == NULL ) {
			pthread_mutex_unlock(&client->chats_mutex);
			psd_recv_chats(client);
			pthread_mutex_lock(&client->chats_mutex);
			if ( (chat = cha_find_chat(client->chats, notifications->chat_admins.member[i].chat_id)) == NULL ) {
				DEBUG_FAILURE_PRINTF("Crap... an admin of a ghost chat");
				continue;
			}
		}
		DEBUG_INFO_PRINTF("changing admin <%d, %s>", notifications->chat_admins.member[i].chat_id, notifications->chat_admins.member[i].name.string);
		cha_change_admin(chat, notifications->chat_admins.member[i].name.string);
	}
	
	pthread_mutex_unlock(&client->chats_mutex);
	total_notifications += i;


	// chats with messages, Maybe the server could send the number of pending messages
	pthread_mutex_lock(&client->chats_mutex);
	for( i = 0 ; i < notifications->chats_with_messages.__sizenelems ; i++ ) {
		DEBUG_INFO_PRINTF("chat with messages <%d>",notifications->chats_with_messages.chat[i].chat_id);
		if ( (chat = cha_find_chat(client->chats, notifications->chats_with_messages.chat[i].chat_id)) == NULL ) {
			pthread_mutex_unlock(&client->chats_mutex);
			psd_recv_chats(client);
			pthread_mutex_lock(&client->chats_mutex);
			if ( (chat = cha_find_chat(client->chats, notifications->chats_with_messages.chat[i].chat_id)) == NULL ) {
				DEBUG_FAILURE_PRINTF("Crap, you have message from a ghost chat");
				continue;
			}
		}
		cha_set_pending(chat, 1);
	}
	for( i = 0 ; i < notifications->chats_read_times.__sizenelems ; i++ ) {
		if ( (chat = cha_find_chat(client->chats, notifications->chats_read_times.chat[i].chat_id)) == NULL ) {
			pthread_mutex_unlock(&client->chats_mutex);
			psd_recv_chats(client);
			pthread_mutex_lock(&client->chats_mutex);
			if ( (chat = cha_find_chat(client->chats, notifications->chats_read_times.chat[i].chat_id)) == NULL ) {
				DEBUG_FAILURE_PRINTF("Crap, you have double check info from a ghost chat");
				continue;
			}
		}
		cha_set_all_read_timestamp(chat, notifications->chats_read_times.chat[i].timestamp);
	}
	pthread_mutex_unlock(&client->chats_mutex);

	total_notifications += i;

	net_free_notification_list(notifications);

	return total_notifications;
}


int psd_recv_messages(psd_ims_client *client, int chat_id) {
	DEBUG_TRACE_PRINT();
	chat_info *chat;
	int ret_val;

	pthread_mutex_lock(&client->chats_mutex);
	if ( (chat = cha_find_chat(client->chats, chat_id)) == NULL ) {
		pthread_mutex_unlock(&client->chats_mutex);
		return -1;
	}
	ret_val = _recv_messages(client, chat);
	pthread_mutex_unlock(&client-> chats_mutex);

	return ret_val;
}


/*
 * Receive the chat's messages only if there are "pending messages"
 * Returns the number of received messages or -1 if fails
 */
int psd_recv_pending_messages(psd_ims_client *client, int chat_id) {
	DEBUG_TRACE_PRINT();
	chat_info *chat;
	int ret_val;

	pthread_mutex_lock(&client->chats_mutex);
	if ( (chat = cha_find_chat(client->chats, chat_id)) == NULL ) {
		pthread_mutex_unlock(&client->chats_mutex);
		return -1;
	}
	
	if (cha_pending(chat) <= 0 ) {
		pthread_mutex_unlock(&client->chats_mutex);
		return 0;
	}
	ret_val = _recv_messages(client, chat);
	pthread_mutex_unlock(&client-> chats_mutex);

	return ret_val;
}


/*
 * Receive the chats' messages
 * Returns the number of received messages or -1 if fails
 */
int psd_recv_all_messages(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	chat_iterator *iterator;
	chat_info *chat;
	int ret_val = 0;
	int ret;

	pthread_mutex_lock(&client->chats_mutex);
	iterator = cha_get_chats_iterator(client->chats);
	while( iterator != NULL ) {
		chat = cha_get_info(iterator);
		if( (ret = _recv_messages(client, chat)) < 0 ) {
			DEBUG_FAILURE_PRINTF("Could not get the chat messages");
			ret_val = ret;
			cha_iterator_next(client->chats, iterator);
			continue;
		}
		iterator = cha_iterator_next(client->chats, iterator);
	}
	pthread_mutex_unlock(&client->chats_mutex);

	return ret_val;
}


/*
 * Receive the chat's messages only if there are "pending messages"
 * Returns the number of received messages or -1 if fails
 */
int psd_recv_all_pending_messages(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	chat_iterator *iterator;
	chat_info *chat;
	int ret_val = 0;
	int ret;

	pthread_mutex_lock(&client->chats_mutex);
	iterator = cha_get_chats_iterator(client->chats);
	while( iterator != NULL ) {
		chat = cha_get_info(iterator);
		if (cha_pending(chat) <= 0 ) {
			cha_iterator_next(client->chats, iterator);
			continue;
		}
		if( (ret = _recv_messages(client, chat)) < 0 ) {
			DEBUG_FAILURE_PRINTF("Could not get the chat messages");
			ret_val = ret;
			cha_iterator_next(client->chats, iterator);
			continue;
		}
		cha_iterator_next(client->chats, iterator);
	}
	pthread_mutex_unlock(&client->chats_mutex);

	return ret_val;
}


/*
 *
 *
 */
int psd_recv_message_attachment(psd_ims_client *client, int chat_id, int msg_timestamp) {
	DEBUG_TRACE_PRINT();
	
	psdims__file *file;
	char file_path[MAX_FILE_PATH_CHARS];
	FILE *fd;
	struct stat st;

	pthread_mutex_lock(&client->network_mutex);
	if( (file = net_get_attachment(client->network, chat_id, msg_timestamp)) == NULL ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not receive the chat list");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);
	
	if( stat(ATTACH_FILES_DIR_RCV, &st) == -1 ) {
		mkdir(ATTACH_FILES_DIR_RCV, 0700);
	}
	
	// Create the file path
	create_file_path_rcv(file_path, chat_id, msg_timestamp);

	// Write the file in the disk
	if( (fd = fopen(file_path, "w")) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not create the file");
		net_free_file(file);
		return -1;
	}
	if( fwrite(file->__ptr, file->__size, 1, fd) != 1 ) {
		DEBUG_FAILURE_PRINTF("Could not save the received file");
		fclose(fd);
		net_free_file(file);
		return -1;
	}

	fclose(fd);

	net_free_file(file);

	return 0;
}


/*
 * Receive the chat list
 * Returns 0 or -1 if fails
 */
int psd_recv_chats(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	int i, j;
	int n_chats;
	psdims__chat_list *chats;
	friend_info **members;
	char **member_names;
	int chat_timestamp;

	pthread_mutex_lock(&client->chats_mutex);
	cha_get_timestamp(client->chats, chat_timestamp);
	pthread_mutex_unlock(&client->chats_mutex);
	
	pthread_mutex_lock(&client->network_mutex);
	if( (chats = net_get_chat_list(client->network, chat_timestamp)) == NULL ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not receive the chat list");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	for( i = 0 ; i < chats->__sizenelems ; i++ ) {
		DEBUG_INFO_PRINTF("Adding chat <%d, %s>", chats->chat_info[i].chat_id, chats->chat_info[i].description);
		// Alloc member list
		if ( (members = malloc(sizeof(friend_info *)*chats->chat_info[i].members.__sizenelems)) == NULL ) {
			DEBUG_FAILURE_PRINTF("Could not allocate memory for temp member list");
			net_free_chat_list(chats);
			return -1;
		}
		if ( (member_names = malloc(sizeof(char *)*chats->chat_info[i].members.__sizenelems)) == NULL ) {
			DEBUG_FAILURE_PRINTF("Could not allocate memory for temp member list");
			net_free_chat_list(chats);
			free(members);
			return -1;
		}
	
		for ( j = 0 ; j < chats->chat_info[i].members.__sizenelems ; j ++ ) {
			members[j] = fri_find_friend(client->friends, chats->chat_info[i].members.name[j].string);
			member_names[j] = chats->chat_info[i].members.name[j].string;
			DEBUG_INFO_PRINTF("Member <%d, %s, %s>", j , member_names[j], ((members[j] != NULL)? "FRIEND" : "NOT FRIEND") );
		}
		pthread_mutex_unlock(&client->friends_mutex);
		
		pthread_mutex_lock(&client->chats_mutex);
		if ( cha_add_chat(client->chats, chats->chat_info[i].chat_id, chats->chat_info[i].description, 
				chats->chat_info[i].admin, members, member_names, chats->chat_info[i].members.__sizenelems, 
				MAX_MEMBERS, MAX_MESSAGES, chats->chat_info[i].read_timestamp, chats->chat_info[i].all_read_timestamp) != 0 ) {
			DEBUG_FAILURE_PRINTF("Could not add chat");
		}
	
		pthread_mutex_unlock(&client->chats_mutex);

		// Free member list
		free(members);
		free(member_names);
	}
	
	pthread_mutex_lock(&client->chats_mutex);
	cha_set_timestamp(client->chats, chats->last_timestamp);
	pthread_mutex_unlock(&client->chats_mutex);
	
	n_chats = chats->__sizenelems;
	net_free_chat_list(chats);

	return n_chats;
}


/*
 * Receive the friend list
 * Returns 0 or -1 if fails
 */
int psd_recv_friends(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	int n_friends;
	psdims__user_list *friends;

	int i;
	int friends_timestamp;

	pthread_mutex_lock(&client->friends_mutex);
	fri_get_timestamp(client->friends, friends_timestamp);
	pthread_mutex_unlock(&client->friends_mutex);

	pthread_mutex_lock(&client->network_mutex);
	if( (friends = net_get_friend_list(client->network, friends_timestamp)) == NULL ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not receive the chat list");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	pthread_mutex_lock(&client->friends_mutex);
	for( i = 0 ; i < friends->__sizenelems; i ++ ) {
		if( fri_add_friend(client->friends, friends->user[i].name, friends->user[i].information) == -1 ) {
			DEBUG_FAILURE_PRINTF("Could not add friend");
			break;
		}	
	}
	if ( i < friends->__sizenelems ) {
		for( i = i-1 ; i >= 0 ; i-- ) {
			if (fri_del_friend(client->friends, friends->user[i].name) == -1 ) {
				DEBUG_FAILURE_PRINTF("CRITICAL: could not restore, the friend list is probably inconsistent");
				pthread_mutex_unlock(&client->friends_mutex);
				net_free_user_list(friends);
				return -1;
			}
		}
		pthread_mutex_unlock(&client->friends_mutex);
		net_free_user_list(friends);
		return -1;
	}
	fri_set_timestamp(client->friends, friends->last_timestamp);
	pthread_mutex_unlock(&client->friends_mutex);

	n_friends = friends->__sizenelems;

	net_free_user_list(friends);

	return n_friends;
}


/*
 * Creates a new chat
 * Returns the chat id or -1 if fails
 */
int psd_create_chat(psd_ims_client *client, char *description, char *member) {

	int chat_id;
	int n_members = 0;
	//friend_info *friend = NULL;

	// check if "member" is a friend of the user
	if (member != NULL) {
		n_members = 1;
		pthread_mutex_lock(&client->friends_mutex);
		if ( fri_find_friend(client->friends, member) == NULL ) {
			pthread_mutex_unlock(&client->friends_mutex);
			DEBUG_FAILURE_PRINTF("The member is not a friend");
			return -1;
		} 
		pthread_mutex_unlock(&client->friends_mutex);
	}

	pthread_mutex_lock(&client->network_mutex);
	if( net_create_chat(client->network, description, member, &chat_id) != 0 ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not create the chat");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

/*
	if ( cha_add_chat(client->chats, chat_id, description, NULL, &friend, &member, n_members, MAX_MEMBERS, MAX_MESSAGES, 0) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not create the chat locally, but it has been sended to the server");
		return -1;
	}
*/
	return chat_id;
}


/*
 *
 *
 */
int psd_add_member_to_chat(psd_ims_client *client, char *member, int chat_id) {
	DEBUG_TRACE_PRINT();
	chat_info *chat = NULL;
	friend_info *friend = NULL;

	pthread_mutex_lock(&client->network_mutex);
	if( net_add_user_to_chat(client->network, member, chat_id) != 0 ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not add the member to the chat");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);
	return 0;
}


/*
 *
 *
 */
int psd_del_member_from_chat(psd_ims_client *client, char *member, int chat_id) {
	DEBUG_TRACE_PRINT();
	chat_info *chat = NULL;
	friend_info *friend = NULL;

	pthread_mutex_lock(&client->network_mutex);
	if( net_remove_user_from_chat(client->network, member, chat_id) != 0 ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not remove the member from the chat");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);
	return 0;
}


/*
 *
 *
 */
int psd_quit_from_chat(psd_ims_client *client, int chat_id) {
	DEBUG_TRACE_PRINT();

	pthread_mutex_lock(&client->network_mutex);
	if( net_quit_from_chat(client->network, chat_id) != 0 ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not quit from chat");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	return 0;
}


/*
 *
 *
 */
int psd_remove_chat(psd_ims_client *client, int chat_id) {
	DEBUG_TRACE_PRINT();
	
	pthread_mutex_lock(&client->chats_mutex);
	if( cha_del_chat(client->chats, chat_id) != 0 ) {
		pthread_mutex_unlock(&client->chats_mutex);
		DEBUG_FAILURE_PRINTF("Could not remove the chat locally, but you are not longer in it");
		return -1;
	}
	pthread_mutex_unlock(&client->chats_mutex);
	return 0;
}

/*
 * Send a message to the chat "chat_id"
 * Returns 0 or -1 if fails
 */
int psd_send_message(psd_ims_client *client, int chat_id, char *text, char *file_path, char *file_info) {
	DEBUG_TRACE_PRINT();
	
	int send_timestamp = 0;
	char file_path_internal[MAX_FILE_PATH_CHARS];
	char * file_buff = NULL;
	char * file_buff_aux;
	FILE *fd;
	struct stat st;
	int written_blocks = 0;
	int readed_blocks = 0;
	int total_blocks = 0;
	chat_info *chat;
	
	pthread_mutex_lock(&client->chats_mutex);
	if( (chat = cha_find_chat(client->chats, chat_id)) == NULL ) {
		DEBUG_FAILURE_PRINTF("The chat does not exist");
		return -1;
	}
	pthread_mutex_unlock(&client->chats_mutex);

	/* Read the attached file */
	DEBUG_INFO_PRINTF("Reading the attached file");
	if( file_path != NULL ) {
		file_buff = malloc( sizeof(char)*MAX_FILE_CHARS);
		if( (fd = fopen(file_path, "r")) == NULL ) {
			DEBUG_FAILURE_PRINTF("Could not read the file");
			return -1;
		}

		file_buff_aux = file_buff;
		do {
			readed_blocks = fread(file_buff_aux++, 1, 1, fd);
			total_blocks += readed_blocks;
		} while( (readed_blocks == 1) && (total_blocks < MAX_FILE_CHARS) );

		fclose(fd);
	}

	/* Send the message */
	DEBUG_INFO_PRINTF("Sending the message");
	pthread_mutex_lock(&client->network_mutex);
	if( net_send_message(client->network, chat_id, text, file_info, &send_timestamp) != 0 ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not send the message");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	/* Copy the attached file to an internal directory */
	DEBUG_INFO_PRINTF("Copying the file in the internal directory");
	if( file_path != NULL ) {
		
		// Create the file path
		create_file_path_snd(file_path_internal, chat_id, send_timestamp);

		if( stat(ATTACH_FILES_DIR_SND, &st) == -1 ) {
			mkdir(ATTACH_FILES_DIR_SND, 0700);
		}

		// Copy the attached file
		if( (fd = fopen(file_path_internal, "w")) == NULL ) {
			DEBUG_FAILURE_PRINTF("Could not open the file");
			return -1;
		}

		written_blocks = 0;
		file_buff_aux = file_buff;
		do {
			written_blocks += fwrite(file_buff_aux++, 1, 1, fd);
		} while( written_blocks < total_blocks);

		fclose(fd);
	}
	
	// Send the attachment
	if( file_path != NULL ) {
		pthread_mutex_lock(&client->network_mutex);
		if( net_send_attachment(client->network, chat_id, send_timestamp, file_buff, sizeof(char)*total_blocks) != 0 ) {
			pthread_mutex_unlock(&client->network_mutex);
			DEBUG_FAILURE_PRINTF("Could not send the message");
			return -1;
		}
		pthread_mutex_unlock(&client->network_mutex);
	}

	free(file_buff);

	return 0;
}


/*
 * Send a friend request to "user"
 * Returns 0 or -1 if fails
 */
int psd_send_friend_request(psd_ims_client *client, char *user) {
	DEBUG_TRACE_PRINT();
	int send_timestamp = 0;

	pthread_mutex_lock(&client->network_mutex);
	if ( net_send_friend_request(client->network, user, &send_timestamp) != 0 ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not send the friend request");	
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);
	
	return 0;
}


/*
 * Accept a friend request from "user"
 * Returns 0 or -1 if fails
 */
int psd_send_request_accept(psd_ims_client *client, char *user) {
	DEBUG_TRACE_PRINT();

	psdims__user_info *user_info;
	int send_date = 0;

	// send the request accept
	pthread_mutex_lock(&client->network_mutex);
	if ( net_send_request_accept(client->network, user, &send_date) != 0 ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not accept the friend request");	
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	// delete the local friend request
	pthread_mutex_lock(&client->requests_mutex);
	if ( req_del_request(client->requests, user) != 0 ) {
		pthread_mutex_unlock(&client->requests_mutex);
		DEBUG_FAILURE_PRINTF("Could not remove the request locally, but it has been accepted");
		return -1;
	}
	pthread_mutex_unlock(&client->requests_mutex);	

	return 0;
}


/*
 * Reject a friend request from "user"
 * Returns 0 or -1 if fails
 */
int psd_send_request_decline(psd_ims_client *client, char *user) {
	DEBUG_TRACE_PRINT();

	pthread_mutex_lock(&client->network_mutex);
	if ( net_send_request_decline(client->network, user) != 0 ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not accept the friend request");	
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	pthread_mutex_lock(&client->requests_mutex);
	if ( req_del_request(client->requests, user) != 0 ) {
		pthread_mutex_unlock(&client->requests_mutex);
		DEBUG_FAILURE_PRINTF("Could not remove the request locally, but it has been declined");
		return -1;
	}
	pthread_mutex_unlock(&client->requests_mutex);
	
	return 0;
}


/*
 * Check if the chat exists in list
 */
boolean psd_chat_exists(psd_ims_client *client, int chat_id) {
	return cha_find_chat(client->chats, chat_id)? TRUE : FALSE;
}
