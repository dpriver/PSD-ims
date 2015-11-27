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
#include "network.h"
#include "pending_chats.h"

typedef chat_node chat_list;
typedef friend_node friend_list;

typedef struct psd_ims_client psd_ims_client;
struct psd_ims_client {
	char *user_name;
	char *user_pass;
	char *user_info;
	int last_connection;
	int last_notif_timestamp;
	pending_chat_list new_chats;
	network *network;
	friends *friends;
	chat_list *chats;
};



/* =========================================================================
 *  Client struct
 * =========================================================================*/

/*
 * Allocates a new psd_ims_client struct
 * Returns a pointer to the structure or NULL if fails
 */
psd_ims_client *psd_new_client();

/*
 * Removes and frees the client struct
 */
void psd_free_client(psd_ims_client *client);

/*
 * Sets client name
 * Returns o or -1 if fails
 */
int psd_set_name(psd_ims_client *client, const char *name);

/*
 * Sets client password
 * Returns 0 or -1 if fails
 */
int psd_set_pass(psd_ims_client *client, const char *pass);


/* =========================================================================
 *  Network operations
 * =========================================================================*/

/*
 * Init the network
 */
int psd_bind_network(psd_ims_client *client, char *serverURL);

/*
 * Gets the user information from the server
 * Returns 0 or -1 if fails
 */
int psd_login(psd_ims_client *client, char *name, char *password);

/*
 * Register the user in the system
 * Returns 0 or -1 if fails
 */
int psd_user_register(psd_ims_client *client, char *name, char *password, char *information);

/*
 * Receive the pending notifications
 * Returns the number of received notifications or -1 if fails
 */
int psd_recv_notifications(psd_ims_client *client);

/*
 * Receive the pending messages
 * Returns the number of received messages or -1 if fails
 */
int psd_recv_pending_messages(psd_ims_client *client, int chat_id);

/*
 * Receive the user chats
 * Returns the number of created chats or -1 if fails
 */
int psd_recv_chats(psd_ims_client *client);

/*
 * Receive the user friends
 * Returns the number of created chats or -1 if fails
 */
int psd_recv_friends(psd_ims_client *client);

/*
 * Receive the locally not registered chats
 * Returns the number of created chats or -1 if fails
 */
int psd_recv_new_chats(psd_ims_client *client);

/*
 * Send a message to the chat "chat_id"
 * Returns 0 or -1 if fails
 */
int psd_send_message(psd_ims_client *client, int chat_id, char *text, char *attach_path);

/*
 * Send a friend request to "user"
 * Returns 0 or -1 if fails
 */
int psd_send_friend_request(psd_ims_client *client, char *user);

/*
 * Accept a friend request from "user"
 * Returns 0 or -1 if fails
 */
int psd_send_request_accept(psd_ims_client *client, char *user);

/*
 * Reject a friend request from "user"
 * Returns 0 or -1 if fails
 */
int psd_send_request_decline(psd_ims_client *client, char *user);


/* =========================================================================
 *  Chats
 * =========================================================================*/

/*
 * Prints all chats line by line
 */
void psd_print_chats(psd_ims_client *client);

/*
 * Prints all chat members line by line
 */
void psd_print_chat_members(psd_ims_client *client, int chat_id);

/*
 * Adds a new chat to client's chat list
 * Returns 0 or -1 if fails
 */
int psd_add_chat(psd_ims_client *client, int id, const char *description, const char *admin,
			char *members[], int n_members);

/*
 * Removes and frees the first chat that matches "chat_ic"
 * Returns 0 or -1 if "chat_id" does not exist in the list
 */
int psd_del_chat(psd_ims_client *client, int chat_id);

/*
 * Adds the member "user_name" to the chat "chat_id"
 * Returns 0 or -1 if fails
 */
int psd_add_friend_to_chat(psd_ims_client *client, int chat_id, const char *user_name);

/*
 * Deletes the first ocurrence of a chat member with the provided "name" from the cha "chat_id"
 * Returns 0 or -1 if fails
 */
int psd_del_friend_from_chat(psd_ims_client *client, int chat_id, const char *user_name);

/*
 * Adds the messages in the chat
 * Returns 0 or -1 if fails
 */
int psd_add_messages(psd_ims_client *client, int chat_id, char *sender[], char *text[], int send_date[], char *attach_path[], int n_messages);

/*
 * Clears the chat pending messages counter
 */
int psd_clean_pending_messages(psd_ims_client *client, int chat_id);

/*
 * Updates the chat pending messages counter
 */
int psd_update_pending_messages(psd_ims_client *client, int chat_id, int n_messages);

/*
 * Switches the current admin with the chat member named "user_name"
 * that means that the previous admin becomes a normal member
 * Returns 0 or -1 if fails
 */
int psd_change_chat_admin(psd_ims_client *client, int chat_id, const char *user_name);

/*
 * Promotes the member named "user_name" to chat admin
 * The previous admin is NOT introduced as a chat member
 * Returns 0 or -1 if fails
 */
int psd_promote_to_chat_admin(psd_ims_client *client, int chat_id, const char *user_name);


/* =========================================================================
 *  Friends
 * =========================================================================*/

/*
 * Prints all friends line by line
 */
void psd_print_friends(psd_ims_client *client);

/*
 * Prints all friends requests line by line
 */
void psd_print_friend_requests(psd_ims_client *client);

/*
 * Adds a new friend to client's friend list
 * Returns 0 or -1 if fails
 */
int psd_add_friend(psd_ims_client *client, const char *name, const char *information);

/*
 * Adds a sended friend request to "name"
 */
int psd_add_friend_req_snd(psd_ims_client *client, const char *name,  int send_date);

/*
 * Adds a received friend request from "name"
 */
int psd_add_friend_req_rcv(psd_ims_client *client, const char *name, int send_date);

/*
 * Removes and frees the first friend that matches "name"
 * Returns 0 or -1 if "name" does not exist in the list
 */
int psd_del_friend(psd_ims_client *client, const char *name);


#endif /* __PSD_IMS_CLIENT */
