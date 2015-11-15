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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef DEBUG
#include "leak_detector_c.h"
#endif


void free_node(friend_node *node) {
	if (node->info != NULL) {
		free(node->info->name);
		free(node->info->information);
		free(node->info);
	}
	free(node);
}

void delete_node(friend_node *node) {	

		// link list
		node->prev->next = node->next;
		node->next->prev = node->prev;

		// free node
		free_node(node);
}

friend_node *find_node(friend_node *list, const char *friend_name) {
	friend_node *aux_node;

	aux_node = list->next;
	while ( aux_node != list ) {
		if (strcmp(aux_node->info->name, friend_name) == 0)
			return aux_node;
		aux_node = aux_node->next;
	}

	return NULL;
}


/*
 * Prints all friends line by line
 */
void friends_print_list(friend_node *list) {
	friend_node *aux_node;

	aux_node = list->next;
	while ( aux_node != list ) {
		printf("%s :%s\n", aux_node->info->name, aux_node->info->information);
		aux_node = aux_node->next;
	}
}


/*
 * Allocates a new friend list
 *
 * Returns a pointer to the list phantom node or NULL if fails
 */
friend_node* friends_new_list() {
	friend_node *new_list = NULL;

	if ( new_list = malloc( sizeof(friend_node) ) ) {
		new_list->info = NULL;
		new_list->next = new_list;
		new_list->prev = new_list;
	}

	return new_list;
}


/*
 * Frees the friend list
 */
void friends_free_list(friend_node *list) {

	while ( list->next != list ) {
		printf("Deleting node: %s\n", list->next->info->name);
		delete_node(list->next);
	}

	free(list);
}


/*
 * Allocates a new friend_info struct with the provided data
 *
 * Returns a pointer to the structure or NULL if fails
 */
friend_info *friends_new_info(const char *name, const char *information){
	friend_info *info;

	if (info = malloc(sizeof(friend_info))) {
		info->name = malloc(strlen(name) + sizeof(char));
		info->information = malloc(strlen(information) + sizeof(char));

		strcpy(info->name, name);
		strcpy(info->information, information);

		return info;
	}

	return NULL;
}


/*
 * Creates a new friend_node in the list with the provided info
 * "*info" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int friends_add(friend_node *list, friend_info *info) {
	friend_node *node;

	if (node = malloc(sizeof(friend_node))) {
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
 * Removes and frees the first node that matches the provided "name"
 *
 * Returns 0 or -1 if "name" does not exist in the list
 */
int friends_del(friend_node *list, const char *name) {
	friend_node *aux_node;	
	if ( aux_node = find_node(list, name) ) {
		delete_node(aux_node);
		return 0;	
	}

	return -1;
}

