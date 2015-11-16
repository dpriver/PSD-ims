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
	// this function have nothing to do
}

void _chats_free_member_list(chat_member_list *list) {
	int i;
	for (i = 0 ; i < list->n_members ; i++ ) {
		_chats_free_member(&list->members[i]);
	}
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


/* =========================================================================
 *  Chat list
 * =========================================================================*/

/*
 * Prints all chats line by line
 */
void chats_print_list(chat_node *list) {
	chat_node *aux_node;

	aux_node = list->next;
	while ( aux_node != list ) {
		printf("%d :%s\n", aux_node->info->id, aux_node->info->description);
		aux_node = aux_node->next;
	}
}

/*
 * Allocates a new chat list
 *
 * Returns a pointer to the list phantom node or NULL if fails
 */
chat_list *chats_new_list() {
	chat_node *new_list = NULL;

	if ( new_list = malloc( sizeof(chat_node) ) ) {
		new_list->info = NULL;
		new_list->next = new_list;
		new_list->prev = new_list;
	}

	return new_list;
}

/*
 * Creates a new chat_node in the list with the provided info
 * "*info" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int chats_add(chat_list *list, chat_info *info) {
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
 *
 * Returns 0 or -1 if "chat_id" does not exist in the list
 */
int chats_del(chat_list *list, int chat_id) {
	chat_node *aux_node;	
	if ( aux_node = _chats_find_node(list, chat_id) ) {
		_chats_delete_node(aux_node);
		return 0;	
	}

	return -1;
}

/*
 * Finds the chat whos id is chat_id
 *
 * Returns a pointer to the chat_info of NULL if fails
 */
chat_info *chats_find(chat_list *list, int chat_id) {
	chat_node *chat_node;
	chat_node = _chats_find_node(list, chat_id);	
	return chat_node->info;
}

/*
 * Frees the chat list
 */
void chats_free_list(chat_list *list) {

	while ( list->next != list ) {
		printf("Deleting node: %d\n", list->next->info->id);
		_chats_delete_node(list->next);
	}

	free(list);
}


/* =========================================================================
 *  Chats
 * =========================================================================*/

/*
 * Allocates a new chat_info struct with the provided data
 *
 * Returns a pointer to the structure or NULL if fails
 */
chat_info *chats_new_info(const char *description, chat_member *admin, chat_member_list *members) {
	chat_info *info;

	if (info = malloc(sizeof(chat_info))) {
		info->id = 0;  //TODO use a real id
		info->description = malloc(strlen(description) + sizeof(char));
		strcpy(info->description, description);

		info->admin = admin;
		info->members = members;
		return info;
	}

	return NULL;
}

/*
 * Switches the current admin with the chat member named "name"
 * that means that the previous admin becomes a normal member
 *
 * Returns 0 or -1 if fails
 */
int chats_change_admin(chat_info *chat_info, const char *name) {
	return -1;
}

/*
 * Promotes the member named "name" to chat admin
 * The previous admin is NOT introduced as a chat member
 *
 * Returns 0 or -1 if fails
 */
int chats_promote_to_admin(chat_info *chat_info, const char *name) {
	return -1;
}


/* =========================================================================
 *  Chat member list
 * =========================================================================*/

/*
 * Allocates a new chat member list
 *
 * Returns a pointer to the list phantom node or NULL if fails
 */
chat_member_list *chats_new_member_list() {
	return NULL;
}

/*
 * Adds the member to the chat list
 * "*member" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int chats_add_member(chat_member_list *list, chat_member *member) {
	return -1;
}

/*
 * Deletes the first ocurrence of a chat member with the provided "name"
 *
 * Returns 0 or -1 if fails
 */
int chats_del_member(chat_member_list *list, const char *name) {
	return -1;
}

/*
 * Finds the first chat member named "user_name"
 *
 * Returns a pointer to the chat_member struct or NULL if fails
 */
chat_member *chats_find_member(chat_member_list *list, const char *user_name) {
	return NULL;
}

/*
 * Creates a new chat_node in the list with the provided info
 * "*info" is attached, not copied
 */
void chats_free_member_list(chat_member_list *list) {

}


/* =========================================================================
 *  Chat members
 * =========================================================================*/

/*
 * Allocates a new chat member struct
 *
 * Returns a pointer to the structure or NULL if fails
 */
chat_member *chats_new_member(friend_info *friend_info) {
	return NULL;
}
