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

#ifdef DEBUG_TRACE
	#undef DEBUG_TRACE
#endif
#ifdef DEBUG_INFO
	#undef DEBUG_INFO
#endif

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

	client->friends = fri_new();
	client->chats = cha_new();
	client->network = net_new("url");
	pen_init(&client->new_chats);

	return client;
}


/*
 * Removes and frees the client struct
 */
void psd_free_client(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
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
 * Gets the user information from the server
 * Returns 0 or -1 if fails
 */
int psd_login(psd_ims_client *client, char *name, char *password) {
	DEBUG_TRACE_PRINT();
	psdims__user_info *user_info;

	if( (user_info = net_login(client->network, name, password)) == NULL ) {
		DEBUG_FAILURE_PRINTF("Bad login");
		return -1;
	}
	
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

	return 0;
}


/*
 * Register the user in the system
 * Returns 0 or -1 if fails
 */
int psd_user_register(psd_ims_client *client, char *name, char *password, char *information) {
	DEBUG_TRACE_PRINT();

	if( net_user_register(client->network, name, password, information) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not register the user");
		return -1;
	}

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

	if( (notifications = net_recv_notifications(client->network, client->last_notif_timestamp)) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not register the user");
		return -1;
	}

	// new friend requests
	for( i = 0 ; i < notifications->friend_request.__sizenelems ; i++ ) {
		fri_add_rcv_request(client->friends, notifications->friend_request.user[i].name.string, notifications->friend_request.user[i].send_date);
	}
	total_notifications += i;
	// deleted friends TODO: Delete the needed structs
	for( i = 0 ; i < notifications->deleted_friends.__sizenelems ; i++ ) {
		fri_del_friend(client->friends, notifications->deleted_friends.user[i].name.string);
	}
	// new chats
	for( i = 0 ; i < notifications->new_chats.__sizenelems ; i++ ) {
		pen_lst_add(&client->new_chats, notifications->new_chats.chat[i].chat_id);
	}
	// deleted chats
	for( i = 0 ; i < notifications->deleted_chats.__sizenelems ; i++ ) {
		cha_del_chat(client->chats, notifications->deleted_chats.chat[i].chat_id);
	}
	// chats with messages, Maybe the server could send he number of pending messages
	for( i = 0 ; i < notifications->chats_with_messages.__sizenelems ; i++ ) {
		cha_set_pending(client->chats, notifications->chats_with_messages.chat[i].chat_id, 1);
	}
	total_notifications += i;

	return total_notifications;
}


/*
 * Receive the pending messages
 * Returns the number of received messages or -1 if fails
 */
int psd_recv_pending_messages(psd_ims_client *client, int chat_id) {
	DEBUG_TRACE_PRINT();
	psdims__message_list *list;
	char **sender;
	char **text;
	char **attach_path;
	int *send_date;

	int i;
	int timestamp;

	if (cha_get_pending(client->chats, chat_id) <= 0 ) {
		return 0;
	}

	timestamp = cha_get_last_message_date(client->chats, chat_id);

	if( (list = net_recv_pending_messages(client->network, chat_id, timestamp)) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not get the message list");
		return -1;
	}
	
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

	if ( cha_add_messages(client->chats, chat_id, sender, text, send_date, attach_path, list->__sizenelems) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not add messages");
		return -1;
	}

	cha_set_pending(client->chats, chat_id, 0);
	cha_update_unread(client->chats, chat_id, list->__sizenelems);

	return list->__sizenelems;
}


/*
 * Receive the locally not registered chats
 * Returns the number of created chats or -1 if fails
 */
int psd_recv_chats(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	DEBUG_FAILURE_PRINTF("Not implemented");
	return -1;
}


/*
 * Receive the locally not registered chats
 * Returns the number of created chats or -1 if fails
 */
int psd_recv_friends(psd_ims_client *client) {
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
 * Send a message to the chat "chat_id"
 * Returns 0 or -1 if fails
 */
int psd_send_message(psd_ims_client *client, int chat_id, char *text, char *attach_path) {
	DEBUG_TRACE_PRINT();
	int send_date = 0;

	// TODO send message should return the send_date
	// send_date = ///
	if( net_send_message(client->network, chat_id, text, attach_path) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not send the message");
		return -1;
	}

	if ( cha_add_message(client->chats, chat_id, NULL, text, send_date, attach_path) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not add the message, but it has been sended");
		return -1;
	}

	return 0;
}


/*
 * Send a friend request to "user"
 * Returns 0 or -1 if fails
 */
int psd_send_friend_request(psd_ims_client *client, char *user) {
	DEBUG_TRACE_PRINT();
	int send_date = 0;

	// TODO send friend request should return the send_date
	// send_date = ///
	if ( net_send_friend_request(client->network, user) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not send the friend request");	
		return -1;
	}

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

	if ( net_send_request_accept(client->network, user) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not accept the friend request");	
		return -1;
	}

	if ( fri_del_rcv_request(client->friends, user) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not remove the request locally, but it has been accepted");
		return -1;
	}
	
	return 0;
}


/*
 * Reject a friend request from "user"
 * Returns 0 or -1 if fails
 */
int psd_send_request_decline(psd_ims_client *client, char *user) {
	DEBUG_TRACE_PRINT();

	if ( net_send_request_decline(client->network, user) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not accept the friend request");	
		return -1;
	}

	if ( fri_del_rcv_request(client->friends, user) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not remove the request locally, but it has been declined");
		return -1;
	}
	
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
	cha_print_chat_list(client->chats);
}

/*
 * Prints all chat members line by line
 */
void psd_print_chat_members(psd_ims_client *client, int chat_id) {
	DEBUG_TRACE_PRINT();
	cha_print_chat_members(client->chats, chat_id);
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
		if ( (aux_admin = fri_find_friend(client->friends, admin)) == NULL ) {
			aux_admin = NULL;
			return -1; // chat admin is not a friend
		}
	}

	aux_friend_list = malloc( sizeof(friend_info)*n_members );

	for( i = 0 ; i < n_members ; i++) {
		aux_friend_list[i] = fri_find_friend(client->friends, members[i]);
	}

	if( cha_add_chat(client->chats, id, description, aux_admin, aux_friend_list, n_members) == -1 ) {
		free(aux_friend_list);
		return -1;
	}

	free(aux_friend_list);
	return 0;
}


/*
 * Removes and frees the first chat that matches "chat_ic"
 * Returns 0 or -1 if "chat_id" does not exist in the list
 */
int psd_del_chat(psd_ims_client *client, int chat_id) {
	DEBUG_TRACE_PRINT();
	return cha_del_chat(client->chats, chat_id);
}


/*
 * Adds the member "user_name" to the chat "chat_id"
 * Returns 0 or -1 if fails
 */
int psd_add_friend_to_chat(psd_ims_client *client, int chat_id, const char *user_name) {
	DEBUG_TRACE_PRINT();
	friend_info *friend_info;

	// find friend in client->friends
	if ( (friend_info = fri_find_friend(client->friends, user_name)) == NULL ) {
		return -1; // friend does not exist
	}

	if ( cha_add_member(client->chats, chat_id, friend_info) == -1 ) {
		return -1; // could not add member to chat
	}

	return 0;
}


/*
 * Deletes the first ocurrence of a chat member with the provided "name" from the cha "chat_id"
 * Returns 0 or -1 if fails
 */
int psd_del_friend_from_chat(psd_ims_client *client, int chat_id, const char *user_name) {
	DEBUG_TRACE_PRINT();
	return cha_del_member(client->chats, chat_id, user_name);
}


/*
 * Adds the messages in the chat
 * Returns 0 or -1 if fails
 */
int psd_add_messages(psd_ims_client *client, int chat_id, char *sender[], char *text[], int send_date[], char *attach_path[], int n_messages) {
	DEBUG_TRACE_PRINT();
	return cha_add_messages(client->chats, chat_id, sender, text, send_date, attach_path, n_messages);
}


/*
 * Clears the chat pending messages counter
 */
int psd_clean_pending_messages(psd_ims_client *client, int chat_id) {
	return cha_set_unread(client->chats, chat_id, 0);
}


/*
 * Updates the chat pending messages counter
 */
int psd_update_pending_messages(psd_ims_client *client, int chat_id, int n_messages) {
	return cha_update_unread(client->chats, chat_id, n_messages);
}


/*
 * Switches the current admin with the chat member named "user_name"
 * that means that the previous admin becomes a normal member
 * Returns 0 or -1 if fails
 */
int psd_change_chat_admin(psd_ims_client *client, int chat_id, const char *user_name) {
	DEBUG_TRACE_PRINT();
	return cha_change_admin(client->chats, chat_id, user_name);
}


/*
 * Promotes the member named "user_name" to chat admin
 * The previous admin is NOT introduced as a chat member
 * Returns 0 or -1 if fails
 */
int psd_promote_to_chat_admin(psd_ims_client *client, int chat_id, const char *user_name) {
	DEBUG_TRACE_PRINT();
	return cha_promote_to_admin(client->chats, chat_id, user_name);
}


/* =========================================================================
 *  Friends
 * =========================================================================*/

/*
 * Prints all friends line by line
 */
void psd_print_friends(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	fri_print_friend_list(client->friends);
}


/*
 * Prints all friends requests line by line
 */
void psd_print_friend_requests(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	fri_print_rcv_request_list(client->friends);
}


/*
 * Adds a new friend to client's friend list
 * Returns 0 or -1 if fails
 */
int psd_add_friend(psd_ims_client *client, const char *name, const char *information) {
	DEBUG_TRACE_PRINT();
	return fri_add_friend(client->friends, name, information);
}


/*
 * Adds a sended friend request to "name"
 */
int psd_add_friend_req_snd(psd_ims_client *client, const char *name, int send_date) {
	return fri_add_snd_request(client->friends, name, send_date);
}	


/*
 * Adds a received friend request from "name"
 */
int psd_add_friend_req_rcv(psd_ims_client *client, const char *name, int send_date) {
	return fri_add_rcv_request(client->friends, name, send_date);
}


/*
 * Removes and frees the first friend that matches "name"
 * Returns 0 or -1 if "name" does not exist in the list
 */
int psd_del_friend(psd_ims_client *client, const char *name) {
	DEBUG_TRACE_PRINT();
	return fri_del_friend(client->friends, name);
}



