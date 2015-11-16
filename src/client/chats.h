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


#include "friends.h"

typedef struct chat_member chat_member;
struct chat_member {
	friend_info *info;
};

typedef struct chat_member_list chat_member_list;
struct chat_member_list {
	chat_member *members;
	int n_members;
};

typedef struct chat_info chat_info;
struct chat_info {
	int id;
	char *description;
	chat_member *admin;
	chat_member_list *members;
	//message_list *messages;
};

typedef struct chat_node chat_node;
struct chat_node {
	chat_info *info;
	chat_node *next;
	chat_node *prev;
};

typedef chat_node chat_list;


/* =========================================================================
 *  Chat list
 * =========================================================================*/

/*
 * Prints all chats line by line
 */
void chats_print_list(chat_node *list);

/*
 * Allocates a new chat list
 *
 * Returns a pointer to the list phantom node or NULL if fails
 */
chat_list *chats_new_list();

/*
 * Creates a new chat_node in the list with the provided info
 * "*info" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int chats_add(chat_list *list, chat_info *info);

/*
 * Removes and frees the chat with id "chat_id"
 *
 * Returns 0 or -1 if "chat_id" does not exist in the list
 */
int chats_del(chat_list *list, int chat_id);

/*
 * Finds the chat whos id is chat_id
 *
 * Returns a pointer to the chat_info of NULL if fails
 */
chat_info *chats_find(chat_list *list, int chat_id);
/*
 * Frees the chat list
 */
void chats_free_list(chat_list *list);


/* =========================================================================
 *  Chats
 * =========================================================================*/

/*
 * Allocates a new chat_info struct with the provided data
 *
 * Returns a pointer to the structure or NULL if fails
 */
chat_info *chats_new_info(const char *description, chat_member *admin, chat_member_list *members);

/*
 * Switches the current admin with the chat member named "name"
 * that means that the previous admin becomes a normal member
 *
 * Returns 0 or -1 if fails
 */
int chats_change_admin(chat_info *chat_info, const char *name);

/*
 * Promotes the member named "name" to chat admin
 * The previous admin is NOT introduced as a chat member
 *
 * Returns 0 or -1 if fails
 */
int chats_promote_to_admin(chat_info *chat_info, const char *name);


/* =========================================================================
 *  Chat member list
 * =========================================================================*/

/*
 * Allocates a new chat member list
 *
 * Returns a pointer to the list phantom node or NULL if fails
 */
chat_member_list *chats_new_member_list();

/*
 * Adds the member to the chat list
 * "*member" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int chats_add_member(chat_member_list *list, chat_member *member);

/*
 * Deletes the first ocurrence of a chat member with the provided "name"
 *
 * Returns 0 or -1 if fails
 */
int chats_del_member(chat_member_list *list, const char *name);

/*
 * Finds the first chat member named "user_name"
 *
 * Returns a pointer to the chat_member struct or NULL if fails
 */
chat_member *chats_find_member(chat_member_list *list, const char *user_name);

/*
 * Creates a new chat_node in the list with the provided info
 * "*info" is attached, not copied
 */
void chats_free_member_list(chat_member_list *list);


/* =========================================================================
 *  Chat members
 * =========================================================================*/

/*
 * Allocates a new chat member struct
 *
 * Returns a pointer to the structure or NULL if fails
 */
chat_member *chats_new_member(friend_info *friend_info);


#endif /* __CHATS */
