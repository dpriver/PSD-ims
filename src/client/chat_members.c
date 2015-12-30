/*******************************************************************************
 *	friends.c
 *
 *  Client friend list
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

#include "chat_members.h"
#include "friends.h"
#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "debug_def.h"

#ifdef DEBUG
#include "leak_detector_c.h"
#endif


#define sizeofstring(string) \
	(strlen(string) + sizeof(char))

void member_list_info_free(void *info) {
	free(info);
}

void member_free(void *member) {
	free(((member_info*)member)->name);
	free(member);
}

int member_name_comp(const void *member, const void *name) {
	return strcmp(((member_info*)member)->name, (char*)name);
}

int member_comp(const void *member1, const void *member2) {
	return strcmp(((member_info*)member1)->name, ((member_info*)member2)->name);
}



/* =========================================================================
 *  Chat member struct API
 * =========================================================================*/

/*
 * Allocates a new member list
 * Returns a pointer to the list or NULL if fails
 */
friends *members_new(int max) {
	DEBUG_TRACE_PRINT();
	chat_members *members_new;
	member_list_info *list_info;

	if ( (list_info = malloc(sizeof(member_list_info))) == NULL ) {
		return NULL;
	}

	list_info->timestamp = 0;

	if ( (members_new = list_new(list_info, max, member_list_info_free, member_free)) == NULL ) {
		list_info_free(list_info);
		return NULL; // could not allocate list
	}
	members_new->item_value_comp = member_name_comp;
	members_new->item_comp = member_comp;

	return members_new;
}


/*
 * Frees the member list
 */
void members_free(chat_members *members) {
	DEBUG_TRACE_PRINT();

	list_free(members);
}


/*
 * Prints all members line by line
 */
void members_print_friend_list(chat_members *members) {
	DEBUG_TRACE_PRINT();
}


/*
 * Creates a new chat member in the list with the provided info
 * Returns 0 or -1 if fails
 */
int members_add_member(chat_members *members, const char *name, friend_info *friend_info) {
	DEBUG_TRACE_PRINT();
	member_info *info;

	if ( (info = malloc(sizeof(member_info))) == NULL ) {
		return -1;
	}
	
	if ( (info->name = malloc(sizeofstring(name)) ) == NULL ) {
		free(info);
		return -1;
	}

	strcpy(info->name, name);
	info->info = friend_info;

	if ( list_add_item(members, info) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not add friend to list");
		member_free(info);
		return -1;
	}

	return 0;	
}


/*
 * Removes and frees the first node that matches the provided "name"
 * Returns 0 or -1 if "name" does not exist in the list
 */
int members_del_member(chat_members *members, const char *name) {
	DEBUG_TRACE_PRINT();
	list_node *node;
	node = list_find_node(members, name);
	if (node == NULL) {
		DEBUG_FAILURE_PRINTF("Member does not exists in list");
		return -1;
	}
	
	list_delete_node(members, node);
	return 0;
}


/*
 * Finds the chat whos id is chat_id
 * Returns a pointer to the chat_info of NULL if fails
 */
member_info *member_find_member(chat_members *members, const char *name) {
	DEBUG_TRACE_PRINT();
	
	return (member_info*)list_find_item(members, name);	
}
