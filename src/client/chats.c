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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef DEBUG
#include "leak_detector_c.h"
#endif


void _chats_free_member(chat_member *member) {
	// As chat_member is just a wrapper for a friend_info pointer,
	// this function have nothing more to do
	free(member);
}

void _chats_free_member_list(chat_member_list *list) {
	int i;
	for (i = 0 ; i < list->n_members-1 ; i++) {
		_chats_free_member(&list->members[i]);
	}
	free(list->members);
	free(list);
}

void _chats_free_node(chat_node *node) {
	if (node->info != NULL) {
		free(node->info->description);
		_chats_free_member(node->info->admin);
		_chats_free_member_list(node->info->members);
		free(node->info);
	}
	free(node);
}

void _chats_delete_node(chat_node *node) {	

		// link list
		node->prev->next = node->next;
		node->next->prev = node->prev;

		// free node
		_chats_free_node(node);
}

chat_node *_chats_find_node(chat_list *list, int chat_id) {
	chat_node *aux_node;

	aux_node = list->next;
	while ( aux_node != list ) {
		if ( aux_node->info->id = chat_id )
			return aux_node;
		aux_node = aux_node->next;
	}

	return NULL;
}

void _chats_free_info(chat_info *info) {
	if (info != NULL) {
		free(info->description);
		_chats_free_member(info->admin);
		_chats_free_member_list(info->members);
		free(info);
	}
}

int _chats_find_member(chat_member_list *list, const char *name) {
	int i;
	for(i = 0 ; i < list->n_members-1 ; i++) {
		if( strcmp(name, fri_GET_FRIEND_NAME(list->members[i].info)) == 0 ) {
			return i;
		}
	}

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
	return cha_lst_new();
}


/*
 * Frees the chat list
 */
void cha_free(chats *chats) {
	cha_lst_free(chats);
}


/*
 * Prints all chats line by line
 */
void cha_print_chat_list(chats *chats) {
	cha_lst_print(chats);
}


/*
 * Prints all chat members line by line
 */
void cha_print_chat_members(chats *chats, int chat_id) {
	chat_node *node;
	if( (node = _chats_find_node(chats, chat_id)) != NULL ) {
		cha_memberlst_print(node->info->members); 
	}
}


/*
 * Creates a new chat in the list with the provided info
 * Returns 0 or -1 if fails
 */
int cha_add_chat(chats *chats, int chat_id, const char *description, friend_info *admin, friend_info *members[], int n_members) {
	int i;
	chat_info *info;
	chat_member *aux_member;
	chat_member *aux_admin;
	chat_member_list *aux_member_list;

#ifdef DEBUG
	printf("[DEBUG]: Creating admin and member list\n");
#endif
	if( (aux_admin = cha_memberinfo_new(admin)) == NULL ) {
		return -1; // can not create admin info
	}
	if( (aux_member_list = cha_memberlst_new()) == NULL ) {
		return -1; // can not create member list
	}

#ifdef DEBUG
	printf("[DEBUG]: Adding members to list\n");
#endif
	for( i = 0; i < n_members-1 ; i++ ) {
		if( (aux_member = cha_memberinfo_new(members[i])) == NULL ) {
			return -1; // can not create member
		}
		if( cha_memberlst_add(aux_member_list, aux_member) == -1 ) {
			return -1; // can not add member to chat
		}
		cha_memberinfo_free(aux_member);
	}

#ifdef DEBUG
	printf("[DEBUG]: Creating chat info\n");
#endif
	// create chat info
	if( (info = cha_info_new(chat_id, description, aux_admin, aux_member_list)) == NULL ) {
		return -1; // can not create chat info
	}

#ifdef DEBUG
	printf("[DEBUG]: adding chat to list\n");
#endif
	// add chat_info
	if( cha_lst_add(chats, info) == -1 ) {
		return -1; // can not add chat
	}

	return 0;
}


/*
 * Creates a new chat member in the list with the provided info
 * Returns 0 or -1 if fails
 */
int cha_add_member(chats *chats, int chat_id, friend_info *member) {
	chat_node *node;
	chat_member *aux_member;	

	if( (node = _chats_find_node(chats, chat_id)) == NULL ) {
		return -1;
	}

	if( (aux_member = cha_memberinfo_new(member)) == NULL ) {
		return -1; // can not create member
	}
	if( cha_memberlst_add(node->info->members, aux_member) == -1 ) {
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
	return cha_lst_del(chats, chat_id);
}


/*
 * Deletes chat member from the chat
 * Returns 0 or -1 if fails
 */
int cha_del_member(chats *chats, int chat_id, const char *name) {
	chat_node *node;
	
	if( (node = _chats_find_node(chats, chat_id)) == NULL ) {
		return -1; // can not find chat
	}

	if( cha_memberlst_del(node->info->members, name) == -1 ) {
		return -1; // can not delete member from chat
	}

	return 0;
}


/*
 * Switches the current admin with the chat member named "name"
 * that means that the previous admin becomes a normal member
 * Returns 0 or -1 if fails
 */
int cha_change_admin(chats *chats, int chat_id, const char *name) {
	chat_node *node;
	
	if( (node = _chats_find_node(chats, chat_id)) == NULL ) {
		return -1; // can not find chat
	}

	if( cha_info_change_admin(node->info, name) == -1 ) {
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
	chat_node *node;
	
	if( (node = _chats_find_node(chats, chat_id)) == NULL


 ) {
		return -1; // can not find chat
	}

	if( cha_info_promote_to_admin(node->info, name) == -1 ) {
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
	while ( list->next != list ) {
#ifdef DEBUG
		printf("Deleting node: %d\n", list->next->info->id);
#endif
		_chats_delete_node(list->next);
	}

	free(list);
}


/*
 * Prints all chats line by line
 */
void cha_lst_print(chat_node *list) {
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
	chat_node *node;

	if (node = malloc(sizeof(chat_node))) {
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
	chat_node *aux_node;	
	if ( aux_node = _chats_find_node(list, chat_id) ) {
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
	_chats_free_info(info);
}


/*
 * Switches the current admin with the chat member named "name"
 * that means that the previous admin becomes a normal member
 * Returns 0 or -1 if fails
 */
int cha_info_change_admin(chat_info *chat_info, const char *name) {
	int member_index;
	friend_info *aux_friend_info;

	if( (member_index = _chats_find_member(chat_info->members, name)) == -1 ) {
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
	int member_index;
	int i;
	friend_info *aux_friend_info;
	chat_member_list *list;

	list = chat_info->members;

	if( (member_index = _chats_find_member(list, name)) == -1 ) {
		return -1;  //user "name" is not part of the chat
	}

	aux_friend_info = list->members[member_index].info;
	chat_info->admin->info = aux_friend_info;

	for(i = member_index ; member_index < list->n_members-1 ; i++) {
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
	chat_member_list *list;

	if( (list = malloc( sizeof(chat_member_list) )) == NULL ) {
		return NULL;
	}
	list->n_members = 0;
	list->list_lenght = 0;

	return list;
}


/*
 * Creates a new chat_node in the list with the provided info
 * "*info" is attached, not copied
 */
void cha_memberlst_free(chat_member_list *list) {
	_chats_free_member_list(list);
}


/*
 * Prints all chat members line by line
 */
void cha_memberlst_print(chat_member_list *list) {
	int i;
	for(i = 0; i < list->n_members-1 ; i++ ) {
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

	if(list->list_lenght = list->n_members) {
		if ( (list->members = realloc(list->members, sizeof(chat_member)*(list->list_lenght + 5)) ) == NULL ) {
			return -1;
		}
	}
	list->list_lenght += 5;

	list->members[list->n_members].info = member->info;
	list->n_members++;

	return 0;
}


/*
 * Deletes the first ocurrence of a chat member with the provided "name"
 * Returns 0 or -1 if fails
 */
int cha_memberlst_del(chat_member_list *list, const char *name) {
	int member_index;
	int i;

	if( (member_index = _chats_find_member(list, name)) == -1 ) {
		return -1;
	}

	for(i = member_index ; member_index < list->n_members-2 ; i++) {
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
	// The friend_info pointer contained in chat_member is expected to exist
	// in the friend_list, so it can not be freed here
	free(info);
}


