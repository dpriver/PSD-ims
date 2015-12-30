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

void friend_list_info_free(void *info) {
	free(info);
}

void friend_free(void *friend) {
	free(((friend_info*)friend)->name);
	free(((friend_info*)friend)->information);
	free(friend);
}

int friend_name_comp(const void *friend, const void *name) {
	return strcmp(((friend_info*)friend)->name, (char*)name);
}

int friend_comp(const void *friend1, const void *friend2) {
	return strcmp(((friend_info*)friend1)->name, ((friend_info*)friend2)->name);
}

/* =========================================================================
 *  Friend struct API
 * =========================================================================*/

/*
 * Allocates a new friend list
 * Returns a pointer to the list phantom node or NULL if fails
 */
friends *fri_new(int max) {
	DEBUG_TRACE_PRINT();
	friends *friends_new;
	friend_list_info *list_info;

	if ( (list_info = malloc(sizeof(friend_list_info))) == NULL ) {
		return NULL;
	}

	list_info->timestamp = 0;

	if ( (friends_new = list_new(list_info, max, friend_list_info_free, friend_free)) == NULL ) {
		list_info_free(list_info);
		return NULL; // could not allocate list
	}
	friends_new->item_value_comp = friend_name_comp;
	friends_new->item_comp = friend_comp;

	return friends_new;
}


/*
 * Frees the friend list
 */
void fri_free(friends *friends) {
	DEBUG_TRACE_PRINT();

	list_free(friends);
}


/*
 * Prints all friends line by line
 */
void fri_print_friend_list(friends *friends) {
	DEBUG_TRACE_PRINT();
	//fri_lst_print(friends->friend_list);
}


/*
 * Creates a new friend_node in the list with the provided info
 * "*info" is attached, not copied
 * Returns 0 or -1 if fails
 */
int fri_add_friend(friends *friends, const char *name, const char *information) {
	DEBUG_TRACE_PRINT();
	friend_info *info;

	if ( (info = malloc(sizeof(friend_info))) == NULL ) {
		return -1;
	}
	
	if ( (info->name = malloc(sizeofstring(name)) ) == NULL ) {
		free(info);
		return -1;
	}
	
	if ( (info->information = malloc(sizeofstring(information)) ) == NULL ) {
		free(info->name);
		free(info);
		return -1;
	}

	strcpy(info->name, name);
	strcpy(info->information, information);

	if ( list_add_item(friends, info) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not add friend to list");
		friend_free(info);
		return -1;
	}

	return 0;
}


/*
 * Removes and frees the first node that matches the provided "name"
 * Returns 0 or -1 if "name" does not exist in the list
 */
int fri_del_friend(friends *friends, const char *name) {
	DEBUG_TRACE_PRINT();
	list_node *node;
	node = list_find_node(friends, name);
	if (node == NULL) {
		DEBUG_FAILURE_PRINTF("Friend does not exists in list");
		return -1;
	}
	
	list_delete_node(friends, node);
	return 0;
}


/*
 * Search for the friend in the list
 * Returns a pointer to the friend_info or NULL if fails
 */
friend_info *fri_find_friend(friends *friends, const char *name) {
	DEBUG_TRACE_PRINT();
	
	return (friend_info*)list_find_item(friends, name);
}
