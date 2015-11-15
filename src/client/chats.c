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


/*
 * Prints all chats line by line
 */
void chats_print_list(chat_node *list){

}


/*
 * Allocates a new chat list
 *
 * Returns a pointer to the list phantom node or NULL if fails
 */
chat_node *chats_new_list() {
	return NULL;
}


/*
 * Frees the chat list
 */
void chats_free_list(chat_node *list) {

}


/*
 * Allocates a new chat_info struct with the provided data
 *
 * Returns a pointer to the structure or NULL if fails
 */
chat_info *chats_new_info(const char *description, chat_member *admin, chat_member_list *members) {
	return NULL;
}


chat_member_list *chats_new_member_list() {
	return NULL;
}

chat_member *chats_new_member(friend_info *friend_info) {
	return NULL;
}

int chats_add_member(chat_member_list *list, chat_member *member) {
	return -1;
}

int chats_del_member(chat_member_list *list, const char *name) {
	return -1;
}

int chats_change_admin(chat_info *chat_info, const char *name) {
	return -1;
}

int chats_promote_to_admin(chat_info *chat_info, const char *name) {
	return -1;
}

chat_member *chats_find_member(chat_member_list *list, const char *user_name) {
	return NULL;
}

void chats_free_member_list(chat_member_list *list) {

}

/*
 * Creates a new chat_node in the list with the provided info
 * "*info" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int chats_add(chat_node *list, chat_info *info) {
	return -1;
}


/*
 * Removes and frees the first node that matches the provided "name"
 *
 * Returns 0 or -1 if "name" does not exist in the list
 */
int chats_del(friend_node *list, const char *name) {
	return -1;
}

