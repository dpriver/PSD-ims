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
#include "messages.h"

typedef struct chat_member chat_member;
struct chat_member {
	friend_info *info;
};

typedef struct chat_member_list chat_member_list;
struct chat_member_list {
	chat_member *members;
	int n_members;
	int list_lenght;
};

typedef struct chat_info chat_info;
struct chat_info {
	int id;
	char *description;
	chat_member *admin;
	chat_member_list *members;
	messages *messages;
};

typedef struct chat_node chat_node;
struct chat_node {
	chat_info *info;
	chat_node *next;
	chat_node *prev;
};

typedef chat_node chat_list;
typedef chat_list chats;


/* =========================================================================
 *  Structs access macros
 * =========================================================================*/
#define cha_GET_CHAT_ID(chat_info) \
		chat_info->id

#define cha_GET_FRIEND_INFORMATION(chat_info) \
		chat_info->description

#define cha_GET_N_MEMBERS(chat_info) \
		chat_info->members->n_members

#define cha_GET_MEMBER_NAME(chat_member) \
		fri_GET_FRIEND_NAME(chat_member->info)

#define cha_GET_MEMBER_INFORMATION(chat_member) \
		fri_GET_FRIEND_INFORMATION(chat_member->info)


/* =========================================================================
 *  Chat struct API
 * =========================================================================*/

/*
 * Allocates a new chat list
 * Returns a pointer to the list or NULL if fails
 */
chats *cha_new();

/*
 * Frees the chat list
 */
void cha_free(chats *chats);

/*
 * Prints all chats line by line
 */
void cha_print_chat_list(chats *chats);

/*
 * Prints all chat members line by line
 */
void cha_print_chat_members(chats *chats, int chat_id);

/*
 * Creates a new chat in the list with the provided info
 * Returns 0 or -1 if fails
 */
int cha_add_chat(chats *chats, int chat_id, const char *description, friend_info *admin, friend_info *members[], int n_members);

/*
 * Creates a new chat member in the list with the provided info
 * Returns 0 or -1 if fails
 */
int cha_add_member(chats *chats, int chat_id, friend_info *member);

/*
 * Deletes chat from the list
 * Returns 0 or -1 if fails
 */
int cha_del_chat(chats *chats, int chat_id);

/*
 * Deletes chat member from the chat
 * Returns 0 or -1 if fails
 */
int cha_del_member(chats *chats, int chat_id, const char *name);

/*
 * Switches the current admin with the chat member named "name"
 * that means that the previous admin becomes a normal member
 * Returns 0 or -1 if fails
 */
int cha_change_admin(chats *chats, int chat_id, const char *name);

/*
 * Promotes the member named "name" to chat admin
 * The previous admin is NOT introduced as a chat member
 * Returns 0 or -1 if fails
 */
int cha_promote_to_admin(chats *chats, int chat_id, const char *name);


/* =========================================================================
 *  Chat list
 * =========================================================================*/

/*
 * Allocates a new chat list
 * Returns a pointer to the list phantom node or NULL if fails
 */
chat_list *cha_lst_new();

/*
 * Frees the chat list
 */
void cha_lst_free(chat_list *list);

/*
 * Prints all chats line by line
 */
void cha_lst_print(chat_node *list);

/*
 * Creates a new chat_node in the list with the provided info
 * "*info" is attached, not copied
 * Returns 0 or -1 if fails
 */
int cha_lst_add(chat_list *list, chat_info *info);

/*
 * Removes and frees the chat with id "chat_id"
 * Returns 0 or -1 if "chat_id" does not exist in the list
 */
int cha_lst_del(chat_list *list, int chat_id);

/*
 * Finds the chat whos id is chat_id
 * Returns a pointer to the chat_info of NULL if fails
 */
chat_info *cha_lst_find(chat_list *list, int chat_id);



/* =========================================================================
 *  Chats
 * =========================================================================*/

/*
 * Allocates a new chat_info struct with the provided data
 * Returns a pointer to the structure or NULL if fails
 */
chat_info *cha_info_new(int id, const char *description, chat_member *admin, chat_member_list *members);

/*
 * Frees the chat_info struct
 * Returns a pointer to the structure or NULL if fails
 */
void cha_info_free(chat_info *info);

/*
 * Switches the current admin with the chat member named "name"
 * that means that the previous admin becomes a normal member
 * Returns 0 or -1 if fails
 */
int cha_info_change_admin(chat_info *chat_info, const char *name);

/*
 * Promotes the member named "name" to chat admin
 * The previous admin is NOT introduced as a chat member
 * Returns 0 or -1 if fails
 */
int cha_info_promote_to_admin(chat_info *chat_info, const char *name);


/* =========================================================================
 *  Chat member list
 * =========================================================================*/

/*
 * Allocates a new chat member list
 * Returns a pointer to the list phantom node or NULL if fails
 */
chat_member_list *cha_memberlst_new();

/*
 * Creates a new chat_node in the list with the provided info
 * "*info" is attached, not copied
 */
void cha_memberlst_free(chat_member_list *list);

/*
 * Prints all chat members line by line
 */
void cha_memberlst_print(chat_member_list *list);

/*
 * Adds the member to the chat list
 * "*member" is attached, not copied
 * Returns 0 or -1 if fails
 */
int cha_memberlst_add(chat_member_list *list, chat_member *member);

/*
 * Deletes the first ocurrence of a chat member with the provided "name"
 * Returns 0 or -1 if fails
 */
int cha_memberlst_del(chat_member_list *list, const char *name);

/*
 * Finds the first chat member named "user_name"
 * Returns a pointer to the chat_member struct or NULL if fails
 */
chat_member *cha_memberlst_find(chat_member_list *list, const char *user_name);


/* =========================================================================
 *  Chat members
 * =========================================================================*/

/*
 * Allocates a new chat member struct
 * Returns a pointer to the structure or NULL if fails
 */
chat_member *cha_memberinfo_new(friend_info *friend_info);

/*
 * Frees the chat member
 * Returns a pointer to the structure or NULL if fails
 */
chat_member *cha_memberinfo_free(chat_member *info);


#endif /* __CHATS */
