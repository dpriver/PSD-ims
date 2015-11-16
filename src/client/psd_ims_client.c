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

#include "psd_ims_client.h"
#include "friends.h"
#include "chats.h"
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#include "leak_detector_c.h"
#endif


/* =========================================================================
 *  Client struct
 * =========================================================================*/

/*
 * Allocates a new psd_ims_client struct
 *
 * Returns a pointer to the structure or NULL if fails
 */
psd_ims_client *psd_new_client() {
	psd_ims_client *client;

	client = malloc( sizeof(psd_ims_client) );
	client->user_name = NULL;
	client->user_pass = NULL;
	client->last_connection;

	client->friends = friends_new_list();
	client->chats = chats_new_list();

	return client;
}

/*
 * Sets to client name
 *
 * Returns o or -1 if fails
 */
int psd_set_name(psd_ims_client *client, const char *name) {
	char *user_name;	

	if ( (user_name = malloc(strlen(name) + sizeof(char))) == NULL ) {
		return -1;
	}

	strcpy(user_name, name);
	client->user_name = user_name;

	return 0;
}

/*
 * Sets to client password
 *
 * Returns o or -1 if fails
 */
int psd_set_pass(psd_ims_client *client, const char *pass) {
	char *user_pass;	

	if ( (user_pass = malloc(strlen(pass) + sizeof(char))) == NULL ) {
		return -1;
	}

	strcpy(user_pass, pass);
	client->user_pass = user_pass;

	return 0;
}

/*
 * Removes and frees the client struct
 */
void psd_free_client(psd_ims_client *client) {

	friends_free_list(client->friends);
	chats_free_list(client->chats);

	free(client->user_name);
	free(client->user_pass);

	free(client);
}



/* =========================================================================
 *  Chats
 * =========================================================================*/

/*
 * Prints all chats line by line
 */
void psd_print_chats(psd_ims_client *client) {
	chats_print_list(client->chats);
}


/*
 * Creates a new chat in the list with the provided info
 * "*info" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int psd_add_chat(psd_ims_client *client, const char *description, const char *admin,
			const char **members, int n_members) {
	
	int i;
	chat_member *chat_admin;
	chat_member_list *chat_members;
	chat_info *chat_info;
	chat_member *aux_chat_member;
	friend_info *aux_friend_info;

	// convert string admin into chat_member
	if ( strcmp(admin, client->user_name) == 0 ) {
		aux_friend_info = friends_new_info(client->user_name, "myself"); //TODO change this
	}
	else {
		if ( (aux_friend_info = friends_find(client->friends, admin)) == NULL ) {
			return -1;  // Chat admin is not a friend
		}
	}
	chat_admin = chats_new_member(aux_friend_info);


	// convert string array "members" into chat_member_list
	chat_members = chats_new_member_list();
	for ( i = 0 ; i < n_members ; i++ ) {
		aux_friend_info = friends_find(client->friends, members[i]);
		aux_chat_member = chats_new_member(aux_friend_info);
		chats_add_member(chat_members, aux_chat_member);
	}


	// create and inicialize chat_info struct
	if ( (chat_info = chats_new_info(description, chat_admin, chat_members)) == NULL ) {
		return -1;
	} 

	// add chat_info to chats
	if ( chats_add(client->chats, chat_info) == -1 ) {
		return -1;
	}

	return 0;
}


int psd_add_friend_to_chat(psd_ims_client *client, int chat_id, const char *user_name) {
	
	chat_info *chat_info;
	friend_info *friend_info;
	chat_member *chat_member;
	// find chat in client->chats
	if ( (chat_info = chats_find(client->chats, chat_id)) == NULL ) {
		return -1; // chat does not exist
	}

	// find friend in client->friends
	if ( (friend_info = friends_find(client->friends, user_name)) == NULL ) {
		return -1; // friend does not exist
	}

	// add friend to chat->members
	chat_member = chats_new_member(friend_info);

	if ( chats_add_member(chat_info->members, chat_member) == -1 ) {
		return -1; // could not add member to chat
	}

	return 0;
}


int psd_del_friend_from_chat(psd_ims_client *client, int chat_id, const char *user_name) {

	chat_info *chat_info;
	friend_info *friend_info;
	chat_member *chat_member;

	// find chat in client->chats
	if ( (chat_info = chats_find(client->chats, chat_id)) == NULL ) {
		return -1; // chat does not exist
	}

	if ( chats_del_member(chat_info->members, user_name) == -1 ) {
		return -1; // can not delete chat member
	}

	return 0;
}


int psd_change_chat_admin(psd_ims_client *client, int chat_id, const char *user_name) {

	chat_info *chat_info;

	// find chat in client->chats
	if ( (chat_info = chats_find(client->chats, chat_id)) == NULL ) {
		return -1; // chat does not exist
	}

	if ( chats_change_admin(chat_info, user_name) == -1 ) {
		return -1;
	}
}


int psd_promote_to_chat_admin(psd_ims_client *client, int chat_id, const char *user_name) {

	chat_info *chat_info;

	// find chat in client->chats
	if ( (chat_info = chats_find(client->chats, chat_id)) == NULL ) {
		return -1; // chat does not exist
	}

	// check that the client user is the chat admin
	if ( strcmp(chat_info->admin->info->name, client->user_name) != 0 ) {
		return -1; // client user is not the chat admin
	}

	if ( chats_promote_to_admin(chat_info, user_name) == -1 ) {
		return -1;
	}

}


/*
 * Removes and frees the chat
 *
 * Returns 0 or -1 if "name" does not exist in the list
 */
int psd_del_chat(psd_ims_client *client, int chat_id) {
	return chats_del(client->chats, chat_id);
}


/* =========================================================================
 *  Friends
 * =========================================================================*/

/*
 * Prints all friends line by line
 */
void psd_print_friends(psd_ims_client *client) {
	friends_print_list(client->friends);
}


/*
 * Creates a new friend in the list with the provided info
 * "*info" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int psd_add_friend(psd_ims_client *client, const char *name, const char *information) {
	friend_info *friend_info;

	if ( (friend_info = friends_new_info(name, information)) == NULL ) {
		return -1;
	}

	if ( friends_add(client->friends, friend_info) == -1 ) {
		return -1;
	}

	return 0;
}


/*
 * Removes and frees the first node that matches the provided "name"
 *
 * Returns 0 or -1 if "name" does not exist in the list
 */
int psd_del_friend(psd_ims_client *client, const char *name) {
	return friends_del(client->friends, name);
}



