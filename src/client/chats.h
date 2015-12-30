/*******************************************************************************
 *	chats.h
 *
 *  Client chat list
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

#ifndef __CHATS
#define __CHATS

#include "bool.h"
#include "list.h"
#include "friends.h"
#include "messages.h"
#include "chat_members.h"


typedef struct chat_info chat_info;
struct chat_info {
	int id;
	char *description;
	int read_timestamp;
	int all_read_timestamp;
	int unread_messages;
	int pending_messages;
	char *admin;
	chat_members *members;
	messages *messages;
};

typedef struct chat_list_info chat_list_info;
struct chat_list_info {
	int timestamp;
};

typedef list chats;
typedef list_iterator chat_iterator;

/* =========================================================================
 *  Structs access macros
 * =========================================================================*/
 
#define cha_get_id(chat_info) \
		(chat_info->id);

#define cha_description(chat_info) \
		(chat_info->description)

#define cha_pending(chat_info) \
		(chat_info->pending_messages)
		
#define cha_unread(chat_info) \
		(chat_info->unread_messages)

#define cha_read_timestamp(chat_info) \
		(chat_info->read_timestamp)

#define cha_all_read_timestamp(chat_info) \
		(chat_info->all_read_timestamp)

#define cha_admin_myself(chat_info) \
		(chat_info->admin == NULL)

#define cha_admin_name(chat_info) \
		(chat_info->admin)

#define cha_messages(chat_info) \
		(chat_info->messages)
		
#define cha_members(chat_info) \
		(chat_info->members)

#define cha_clear_unread(chat_info) \
		(chat_info->unread_messages = 0); \
		mes_get_timestamp(chat_info->messages, chat_info->read_timestamp)

#define cha_set_all_read_timestamp(chat_info, timestamp) \
		(chat_info->all_read_timestamp = timestamp)

#define cha_set_pending(chat_info, num_pending) \
		(chat_info->pending_messages = num_pending)

#define cha_update_pending(chat_info, num_pending) \
		(chat_info->pending_messages += num_pending)
		
#define cha_set_unread(chat_info, num_unread) \
		(chat_info->unread_messages = num_unread)
		
#define cha_update_unread(chat_info, num_unread) \
		(chat_info->unread_messages += num_unread)

#define cha_get_members_timestamp(chat_info, members_timestamp) \
	member_get_timestamp(chat_info->members, members_timestamp)

#define cha_get_messages_timestamp(chat_info, mes_timestamp) \
	mes_get_timestamp(chat_info->messages, mes_timestamp)

#define cha_set_members_timestamp(chat_info, members_timestamp) \
	member_set_timestamp(chat_info->members, members_timestamp)

#define cha_set_messages_timestamp(chat_info, messages_timestamp) \
	mes_set_timestamp(chat_info->messages, messages_timestamp)


#define cha_num_chats(chats) \
		list_num_elems(chats)	

#define cha_get_timestamp(chats, chat_timestamp) \
	do{ \
		chat_list_info *aux; \
		aux = list_info(chats); \
		chat_timestamp = aux->timestamp; \
	}while(0)

#define cha_set_timestamp(chats, chat_timestamp) \
	do{ \
		chat_list_info *aux; \
		aux = list_info(chats); \
		aux->timestamp = chat_timestamp; \
	}while(0)



/* =========================================================================
 *  Chat iterator
 * =========================================================================*/
#define cha_get_chats_iterator(list) \
		(chat_iterator*)list_iterator(list)

#define cha_iterator_next(list, iterator) \
		(chat_iterator*)list_iterator_next(list, iterator)

#define cha_get_info(iterator) \
		(chat_info*)list_iterator_info(iterator)

/* =========================================================================
 *  Chat struct API
 * =========================================================================*/

/*
 * Allocates a new chat list
 * Returns a pointer to the list or NULL if fails
 */
chats *cha_new(int max);

/*
 * Frees the chat list
 */
void cha_free(chats *chats);

/*
 * Creates a new chat in the list with the provided info
 * Returns 0 or -1 if fails
 */
int cha_add_chat(chats *chats, int chat_id, const char *description, char *admin, friend_info *members[], char *member_names[], int n_members, int max_members,  int max_messages, int read_timestamp, int all_read_timestamp);

/*
 * Adds the message in the chat
 * Returns 0 or -1 if fails
 */
int cha_add_message(chat_info *chat, const char *sender, const char *text, int send_date, const char *attach_path);

/*
 * Adds the messages in the chat
 * Returns 0 or -1 if fails
 */
int cha_add_messages(chat_info *chat, char *sender[], char *text[], int send_date[], char *attach_path[], int n_messages);

/*
 * Creates a new chat member in the list with the provided info
 * Returns 0 or -1 if fails
 */
int cha_add_member(chat_info *chat, friend_info *member, const char *name);

/*
 * Deletes chat member from the chat
 * Returns 0 or -1 if fails
 */
int cha_del_member(chat_info *chat, const char *member_name);

/*
 * Switches the current admin with the chat member named "name"
 * that means that the previous admin becomes a normal member
 * Returns 0 or -1 if fails
 */
int cha_change_admin(chat_info *chat, const char *member_name);

/*
 * Deletes chat from the list
 * Returns 0 or -1 if fails
 */
int cha_del_chat(chats *chats, int chat_id);

/*
 * Search for the chat coincident with chat_id
 * Returns a chat_info pointer or NULL if fails
 */
chat_info *cha_find_chat(chats *chats, int chat_id);


#endif /* __CHATS */
