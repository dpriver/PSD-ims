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
#include <stdio.h>
#include <string.h>

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
	DEBUG_FAILURE_PRINTF("Not implemented");
	// Llamar a gsoap para obtener la info de "name" "password"
		// si el server la devuelve, agregar en client y devolver 0
		// si no, devolver -1
	return -1;
}


/*
 * Register the user in the system
 * Returns 0 or -1 if fails
 */
int psd_user_register(psd_ims_client *client, char *name, char *password, char *information) {
	DEBUG_TRACE_PRINT();
	DEBUG_FAILURE_PRINTF("Not implemented");
	// llamar a gsoap para registrar el usuario
	// si tiene éxito devolver 0
	// si no,devolver -1
	return -1;
}


/*
 * Receive the pending notifications
 * Returns the number of received notifications or -1 if fails
 */
int psd_recv_notifications(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	DEBUG_FAILURE_PRINTF("Not implemented");
	return -1;
}


/*
 * Receive the pending messages
 * Returns the number of received messages or -1 if fails
 */
int psd_recv_pending_messages(psd_ims_client *client, int chat_id) {
	DEBUG_TRACE_PRINT();
	DEBUG_FAILURE_PRINTF("Not implemented");
	return 0;
}


/*
 * Receive the locally not registered chats
 * Returns the number of created chats or -1 if fails
 */
int psd_recv_new_chats(psd_ims_client *client) {
	DEBUG_TRACE_PRINT();
	DEBUG_FAILURE_PRINTF("Not implemented");
	return 0;
}


/*
 * Send a message to the chat "chat_id"
 * Returns 0 or -1 if fails
 */
int psd_send_message(psd_ims_client *client, int chat_id, char *text, char *attach_path) {
	DEBUG_TRACE_PRINT();
	DEBUG_FAILURE_PRINTF("Not implemented");
	return -1;
}


/*
 * Send a friend request to "user"
 * Returns 0 or -1 if fails
 */
int psd_send_friend_request(psd_ims_client *client, char *user) {
	DEBUG_TRACE_PRINT();
	DEBUG_FAILURE_PRINTF("Not implemented");
	return -1;
}


/*
 * Accept a friend request from "user"
 * Returns 0 or -1 if fails
 */
int psd_send_request_accept(psd_ims_client *client, char *user) {
	DEBUG_TRACE_PRINT();
	DEBUG_FAILURE_PRINTF("Not implemented");
	return -1;
}


/*
 * Reject a friend request from "user"
 * Returns 0 or -1 if fails
 */
int psd_send_request_decline(psd_ims_client *client, char *user) {
	DEBUG_TRACE_PRINT();
	DEBUG_FAILURE_PRINTF("Not implemented");
	return -1;
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
int psd_add_messages(psd_ims_client *client, int chat_id, const char *sender[], const char *text[], int send_date[], const char *attach_path[], int n_messages) {
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



