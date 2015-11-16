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

typedef chat_node chat_list;
typedef friend_node friend_list;

typedef struct psd_ims_client psd_ims_client;
struct psd_ims_client {
	char *user_name;
	char *user_pass;
	int last_connection;
	friends *friends;
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
psd_ims_client *psd_new_client();

/*
 * Sets to client name
 *
 * Returns o or -1 if fails
 */
int psd_set_name(psd_ims_client *client, const char *name);

/*
 * Sets to client password
 *
 * Returns o or -1 if fails
 */
int psd_set_pass(psd_ims_client *client, const char *pass);

/*
 * Removes and frees the client struct
 */
void psd_free_client(psd_ims_client *client);



/* =========================================================================
 *  Chats
 * =========================================================================*/

/*
 * Prints all chats line by line
 */
void psd_print_chats(psd_ims_client *client);


/*
 * Creates a new chat in the list with the provided info
 * "*info" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int psd_add_chat(psd_ims_client *client, int id, const char *description, const char *admin,
			char *members[], int n_members);


int psd_add_friend_to_chat(psd_ims_client *client, int chat_id, const char *user_name);

int psd_del_friend_from_chat(psd_ims_client *client, int chat_id, const char *user_name);

int psd_change_chat_admin(psd_ims_client *client, int chat_id, const char *user_name);


int psd_promote_to_chat_admin(psd_ims_client *client, int chat_id, const char *user_name);


/*
 * Removes and frees the first node that matches the provided "name"
 *
 * Returns 0 or -1 if "name" does not exist in the list
 */
int psd_del_chat(psd_ims_client *client, int chat_id);


/* =========================================================================
 *  Friends
 * =========================================================================*/

/*
 * Prints all friends line by line
 */
void psd_print_friends(psd_ims_client *client);


/*
 * Creates a new friend in the list with the provided info
 * "*info" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int psd_add_friend(psd_ims_client *client, const char *name, const char *information);


/*
 * Removes and frees the first node that matches the provided "name"
 *
 * Returns 0 or -1 if "name" does not exist in the list
 */
int psd_del_friend(psd_ims_client *client, const char *name);


#endif /* __PSD_IMS_CLIENT */
