/*******************************************************************************
 *	chats.c
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

#include "chats.h"
#include "friends.h"
#include "messages.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef DEBUG_TRACE
	#undef DEBUG_TRACE
#endif

// The macros defined here will check for DEBUG definition
#include "debug_def.h"

#ifdef DEBUG
#include "leak_detector_c.h"
#endif


void _chats_free_member(chat_member *member) {
	DEBUG_TRACE_PRINT();
	// As chat_member is just a wrapper for a friend_info pointer,
	// this function have nothing more to do
	// free(member);
}

void _chats_free_member_list(chat_member_list *list) {
	DEBUG_TRACE_PRINT();
	int i;
	for (i = 0 ; i < list->n_members ; i++) {
		_chats_free_member(&list->members[i]);
	}
	free(list->members);
	free(list);
}

void _chats_free_node(chat_node *node) {
	DEBUG_TRACE_PRINT();
	if (node->info != NULL) {
		free(node->info->description);
		free(node->info->admin);
		mes_free(node->info->messages);
		_chats_free_member_list(node->info->members);
		free(node->info);
	}
	free(node);
}

void _chats_delete_node(chat_node *node) {	
	DEBUG_TRACE_PRINT();
	// link list
	node->prev->next = node->next;
	node->next->prev = node->prev;

	// free node
	_chats_free_node(node);
}

chat_node *_chats_find_node(chat_list *list, int chat_id) {
	DEBUG_TRACE_PRINT();
	chat_node *aux_node;

	aux_node = list->next;
	while ( aux_node != list ) {
		if ( aux_node->info->id == chat_id )
			return aux_node;
		aux_node = aux_node->next;
	}

	return NULL;
}

void _chats_free_info(chat_info *info) {
	DEBUG_TRACE_PRINT();
	if (info != NULL) {
		free(info->description);
		_chats_free_member(info->admin);
		_chats_free_member_list(info->members);
		free(info);
	}
}

int _chats_find_member(chat_member_list *list, const char *name) {
	DEBUG_TRACE_PRINT();
	int i;
	for(i = 0 ; i < list->n_members ; i++) {
		if( strcmp(name, fri_GET_FRIEND_NAME(list->members[i].info)) == 0 ) {
			return i;
		}
	}

	DEBUG_FAILURE_PRINTF("Could not find chat member:%s", name);
	return -1;
}


/* =========================================================================
 *  Chat struct API
 * =========================================================================*/

/*
 * Allocates a new chat list
 * Returns a pointer to the list or NULL if fails
 */
chats *cha_new() {
	DEBUG_TRACE_PRINT();
	return cha_lst_new();
}


/*
 * Frees the chat list
 */
void cha_free(chats *chats) {
	DEBUG_TRACE_PRINT();
	cha_lst_free(chats);
}


/*
 * Prints all chats line by line
 */
void cha_print_chat_list(chats *chats) {
	DEBUG_TRACE_PRINT();
	cha_lst_print(chats);
}


/*
 * Prints all chat members line by line
 */
void cha_print_chat_members(chats *chats, int chat_id) {
	DEBUG_TRACE_PRINT();
	chat_node *node;
	DEBUG_INFO_PRINTF("Print members of chat %d", chat_id);
	if( (node = _chats_find_node(chats, chat_id)) != NULL ) {
		cha_memberlst_print(node->info->members); 
	}
}


/*
 * Creates a new chat in the list with the provided info
 * Returns 0 or -1 if fails
 */
int cha_add_chat(chats *chats, int chat_id, const char *description, friend_info *admin, friend_info *members[], int n_members) {

	DEBUG_TRACE_PRINT();
	int i;
	chat_info *info;
	chat_member *aux_member;
	chat_member *aux_admin;
	chat_member_list *aux_member_list;

	if( (aux_admin = cha_memberinfo_new(admin)) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not create admin info");
		return -1; // can not create admin info
	}
	if( (aux_member_list = cha_memberlst_new()) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not create member list");
		return -1; // can not create member list
	}

	for( i = 0; i < n_members ; i++ ) {
		if( (aux_member = cha_memberinfo_new(members[i])) == NULL ) {
			DEBUG_FAILURE_PRINTF("Could not create member info");
			return -1; // can not create member
		}
		if( cha_memberlst_add(aux_member_list, aux_member) == -1 ) {
			DEBUG_FAILURE_PRINTF("Could not add member to chat");
			return -1; // can not add member to chat
		}
		cha_memberinfo_free(aux_member);
	}

	// create chat info
	if( (info = cha_info_new(chat_id, description, aux_admin, aux_member_list)) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not create chat info");
		return -1; // can not create chat info
	}

	if( (info->messages = mes_new()) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not create message list");
		return -1;
	}

	DEBUG_INFO_PRINTF("Created chat %d with %d members", info->id, info->members->n_members);

	// add chat_info
	if( cha_lst_add(chats, info) == -1 ) {
		DEBUG_FAILURE_PRINTF("Could not add chat");
		return -1; // can not add chat
	}

	DEBUG_INFO_PRINTF("Succesfully created chat %d with %d members", info->id, info->members->n_members);
	return 0;
}


/*
 * Adds the message in the chat
 * Returns 0 or -1 if fails
 */
int cha_add_message(chats *chats, int chat_id, const char *sender, const char *text, int send_date, const char *attach_path) {
	DEBUG_TRACE_PRINT();
	
	chat_info *chat_info;
	if( (chat_info = cha_lst_find(chats, chat_id)) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not find chat");
		return -1;
	}

		if( mes_add_message(chat_info->messages, sender, text, send_date, attach_path) != 0 ) {
			DEBUG_FAILURE_PRINTF("Could not add the message");
			mes_del_last_messages(chat_info->messages, 1); // (i+1) messages (-1) the last failed
			return -1;
		}

	return 0;
}


/*
 * Adds the messages in the chat
 * Returns 0 or -1 if fails
 */
int cha_add_messages(chats *chats, int chat_id, char *sender[], char *text[], int send_date[], char *attach_path[], int n_messages) {
	DEBUG_TRACE_PRINT();
	
	int i;
	chat_info *chat_info;
	if( (chat_info = cha_lst_find(chats, chat_id)) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not find chat");
		return -1;
	}

	for( i=0; i< n_messages; i++ ) {
		if( mes_add_message(chat_info->messages, sender[i], text[i], send_date[i], attach_path[i]) != 0 ) {
			DEBUG_FAILURE_PRINTF("Could not add the message");
			mes_del_last_messages(chat_info->messages, i); // (i+1) messages (-1) the last failed
			return -1;
		}
	}

	return 0;
}


/*
 * Creates a new chat member in the list with the provided info
 * Returns 0 or -1 if fails
 */
int cha_add_member(chats *chats, int chat_id, friend_info *member) {
	DEBUG_TRACE_PRINT();

	chat_node *node;
	chat_member *aux_member;	

	if( (node = _chats_find_node(chats, chat_id)) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not find chat node");
		return -1;
	}

	if( (aux_member = cha_memberinfo_new(member)) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not create chat member");
		return -1; // can not create member
	}
	if( cha_memberlst_add(node->info->members, aux_member) == -1 ) {
		DEBUG_FAILURE_PRINTF("Could not add member to chat");
		cha_memberinfo_free(aux_member);
		return -1; // can not add member to chat
	}
	cha_memberinfo_free(aux_member);

	return 0;
}


/*
 * Deletes chat from the list
 * Returns 0 or -1 if fails
 */
int cha_del_chat(chats *chats, int chat_id) {
	DEBUG_TRACE_PRINT();
	return cha_lst_del(chats, chat_id);
}


/*
 * Deletes chat member from the chat
 * Returns 0 or -1 if fails
 */
int cha_del_member(chats *chats, int chat_id, const char *name) {
	DEBUG_TRACE_PRINT();
	chat_node *node;
	
	if( (node = _chats_find_node(chats, chat_id)) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not find chat");
		return -1; // can not find chat
	}

	if( cha_memberlst_del(node->info->members, name) == -1 ) {
		DEBUG_FAILURE_PRINTF("Could not delete member from chat");
		return -1; // can not delete member from chat
	}

	return 0;
}


/*
 * Sets the number of unread messages
 * Returns 0 or -1 if fails
 */
int cha_set_unread(chats *chats, int chat_id, int n_messages) {
	DEBUG_TRACE_PRINT();
	chat_info *chat_info;
	if( (chat_info = cha_lst_find(chats, chat_id)) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not find chat");
		return -1;
	}

	if( n_messages < 0 ) {
		DEBUG_FAILURE_PRINTF("Can not set unread messages to less than 0");
		return -1;
	}

	cha_SET_N_UNREAD(chat_info, n_messages);
	return 0;
}


/*
 * Updates the number of unread messages, the number of unread will be
 * (current_unread + n_messages), n_messages can be a negative number
 * Returns 0 or -1 if fails
 */
int cha_update_unread(chats *chats, int chat_id, int n_messages) {
	DEBUG_TRACE_PRINT();
	chat_info *chat_info;
	int unread;

	if( (chat_info = cha_lst_find(chats, chat_id)) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not find chat");
		return -1;
	}

	unread = cha_GET_N_UNREAD(chat_info) + n_messages;
	if( unread < 0 ) {
		DEBUG_FAILURE_PRINTF("Can not set unread messages to less than 0");
		return -1;
	}

	cha_SET_N_UNREAD(chat_info, unread);

	return 0;
}


/*
 * Switches the current admin with the chat member named "name"
 * that means that the previous admin becomes a normal member
 * Returns 0 or -1 if fails
 */
int cha_change_admin(chats *chats, int chat_id, const char *name) {
	DEBUG_TRACE_PRINT();
	chat_node *node;
	
	if( (node = _chats_find_node(chats, chat_id)) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not find chat");
		return -1; // can not find chat
	}

	if( cha_info_change_admin(node->info, name) == -1 ) {
		DEBUG_FAILURE_PRINTF("Could not change admin");
		return -1; // can not change admin
	}

	return 0;
}


/*
 * Promotes the member named "name" to chat admin
 * The previous admin is NOT introduced as a chat member
 * Returns 0 or -1 if fails
 */
int cha_promote_to_admin(chats *chats, int chat_id, const char *name) {
	DEBUG_TRACE_PRINT();
	chat_node *node;
	
	if( (node = _chats_find_node(chats, chat_id)) == NULL


 ) {
		DEBUG_FAILURE_PRINTF("Could not find chat");
		return -1; // can not find chat
	}

	if( cha_info_promote_to_admin(node->info, name) == -1 ) {
		DEBUG_FAILURE_PRINTF("Could not promote member to admin");
		return -1; // can not promote to admin
	}

	return 0;
}


/* =========================================================================
 *  Chat list
 * =========================================================================*/

/*
 * Allocates a new chat list
 * Returns a pointer to the list phantom node or NULL if fails
 */
chat_list *cha_lst_new() {
	DEBUG_TRACE_PRINT();
	chat_node *new_list;

	if ( new_list = malloc( sizeof(chat_node) ) ) {
		new_list->info = NULL;
		new_list->next = new_list;
		new_list->prev = new_list;
	}

	return new_list;
}


/*
 * Frees the chat list
 */
void cha_lst_free(chat_list *list) {
	DEBUG_TRACE_PRINT();
	while ( list->next != list ) {
		_chats_delete_node(list->next);
	}

	free(list);
}


/*
 * Prints all chats line by line
 */
void cha_lst_print(chat_node *list) {
	DEBUG_TRACE_PRINT();
	chat_node *aux_node;

	aux_node = list->next;
	while ( aux_node != list ) {
		printf("%d :%s\n", aux_node->info->id, aux_node->info->description);
		aux_node = aux_node->next;
	}
}


/*
 * Creates a new chat_node in the list with the provided info
 * "*info" is attached, not copied
 * Returns 0 or -1 if fails
 */
int cha_lst_add(chat_list *list, chat_info *info) {
	DEBUG_TRACE_PRINT();
	chat_node *node;

	if ( (node = malloc(sizeof(chat_node))) != NULL ) {
		node->info = info;
		node->next = list;
		node->prev = list->prev;
		list->prev->next = node;
		list->prev = node;
		return 0;
	}

	return -1;
}


/*
 * Removes and frees the chat with id "chat_id"
 * Returns 0 or -1 if "chat_id" does not exist in the list
 */
int cha_lst_del(chat_list *list, int chat_id) {
	DEBUG_TRACE_PRINT();
	chat_node *aux_node;	
	if ( (aux_node = _chats_find_node(list, chat_id)) != NULL ) {
		_chats_delete_node(aux_node);
		return 0;	
	}

	return -1;
}


/*
 * Finds the chat whos id is chat_id
 * Returns a pointer to the chat_info of NULL if fails
 */
chat_info *cha_lst_find(chat_list *list, int chat_id) {
	DEBUG_TRACE_PRINT();
	chat_node *chat_node;
	chat_node = _chats_find_node(list, chat_id);	
	return chat_node->info;
}


/* =========================================================================
 *  Chats
 * =========================================================================*/

/*
 * Allocates a new chat_info struct with the provided data
 * Returns a pointer to the structure or NULL if fails
 */
chat_info *cha_info_new(int id, const char *description, chat_member *admin, chat_member_list *members) {
	DEBUG_TRACE_PRINT();
	chat_info *info;

	if (info = malloc(sizeof(chat_info))) {
		info->id = id;
		info->description = malloc(strlen(description) + sizeof(char));
		strcpy(info->description, description);

		info->admin = admin;
		info->members = members;
		return info;
	}

	return NULL;
}


/*
 * Frees the chat_info struct
 * Returns a pointer to the structure or NULL if fails
 */
void cha_info_free(chat_info *info) {
	DEBUG_TRACE_PRINT();
	_chats_free_info(info);
}


/*
 * Switches the current admin with the chat member named "name"
 * that means that the previous admin becomes a normal member
 * Returns 0 or -1 if fails
 */
int cha_info_change_admin(chat_info *chat_info, const char *name) {
	DEBUG_TRACE_PRINT();
	int member_index;
	friend_info *aux_friend_info;

	if( (member_index = _chats_find_member(chat_info->members, name)) == -1 ) {
		DEBUG_FAILURE_PRINTF("Could not find member in chat");
		return -1;  //user "name" is not part of the chat
	}

	aux_friend_info = chat_info->members->members[member_index].info;
	chat_info->members->members[member_index].info = chat_info->admin->info;
	chat_info->admin->info = aux_friend_info;

	return 0;
}


/*
 * Promotes the member named "name" to chat admin
 * The previous admin is NOT introduced as a chat member
 * Returns 0 or -1 if fails
 */
int cha_info_promote_to_admin(chat_info *chat_info, const char *name) {
	DEBUG_TRACE_PRINT();
	int member_index;
	int i;
	friend_info *aux_friend_info;
	chat_member_list *list;

	list = chat_info->members;

	if( (member_index = _chats_find_member(list, name)) == -1 ) {
		DEBUG_FAILURE_PRINTF("Could not find member in chat");
		return -1;  //user "name" is not part of the chat
	}

	aux_friend_info = list->members[member_index].info;
	chat_info->admin->info = aux_friend_info;

	for(i = member_index ; member_index < list->n_members ; i++) {
		list->members[i] = list->members[i+1];
	}
	list->n_members--;

	return 0;
}


/* =========================================================================
 *  Chat member list
 * =========================================================================*/

/*
 * Allocates a new chat member list
 * Returns a pointer to the list phantom node or NULL if fails
 */
chat_member_list *cha_memberlst_new() {
	DEBUG_TRACE_PRINT();
	chat_member_list *list;

	if( (list = malloc( sizeof(chat_member_list) )) == NULL ) {
		return NULL;
	}
	list->members = NULL;
	list->n_members = 0;
	list->list_lenght = 0;

	return list;
}


/*
 * Creates a new chat_node in the list with the provided info
 * "*info" is attached, not copied
 */
void cha_memberlst_free(chat_member_list *list) {
	DEBUG_TRACE_PRINT();
	_chats_free_member_list(list);
}


/*
 * Prints all chat members line by line
 */
void cha_memberlst_print(chat_member_list *list) {
	DEBUG_TRACE_PRINT();
	int i;
	printf("Chat members: %d\n", list->n_members);
	for(i = 0; i < list->n_members ; i++ ) {
		if( list->members[i].info != NULL ) {
		printf("%s: %s\n", fri_GET_FRIEND_NAME(list->members[i].info), 
					fri_GET_FRIEND_INFORMATION(list->members[i].info));
		}
		else {
			printf("Not a friend\n");
		}
	}
}


/*
 * Adds the member to the chat list
 * "*member" is COPIED
 * Returns 0 or -1 if fails
 */
int cha_memberlst_add(chat_member_list *list, chat_member *member) {
	DEBUG_TRACE_PRINT();


	if( list->members == NULL) {
		if ( (list->members = malloc(sizeof(chat_member)*(list->list_lenght + 3)) ) == NULL ) {
			return -1;
		}
		list->list_lenght += 3;
	}
	else if(list->list_lenght = list->n_members) {
		if ( (list->members = realloc(list->members, sizeof(chat_member)*(list->list_lenght + 5)) ) == NULL ) {
			return -1;
		}
		list->list_lenght += 5;
	}

	DEBUG_INFO_PRINTF("Add member %s to list", member->info->name);
	list->members[list->n_members].info = member->info;
	list->n_members++;

	return 0;
}


/*
 * Deletes the first ocurrence of a chat member with the provided "name"
 * Returns 0 or -1 if fails
 */
int cha_memberlst_del(chat_member_list *list, const char *name) {
	DEBUG_TRACE_PRINT();

	int member_index;
	int i;


	if( (member_index = _chats_find_member(list, name)) == -1 ) {
		return -1;
	}

	for(i = member_index ; member_index < list->n_members-1 ; i++) {
		list->members[i] = list->members[i+1];
	}
	list->n_members--;

	if( (list->list_lenght - list->n_members) > 5 ) {
		if ( (list->members = realloc(list->members, sizeof(chat_member)*(list->list_lenght - 5)) ) == NULL ) {
			return -1;
		}
	}
	list->list_lenght -= 5;
}


/*
 * Finds the first chat member named "user_name"
 * Returns a pointer to the chat_member struct or NULL if fails
 */
chat_member *cha_memberlst_find(chat_member_list *list, const char *user_name) {
	DEBUG_TRACE_PRINT();
	int member_index;
	if( (member_index = _chats_find_member(list, user_name)) == -1 ) {
		return NULL;
	}

	return &list->members[member_index];
}


/* =========================================================================
 *  Chat members
 * =========================================================================*/

/*
 * Allocates a new chat member struct
 * Returns a pointer to the structure or NULL if fails
 */
chat_member *cha_memberinfo_new(friend_info *friend_info) {
	DEBUG_TRACE_PRINT();
	chat_member *member;

	if( (member = malloc( sizeof(chat_member) )) == NULL ) {
		return NULL;
	}
	member->info = friend_info;

	return member;
}


/*
 * Frees the chat member
 * Returns a pointer to the structure or NULL if fails
 */
chat_member *cha_memberinfo_free(chat_member *info) {
	DEBUG_TRACE_PRINT();
	// The friend_info pointer contained in chat_member is expected to exist
	// in the friend_list, so it can not be freed here
	free(info);
}


