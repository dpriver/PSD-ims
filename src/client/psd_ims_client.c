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
#include "pending_chats.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "debug_def.h"

#ifdef DEBUG
#include "leak_detector_c.h"
#endif


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

	client->friends = fri_new();
	client->chats = cha_new();
	client->network = net_new();
	pen_init(&client->new_chats);

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
	cha_free(client->chats);
	net_free(client->network);	
	pen_free(&client->new_chats);

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

	if ( (user_name = malloc(strlen(name) + sizeof(char))) == NULL ) {
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

	if ( (user_pass = malloc(strlen(pass) + sizeof(char))) == NULL ) {
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

	if ( (client->user_name = malloc( strlen(name)+sizeof(char) )) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory");
		return -1;
	}
	if ( (client->user_pass = malloc( strlen(password)+sizeof(char) )) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory");
		return -1;
	}
	if ( (client->user_info = malloc( strlen(user_info->information)+sizeof(char) )) == NULL ) {
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
 * Receive the pending notifications
 * Returns the number of received notifications or -1 if fails
 */
int psd_recv_notifications(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	int i;
	psdims__notifications *notifications;
	int total_notifications = 0;

	pthread_mutex_lock(&client->network_mutex);
	if( (notifications = net_recv_notifications(client->network, client->last_notif_timestamp)) == NULL ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not receive the notifications");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	// new friend requests
	DEBUG_INFO_PRINTF("Adding friend requests");
	pthread_mutex_lock(&client->friends_mutex);
	for( i = 0 ; i < notifications->friend_request.__sizenelems ; i++ ) {
		fri_add_rcv_request(client->friends, notifications->friend_request.user[i].name.string, notifications->friend_request.user[i].send_date);
	}
	total_notifications += i;
	// deleted friends TODO: Delete the needed structs
	DEBUG_INFO_PRINTF("Deleting friends");
	for( i = 0 ; i < notifications->deleted_friends.__sizenelems ; i++ ) {
		fri_del_friend(client->friends, notifications->deleted_friends.user[i].name.string);
	}
	pthread_mutex_unlock(&client->friends_mutex);
	// new chats
	DEBUG_INFO_PRINTF("Adding new chats");
	pthread_mutex_lock(&client->new_chats_mutex);
	for( i = 0 ; i < notifications->new_chats.__sizenelems ; i++ ) {
		pen_lst_add(&client->new_chats, notifications->new_chats.chat[i].chat_id);
	}
	pthread_mutex_unlock(&client->new_chats_mutex);
	// deleted chats
	DEBUG_INFO_PRINTF("Deleting chats");
	pthread_mutex_lock(&client->chats_mutex);
	for( i = 0 ; i < notifications->deleted_chats.__sizenelems ; i++ ) {
		cha_del_chat(client->chats, notifications->deleted_chats.chat[i].chat_id);
	}
	// chats with messages, Maybe the server could send he number of pending messages
	DEBUG_INFO_PRINTF("Adding new messages to chats");
	for( i = 0 ; i < notifications->chats_with_messages.__sizenelems ; i++ ) {
		cha_set_pending(client->chats, notifications->chats_with_messages.chat[i].chat_id, 1);
	}
	pthread_mutex_unlock(&client->chats_mutex);

	total_notifications += i;

	net_free_notification_list(notifications);

	return total_notifications;
}


/*
 * Receive the chat's messages (the "pending" counter is sets to 0
 * Returns the number of received messages or -1 if fails
 */
int psd_recv_messages(psd_ims_client *client, int chat_id) {
	DEBUG_TRACE_PRINT();
	psdims__message_list *list;
	char **sender;
	char **text;
	char **attach_path;
	int *send_date;

	int n_messages;
	int i;
	int timestamp;

	pthread_mutex_lock(&client->chats_mutex);
	timestamp = cha_get_last_message_date(client->chats, chat_id);
	pthread_mutex_unlock(&client->chats_mutex);

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
	send_date = (int*)malloc(sizeof(char)*list->__sizenelems);


	for( i = 0; i < list->__sizenelems; i++) {
		sender[i] = list->messages[i].user;
		text[i] = list->messages[i].text;
		attach_path[i] = NULL;	
		send_date[i] = list->messages[i].send_date;
	}

	n_messages = list->__sizenelems;
	net_free_message_list(list);

	pthread_mutex_lock(&client->chats_mutex);
	if ( cha_add_messages(client->chats, chat_id, sender, text, send_date, attach_path, list->__sizenelems) != 0 ) {
		pthread_mutex_unlock(&client->chats_mutex);
		DEBUG_FAILURE_PRINTF("Could not add messages");
		return -1;
	}

	cha_set_pending(client->chats, chat_id, 0);
	cha_update_unread(client->chats, chat_id, list->__sizenelems);
	pthread_mutex_unlock(&client->chats_mutex);

	return n_messages;
}


/*
 * Receive the chat's messages only if there are "pending messages"
 * Returns the number of received messages or -1 if fails
 */
int psd_recv_pending_messages(psd_ims_client *client, int chat_id) {
	DEBUG_TRACE_PRINT();

	pthread_mutex_lock(&client->chats_mutex);
	if (cha_get_pending(client->chats, chat_id) <= 0 ) {
		pthread_mutex_unlock(&client->chats_mutex);
		return 0;
	}
	pthread_mutex_unlock(&client-> chats_mutex);

	return psd_recv_messages(client, chat_id);
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
	friend_info *admin;
	friend_info **members;
	char **member_names;

	pthread_mutex_lock(&client->network_mutex);
	if( (chats = net_get_chat_list(client->network, client->chats_timestamp)) == NULL ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not receive the chat list");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	for( i = 0 ; i < chats->__sizenelems ; i++ ) {
		DEBUG_INFO_PRINTF("Chat number: %d", i);
		// Alloc member list
		if ( (members = malloc(sizeof(friend_info *)*chats->chat_info[i].members->__sizenelems)) == NULL ) {
			DEBUG_FAILURE_PRINTF("Could not allocate memory for temp member list");
			return -1;
		}
		if ( (member_names = malloc(sizeof(char *)*chats->chat_info[i].members->__sizenelems)) == NULL ) {
			DEBUG_FAILURE_PRINTF("Could not allocate memory for temp member list");
			return -1;
		}

		pthread_mutex_lock(&client->friends_mutex);
		admin = fri_find_friend(client->friends, chats->chat_info[i].admin );
	
		//DEBUG_INFO_PRINTF("Number of friends: %d", chats->chat_info[i].members->__sizenelems);
		for ( j = 0 ; j < chats->chat_info[i].members->__sizenelems ; j ++ ) {
			//DEBUG_INFO_PRINTF("Looking for friend: %s", chats->chat_info[i].members->name[j].string);
			members[j] = fri_find_friend(client->friends, chats->chat_info[i].members->name[j].string);
			member_names[j] = chats->chat_info[i].members->name[j].string;
		}
		pthread_mutex_unlock(&client->friends_mutex);
		
		DEBUG_INFO_PRINTF("Chat: %d, %s", chats->chat_info[i].chat_id, chats->chat_info[i].description);

		pthread_mutex_lock(&client->chats_mutex);
		if ( cha_add_chat(client->chats, chats->chat_info[i].chat_id, chats->chat_info[i].description, admin, members, chats->chat_info[i].admin, member_names, chats->chat_info[i].members->__sizenelems) != 0 ) {
			pthread_mutex_unlock(&client->chats_mutex);
			DEBUG_FAILURE_PRINTF("Could not add chat");
			return -1;
		}
	
		pthread_mutex_unlock(&client->chats_mutex);

		// Free member list
		free(members);
		free(member_names);
	}
	
	pthread_mutex_lock(&client->chats_mutex);
	client->chats_timestamp = chats->last_timestamp;
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

	pthread_mutex_lock(&client->network_mutex);
	if( (friends = net_get_friend_list(client->network, client->friends_timestamp)) == NULL ) {
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
				pthread_mutex_lock(&client->friends_mutex);
				return -1;
			}
		}
	}
	pthread_mutex_unlock(&client->friends_mutex);

	pthread_mutex_lock(&client->friends_mutex);
	client->friends_timestamp = friends->last_timestamp;
	pthread_mutex_unlock(&client->friends_mutex);

	n_friends = friends->__sizenelems;

	net_free_user_list(friends);

	return n_friends;
}


/*
 * Receive the locally not registered chats
 * Returns the number of created chats or -1 if fails
 */
int psd_update_chats(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	DEBUG_FAILURE_PRINTF("Not implemented");
	return -1;
}


/*
 * Receive the locally not registered chats
 * Returns the number of created chats or -1 if fails
 */
int psd_update_friends(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	DEBUG_FAILURE_PRINTF("Not implemented");
	return -1;
}


/*
 * Receive the locally not registered chats
 * Returns the number of created chats or -1 if fails
 */
int psd_recv_new_chats(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	DEBUG_FAILURE_PRINTF("Not implemented");
	return -1;
}


/*
 * Creates a new chat
 * Returns 0 or -1 if fails
 */
int psd_create_chat(psd_ims_client *client, char *description, char *member) {

	int chat_id;
	int n_members;

	// I could check if "member" is a friend of the user

	pthread_mutex_lock(&client->network_mutex);
	if( net_create_chat(client->network, description, member, &chat_id) != 0 ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not create the chat");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	n_members = (member == NULL)? 0: 1;

	if ( psd_add_chat(client, chat_id, description, client->user_name, &member, n_members) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not create the chat locally, but it has been sended to the server");
		return -1;
	}

	return 0;
}


/*
 * Send a message to the chat "chat_id"
 * Returns 0 or -1 if fails
 */
int psd_send_message(psd_ims_client *client, int chat_id, char *text, char *attach_path) {
	DEBUG_TRACE_PRINT();
	int send_date = 0;

	pthread_mutex_lock(&client->network_mutex);
	if( net_send_message(client->network, chat_id, text, attach_path, &send_date) != 0 ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not send the message");
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	pthread_mutex_lock(&client->chats_mutex);
	if ( cha_add_message(client->chats, chat_id, NULL, text, send_date, attach_path) != 0 ) {
		pthread_mutex_unlock(&client->chats_mutex);
		DEBUG_FAILURE_PRINTF("Could not add the message, but it has been sended");
		return -1;
	}
	pthread_mutex_unlock(&client->chats_mutex);

	return 0;
}


/*
 * Send a friend request to "user"
 * Returns 0 or -1 if fails
 */
int psd_send_friend_request(psd_ims_client *client, char *user) {
	DEBUG_TRACE_PRINT();
	int send_date = 0;

	pthread_mutex_lock(&client->network_mutex);
	if ( net_send_friend_request(client->network, user, &send_date) != 0 ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not send the friend request");	
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	if ( psd_add_friend_req_snd(client, user, send_date) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not add the request locally, but it has been sended");
		return -1;
	}
	
	return 0;
}


/*
 * Accept a friend request from "user"
 * Returns 0 or -1 if fails
 */
int psd_send_request_accept(psd_ims_client *client, char *user) {
	DEBUG_TRACE_PRINT();

	int send_date = 0;

	pthread_mutex_lock(&client->network_mutex);
	if ( net_send_request_accept(client->network, user, &send_date) != 0 ) {
		pthread_mutex_unlock(&client->network_mutex);
		DEBUG_FAILURE_PRINTF("Could not accept the friend request");	
		return -1;
	}
	pthread_mutex_unlock(&client->network_mutex);

	pthread_mutex_lock(&client->friends_mutex);
	if ( fri_del_rcv_request(client->friends, user) != 0 ) {
		pthread_mutex_unlock(&client->friends_mutex);
		DEBUG_FAILURE_PRINTF("Could not remove the request locally, but it has been accepted");
		return -1;
	}
	pthread_mutex_unlock(&client->friends_mutex);	

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

	pthread_mutex_lock(&client->friends_mutex);
	if ( fri_del_rcv_request(client->friends, user) != 0 ) {
		pthread_mutex_unlock(&client->friends_mutex);
		DEBUG_FAILURE_PRINTF("Could not remove the request locally, but it has been declined");
		return -1;
	}
	pthread_mutex_unlock(&client->friends_mutex);
	
	return 0;
}


/* =========================================================================
 *  Chats
 * =========================================================================*/

/*
 * Prints all chats line by line
 */
void psd_print_chats(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	pthread_mutex_lock(&client->chats_mutex);
	cha_print_chat_list(client->chats);
	pthread_mutex_unlock(&client->chats_mutex);
}

/*
 * Prints all chat members line by line
 */
void psd_print_chat_members(psd_ims_client *client, int chat_id) {
	DEBUG_TRACE_PRINT();
	pthread_mutex_lock(&client->chats_mutex);
	cha_print_chat_members(client->chats, chat_id);
	pthread_mutex_unlock(&client->chats_mutex);
}

/*
 * Adds a new chat to client's chat list
 * Returns 0 or -1 if fails
 */
int psd_add_chat(psd_ims_client *client, int id, const char *description, const char *admin,
			char *members[], int n_members) {
	DEBUG_TRACE_PRINT();
	int i;
	friend_info *aux_admin;
	friend_info **aux_friend_list;

	if ( strcmp(admin, client->user_name) == 0 ) {
		aux_admin = NULL;
	}
	else {
		pthread_mutex_lock(&client->friends_mutex);
		if ( (aux_admin = fri_find_friend(client->friends, admin)) == NULL ) {
			pthread_mutex_unlock(&client->friends_mutex);
			aux_admin = NULL;
			return -1; // chat admin is not a friend
		}
		pthread_mutex_unlock(&client->friends_mutex);
	}

	aux_friend_list = malloc( sizeof(friend_info)*n_members );

	pthread_mutex_lock(&client->friends_mutex);
	for( i = 0 ; i < n_members ; i++) {
		aux_friend_list[i] = fri_find_friend(client->friends, members[i]);
	}
	pthread_mutex_unlock(&client->friends_mutex);

	pthread_mutex_lock(&client->chats_mutex);
	if( cha_add_chat(client->chats, id, description, aux_admin, aux_friend_list, admin, members, n_members) == -1 ) {
		pthread_mutex_unlock(&client->chats_mutex);
		free(aux_friend_list);
		return -1;
	}
	pthread_mutex_unlock(&client->chats_mutex);

	free(aux_friend_list);
	return 0;
}


/*
 * Removes and frees the first chat that matches "chat_ic"
 * Returns 0 or -1 if "chat_id" does not exist in the list
 */
int psd_del_chat(psd_ims_client *client, int chat_id) {
	DEBUG_TRACE_PRINT();
	int ret_value;

	pthread_mutex_lock(&client->chats_mutex);
	ret_value = cha_del_chat(client->chats, chat_id);
	pthread_mutex_unlock(&client->chats_mutex);

	return ret_value;
}


/*
 * Adds the member "user_name" to the chat "chat_id"
 * Returns 0 or -1 if fails
 */
int psd_add_friend_to_chat(psd_ims_client *client, int chat_id, const char *user_name) {
	DEBUG_TRACE_PRINT();
	friend_info *friend_info;

	// find friend in client->friends
	pthread_mutex_lock(&client->friends_mutex);
	if ( (friend_info = fri_find_friend(client->friends, user_name)) == NULL ) {
		pthread_mutex_unlock(&client->friends_mutex);
		return -1; // friend does not exist
	}
	pthread_mutex_unlock(&client->friends_mutex);

	pthread_mutex_lock(&client->chats_mutex);
	if ( cha_add_member(client->chats, chat_id, friend_info, user_name) == -1 ) {
		pthread_mutex_unlock(&client->chats_mutex);
		return -1; // could not add member to chat
	}
	pthread_mutex_unlock(&client->chats_mutex);

	return 0;
}


/*
 * Deletes the first ocurrence of a chat member with the provided "name" from the cha "chat_id"
 * Returns 0 or -1 if fails
 */
int psd_del_friend_from_chat(psd_ims_client *client, int chat_id, const char *user_name) {
	DEBUG_TRACE_PRINT();
	int ret_value;

	pthread_mutex_lock(&client->chats_mutex);
	ret_value = cha_del_member(client->chats, chat_id, user_name);
	pthread_mutex_unlock(&client->chats_mutex);

	return ret_value;
}


/*
 * Adds the messages in the chat
 * Returns 0 or -1 if fails
 */
int psd_add_messages(psd_ims_client *client, int chat_id, char *sender[], char *text[], int send_date[], char *attach_path[], int n_messages) {
	DEBUG_TRACE_PRINT();
	int ret_value;

	pthread_mutex_lock(&client->chats_mutex);
	ret_value = cha_add_messages(client->chats, chat_id, sender, text, send_date, attach_path, n_messages);
	pthread_mutex_unlock(&client->chats_mutex);

	return ret_value;
}


/*
 * Get the number of pending messages
 */
int psd_get_n_pending_messages(psd_ims_client *client, int chat_id) {
	DEBUG_TRACE_PRINT();
	DEBUG_FAILURE_PRINTF("Not implemented");
	return -1;
}


/*
 * Clears the chat pending messages counter
 */
int psd_clean_pending_messages(psd_ims_client *client, int chat_id) {
	DEBUG_TRACE_PRINT();
	int ret_value;

	pthread_mutex_lock(&client->chats_mutex);
	ret_value = cha_set_unread(client->chats, chat_id, 0);
	pthread_mutex_unlock(&client->chats_mutex);

	return ret_value;
}


/*
 * Updates the chat pending messages counter
 */
int psd_update_pending_messages(psd_ims_client *client, int chat_id, int n_messages) {
	DEBUG_TRACE_PRINT();
	int ret_value;

	pthread_mutex_lock(&client->chats_mutex);
	ret_value = cha_update_unread(client->chats, chat_id, n_messages);
	pthread_mutex_unlock(&client->chats_mutex);

	return ret_value;
}


/*
 * Switches the current admin with the chat member named "user_name"
 * that means that the previous admin becomes a normal member
 * Returns 0 or -1 if fails
 */
int psd_change_chat_admin(psd_ims_client *client, int chat_id, const char *user_name) {
	DEBUG_TRACE_PRINT();
	int ret_value;

	pthread_mutex_lock(&client->chats_mutex);
	ret_value = cha_change_admin(client->chats, chat_id, user_name);
	pthread_mutex_unlock(&client->chats_mutex);

	return ret_value;
}


/* =========================================================================
 *  Friends
 * =========================================================================*/

/*
 * Prints all friends line by line
 */
void psd_print_friends(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	
	pthread_mutex_lock(&client->friends_mutex);
	fri_print_friend_list(client->friends);
	pthread_mutex_unlock(&client->friends_mutex);
}


/*
 * Prints all friends requests line by line
 */
void psd_print_friend_requests(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();

	pthread_mutex_lock(&client->friends_mutex);
	fri_print_rcv_request_list(client->friends);
	pthread_mutex_unlock(&client->friends_mutex);	
}


/*
 * Adds a new friend to client's friend list
 * Returns 0 or -1 if fails
 */
int psd_add_friend(psd_ims_client *client, const char *name, const char *information) {
	DEBUG_TRACE_PRINT();
	int ret_value;

	pthread_mutex_lock(&client->friends_mutex);
	ret_value = fri_add_friend(client->friends, name, information);
	pthread_mutex_unlock(&client->friends_mutex);	

	return ret_value;
}


/*
 * Adds a sended friend request to "name"
 */
int psd_add_friend_req_snd(psd_ims_client *client, const char *name, int send_date) {
	DEBUG_TRACE_PRINT();
	int ret_value;

	pthread_mutex_lock(&client->friends_mutex);
	ret_value = fri_add_snd_request(client->friends, name, send_date);
	pthread_mutex_unlock(&client->friends_mutex);	

	return ret_value;
}	


/*
 * Adds a received friend request from "name"
 */
int psd_add_friend_req_rcv(psd_ims_client *client, const char *name, int send_date) {
	DEBUG_TRACE_PRINT();
	int ret_value;

	pthread_mutex_lock(&client->friends_mutex);
	ret_value = fri_add_rcv_request(client->friends, name, send_date);
	pthread_mutex_unlock(&client->friends_mutex);	

	return ret_value;
}


/*
 * Removes and frees the first friend that matches "name"
 * Returns 0 or -1 if "name" does not exist in the list
 */
int psd_del_friend(psd_ims_client *client, const char *name) {
	DEBUG_TRACE_PRINT();
	int ret_value;

	pthread_mutex_lock(&client->friends_mutex);
	ret_value = fri_del_friend(client->friends, name);
	pthread_mutex_unlock(&client->friends_mutex);	

	return ret_value;
}



