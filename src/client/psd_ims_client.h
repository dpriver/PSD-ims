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

#ifndef __PSD_IMS_CLIENT
#define __PSD_IMS_CLIENT

#include "friends.h"
#include "chats.h"

typedef struct psd_ims_client psd_ims_client;
struct psd_ims_client {
	char *user_name;
	char *pass;
	int last_connection;
	friend_list *friends;
	chat_list *chats;
};


/* =========================================================================
 *  Client struct
 * =========================================================================*/

/*
 * Allocates a new psd_ims_client struct
 *
 * Returns a pointer to the structure or NULL if fails
 */
psd_ims_client *new_client();

/*
 * Sets to client name
 *
 * Returns o or -1 if fails
 */
int set_name(psd_ims_client *client, const char *name);

/*
 * Sets to client password
 *
 * Returns o or -1 if fails
 */
int set_pass(psd_ims_client *client, const char *pass);

/*
 * Removes and frees the client struct
 */
void free_client(psd_ims_client *client);



/* =========================================================================
 *  Chats
 * =========================================================================*/

/*
 * Prints all chats line by line
 */
void print_chats(psd_ims_client *client);


/*
 * Creates a new chat in the list with the provided info
 * "*info" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int add_chat(psd_ims_client *client, chat_info *info);


/*
 * Removes and frees the first node that matches the provided "name"
 *
 * Returns 0 or -1 if "name" does not exist in the list
 */
int del_chat(psd_ims_client *client, const char *name);


/*
 * Returns true of false whether "name" is in the chat list or not
 */
boolean is_chat_in_list(friend_node *list, char *name);



/* =========================================================================
 *  Friends
 * =========================================================================*/

/*
 * Prints all friends line by line
 */
void print_friends(psd_ims_client *client);


/*
 * Creates a new friend in the list with the provided info
 * "*info" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int add_friend(psd_ims_client *client, friend_info *info);


/*
 * Removes and frees the first node that matches the provided "name"
 *
 * Returns 0 or -1 if "name" does not exist in the list
 */
int del_friend(psd_ims_client *client, const char *name);


/*
 * Returns true of false whether "name" is in the friend list or not
 */
boolean is_friend_in_list(friend_node *list, char *name);




#endif /* __PSD_IMS_CLIENT */
