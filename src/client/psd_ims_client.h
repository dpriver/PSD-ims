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

#define MAX_FRIENDS	(50)
#define MAX_FRIEND_REQUESTS (50)
#define MAX_CHATS (50)
#define MAX_MESSAGES (50)
#define MAX_MEMBERS (50)

#define MAX_FILE_PATH_CHARS (100)
	// max file size = 10MB
#define MAX_FILE_CHARS (10485760)
#define ATTACH_FILES_DIR_RCV "attached_files_rcv"
#define ATTACH_FILES_DIR_SND "attached_files_snd"

#include "friends.h"
#include "chats.h"
#include "friend_requests.h"
#include "messages.h"
#include "chat_members.h"
#include "network.h"
#include <pthread.h>


typedef struct psd_ims_client psd_ims_client;
struct psd_ims_client {
	// user info
	char *user_name;
	char *user_pass;
	char *user_info;
	// timestamps
	int last_connection;
	int last_notif_timestamp;
	// lists
	network *network;
	friends *friends;
	friend_requests *requests;
	chats *chats;
	// lists mutexes
	pthread_mutex_t new_chats_mutex;
	pthread_mutex_t chats_mutex;
	pthread_mutex_t network_mutex;
	pthread_mutex_t friends_mutex;
	pthread_mutex_t requests_mutex;
};

struct chat_mes_iterator {
	mes_iterator *iter;
	chat_info *chat;
};

struct chat_member_iterator {
	member_iterator *iter;
	chat_info *chat;
};

typedef fri_iterator	friends_iterator;
typedef req_iterator 	req_iterator;
typedef chat_iterator	chats_iterator;
typedef struct chat_mes_iterator 	chat_mes_iterator;
typedef struct chat_member_iterator chat_member_iterator;


/* =========================================================================
 *  Struct access
 * =========================================================================*/

#define psd_client_user_name(client) \
		(client->user_name)

#define psd_client_user_information(client) \
		(client->user_info)

#define psd_notif_timestamp(client, timestamp) \
		(timestamp = client->last_notif_timestamp)
		
#define psd_friends_timestamp(client, timestamp) \
		fri_get_timestamp(client->friends, timestamp)
		
#define psd_requests_timestamp(client, timestamp) \
		req_list_timestamp(client->requests, timestamp)
		
#define psd_chats_timestamp(client, timestamp) \
		cha_get_timestamp(client->chats, timestamp)


#define create_file_path_snd(buff, chat_id, timestamp) \
		sprintf(buff, "%s/_%d%d", ATTACH_FILES_DIR_SND, chat_id, timestamp)
		
#define create_file_path_rcv(buff, chat_id, timestamp) \
		sprintf(buff, "%s/_%d%d", ATTACH_FILES_DIR_RCV, chat_id, timestamp)
		
/* =========================================================================
 *  Iterators
 * =========================================================================*/

/*-----------------------------------------------------------
 * 			 FRIENDS ITERATOR
 * ----------------------------------------------------------/*

/*
 * begins a thread-safe friend iteration block and assigns a new iterator to iterator
 */
#define psd_begin_fri_iteration(client, iterator) \
	do{ \
		pthread_mutex_lock(&client->friends_mutex); \
		iterator = fri_get_friends_iterator(client->friends); \
	}while(0)

#define psd_fri_iterator_valid(iterator) \
	(iterator != NULL)

/*
 * ends a friend iteration block
 */
#define psd_end_fri_iteration(client, iterator) \
	do{ \
		pthread_mutex_unlock(&client->friends_mutex); \
		iterator = NULL; \
	}while(0)

#define psd_fri_iterator_next(client, iterator) \
		fri_iterator_next(client->friends, iterator)

#define psd_fri_iterator_name(iterator, name) \
	do{ \
		friend_info *aux; \
		aux = fri_get_info(iterator); \
		name = fri_get_name(aux); \
	}while(0)

#define psd_fri_iterator_information(iterator, information) \
	do{ \
		friend_info *aux; \
		aux = fri_get_info(iterator); \
		information = fri_get_information(aux); \
	}while(0)


/*-----------------------------------------------------------
 * 			 FRIEND REQUESTS ITERATOR
 * ----------------------------------------------------------/*

/*
 * begins a thread-safe friend requests iteration block and assigns a new iterator to iterator
 */
#define psd_begin_req_iteration(client, iterator) \
	do{ \
		pthread_mutex_lock(&client->requests_mutex); \
		iterator = req_get_requests_iterator(client->requests); \
	}while(0)

#define psd_req_iterator_valid(iterator) \
	(iterator != NULL)

/*
 * ends a friend iteration block
 */
#define psd_end_req_iteration(client, iterator) \
	do{ \
		pthread_mutex_unlock(&client->requests_mutex); \
		iterator = NULL; \
	}while(0)

#define psd_req_iterator_next(client, iterator) \
		req_iterator_next(client->requests, iterator)

#define psd_req_iterator_name(iterator, name) \
	do{ \
		request_info *aux; \
		aux = req_get_info(iterator); \
		name = req_name(aux); \
	}while(0)

#define psd_req_iterator_time(iterator, timestamp) \
	do{ \
		request_info *aux; \
		aux = req_get_info(iterator); \
		timestamp = req_time(aux); \
	}while(0)


/*-----------------------------------------------------------
 * 			 CHATS ITERATOR
 * ----------------------------------------------------------/*

/*
 * begins a thread-safe friend requests iteration block and assigns a new iterator to iterator
 */
#define psd_begin_chat_iteration(client, iterator) \
	do{ \
		pthread_mutex_lock(&client->chats_mutex); \
		iterator = cha_get_chats_iterator(client->chats); \
	}while(0)

#define psd_chat_iterator_valid(iterator) \
	(iterator != NULL)

/*
 * ends a friend iteration block
 */
#define psd_end_chat_iteration(client, iterator) \
	do{ \
		pthread_mutex_unlock(&client->chats_mutex); \
		iterator = NULL; \
	}while(0)

#define psd_chat_iterator_next(client, iterator) \
		cha_iterator_next(client->chats, iterator)

#define psd_chat_iterator_id(iterator, id) \
	do{ \
		chat_info *aux; \
		aux = cha_get_info(iterator); \
		id = cha_get_id(aux); \
	}while(0)

#define psd_chat_iterator_description(iterator, description) \
	do{ \
		chat_info *aux; \
		aux = cha_get_info(iterator); \
		description = cha_description(aux); \
	}while(0)

#define psd_chat_iterator_unread(iterator, unread) \
	do{ \
		chat_info *aux; \
		aux = cha_get_info(iterator); \
		unread = cha_unread(aux); \
	}while(0)
	
#define psd_chat_iterator_pending(iterator, pending) \
	do{ \
		chat_info *aux; \
		aux = cha_get_info(iterator); \
		pending = cha_pending(aux); \
	}while(0)

#define psd_chat_iterator_admin_myself(iterator, admin_myself) \
	do{ \
		chat_info *aux; \
		aux = cha_get_info(iterator); \
		admin_myself = cha_admin_myself(aux); \
	}while(0)

#define psd_chat_iterator_admin_name(iterator, name) \
	do{ \
		chat_info *aux; \
		aux = cha_get_info(iterator); \
		if (cha_admin_myself(aux)) { \
			name = NULL; \
			break; \
		} \
		name = cha_admin_name(aux); \
	}while(0)


/*-----------------------------------------------------------
 * 			 CHAT MESSAGES ITERATOR
 * ----------------------------------------------------------/*

/*
 * begins a thread-safe friend requests iteration block and assigns a new iterator to iterator
 */
#define psd_begin_mes_iteration(client, chat_id, iterator) \
	do{ \
		messages *aux; \
		iterator = malloc(sizeof(chat_mes_iterator)); \
		pthread_mutex_lock(&(client->chats_mutex)); \
		iterator->chat = cha_find_chat(client->chats, chat_id); \
		if (iterator->chat == NULL) { \
			free(iterator); \
			iterator = NULL; \
			break; \
		} \
		aux = cha_messages(iterator->chat); \
		iterator->iter = mes_get_messages_iterator( aux ); \
		if (iterator->iter == NULL) { \
			free(iterator); \
			iterator = NULL; \
			break; \
		} \
	}while(0)

#define psd_mes_iterator_valid(iterator) \
	((iterator != NULL ) && (iterator->iter != NULL))

/*
 * ends a friend iteration block
 */
#define psd_end_mes_iteration(client, iterator) \
	do{ \
		pthread_mutex_unlock(&(client->chats_mutex)); \
		free(iterator); \
		iterator = NULL; \
	}while(0)

#define psd_mes_iterator_next(iterator) \
	do{ \
		messages *aux; \
		aux = cha_messages(iterator->chat); \
		mes_iterator_next(aux, iterator->iter); \
	}while(0)

#define psd_mes_iterator_sender(iterator, name) \
	do{ \
		message_info *aux; \
		aux = mes_get_info(iterator->iter); \
		name = mes_sender(aux); \
	}while(0)

#define psd_mes_iterator_text(iterator, text) \
	do{ \
		message_info *aux; \
		aux = mes_get_info(iterator->iter); \
		text = mes_text(aux); \
	}while(0)

#define psd_mes_iterator_attach_path(iterator, path) \
	do{ \
		message_info *aux; \
		aux = mes_get_info(iterator->iter); \
		path = mes_attach_path(aux); \
	}while(0)

#define psd_mes_iterator_time(iterator, time) \
	do{ \
		message_info *aux; \
		aux = mes_get_info(iterator->iter); \
		time = mes_message_timestamp(aux); \
	}while(0)
	
#define psd_mes_iterator_double_check_time(iterator, time) \
		(time = cha_all_read_timestamp(iterator->chat))
	
#define psd_mes_iterator_clear_unread(iterator) \
	do{ \
		cha_clear_unread(iterator->chat); \
	}while(0)
	
	
/*-----------------------------------------------------------
 * 			 CHAT MEMBERS ITERATOR
 * ----------------------------------------------------------/*

/*
 * begins a thread-safe friend requests iteration block and assigns a new iterator to iterator
 */
#define psd_begin_member_iteration(client, chat_id, iterator) \
	do{ \
		chat_members *aux; \
		iterator = malloc(sizeof(chat_member_iterator)); \
		if (iterator == NULL) { \
			free(iterator); \
			iterator = NULL; \
			break; \
		} \
		iterator->chat = cha_find_chat(client->chats, chat_id); \
		pthread_mutex_lock(&client->chats_mutex); \
		aux = cha_members(iterator->chat); \
		iterator->iter = member_get_members_iterator( aux ); \
		if (iterator->iter == NULL) { \
			free(iterator); \
			iterator = NULL; \
			break; \
		} \
	}while(0)

#define psd_member_iterator_valid(iterator) \
	((iterator != NULL ) && (iterator->iter != NULL))

/*
 * ends a friend iteration block
 */
#define psd_end_member_iteration(client, iterator) \
	do{ \
		pthread_mutex_unlock(&client->chats_mutex); \
		free(iterator); \
		iterator = NULL; \
	}while(0)

#define psd_member_iterator_next(iterator) \
	do{ \
		chat_members *aux; \
		aux = cha_members(iterator->chat); \
		member_iterator_next(aux, iterator->iter); \
	}while(0)


#define psd_member_iterator_name(iterator, name) \
	do{ \
		member_info *aux; \
		aux = member_get_info(iterator->iter); \
		name = member_name(aux); \
	}while(0)

#define psd_member_iterator_isfriend(iterator, isfriend) \
	do{ \
		member_info *aux; \
		aux = member_get_info(iterator->iter); \
		isfriend = member_is_friend(aux); \
	}while(0)

#define psd_member_iterator_information(iterator, information) \
	do{ \
		member_info *aux; \
		aux = member_get_info(iterator->iter); \
		if (!member_is_friend(aux)) { \
			information = NULL; \
			break; \
		} \
		information = fri_get_information(member_friend_info(aux)); \
	}while(0)
	
#define psd_member_iterator_admin_myself(iterator, admin_myself) \
		(admin_myself = cha_admin_myself(iterator->chat))

#define psd_member_iterator_admin_name(iterator, name) \
		(name = cha_admin_name(iterator->chat))
	
#define psd_member_iterator_admin_info(iterator, information) \
		(information = cha_admin_information(iterator->chat))


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
 * Loggs out
 * Returns 0 or -1 if fails
 */
void psd_logout(psd_ims_client *client);

/*
 * Register the user in the system
 * Returns 0 or -1 if fails
 */
int psd_user_register(psd_ims_client *client, char *name, char *password, char *information);

/*
 * Unregister the user from the system
 * Returns 0 or -1 if fails
 */
int psd_user_unregister(psd_ims_client *client, char *name, char *password);

/*
 * Receive the pending notifications
 * Returns the number of received notifications or -1 if fails
 */
int psd_recv_notifications(psd_ims_client *client);

/*
 * Receive all the chat's messages
 * Returns the number of received messages or -1 if fails
 */
int psd_recv_messages(psd_ims_client *client, int chat_id);

/*
 * Receive the pending messages
 * Returns the number of received messages or -1 if fails
 */
int psd_recv_pending_messages(psd_ims_client *client, int chat_id);

/*
 * Receive the chat's messages
 * Returns the number of received messages or -1 if fails
 */
int psd_recv_all_messages(psd_ims_client *client);

/*
 * Receive the chat's messages only if there are "pending messages"
 * Returns the number of received messages or -1 if fails
 */
int psd_recv_all_pending_messages(psd_ims_client *client);

/*
 * Receive the message's attachment
 * Returns 0 or -1 if fails
 */
int psd_recv_message_attachment(psd_ims_client *client, int chat_id, int msg_timestamp);

/*
 * Receive the user chats
 * Returns the number of created chats or -1 if fails
 */
int psd_recv_chats(psd_ims_client *client);

/*
 * Receive the user friends
 * Returns the number of added friends or -1 if fails
 */
int psd_recv_friends(psd_ims_client *client);

/*
 * Creates a new chat
 * Returns 0 or -1 if fails
 */
int psd_create_chat(psd_ims_client *client, char *description, char *member);

/*
 *
 *
 */
int psd_add_member_to_chat(psd_ims_client *client, char *member, int chat_id);

/*
 *
 *
 */
int psd_del_member_from_chat(psd_ims_client *client, char *member, int chat_id);

/*
 *
 *
 */
int psd_quit_from_chat(psd_ims_client *client, int chat_id);

/*
 *
 *
 */
int psd_remove_chat(psd_ims_client *client, int chat_id);

/*
 * Send a message to the chat "chat_id"
 * Returns 0 or -1 if fails
 */
int psd_send_message(psd_ims_client *client, int chat_id, char *text, char *file_path, char *file_info);

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

/*
 * Check if the chat exists in list
 */
boolean psd_chat_exists(psd_ims_client *client, int chat_id);


#endif /* __PSD_IMS_CLIENT */
