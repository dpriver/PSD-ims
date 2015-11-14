/*******************************************************************************
 *	friends.c
 *
 *  <ONE LINE DESCRIPTION.>
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
#include "bool.h"
#include <stdlib.h>
#include <stdio.h>

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
	aux_node = list;

	do {
		if (strcmp(aux_node->info->name, friend_name) != 0) {
			return aux_node;
		}
		aux_node = aux_node->next;
	} while (aux_node != list);

	return NULL;
}

void print_friend_list(friend_node *list) {
	friend_node *aux_node;
	aux_node = list;

	do {
		printf("%s %s", aux_node->info->name, aux_node->info->information);
		aux_node = aux_node->next;
	} while (aux_node != list);
}

friend_node* new_friend_list() {
	friend_node *new_list;

#ifdef DEBUG
	printf("Debug mode...\n");
#endif

	if ( new_list = malloc( sizeof(friend_node) ) ) {
		new_list->info = NULL;
		new_list->next = new_list;
		new_list->prev = new_list;
	}

	return new_list;
}

/*
 * Returns 0 if success 
 * Returns < 0 if error
 */
int free_friend_list(friend_node *list) {

	while ( list->next != list ) {
		printf("Deleting node: %s\n", list->next->info->name);
		delete_node(list->next);
	}

	free(list);
	return 0;
}

/*
 * Returns 0 if success 
 * Returns < 0 if error
 */
int add_friend(friend_node *list, friend_info *info) {
	friend_node *node;

	if (node = malloc(sizeof(friend_node))) {
		node->info = info;
		node->next = list->next;
		node->prev = list;
		list->next = node;
		if (list->prev = list) {
			list->prev = node;
		}
		return 0;
	}

	return -1;
}

/*
 * Returns 0 if success 
 * Returns < 0 if error
 */
int del_friend(friend_node *list, const char *name) {
	friend_node *aux_node;	
	if (aux_node = find_node(list, name)) {
		delete_node(list->next);
		return 0;	
	}

	return -1;
}

/*
 * Returns true of false wheter friend_name is in the list or not
 */
boolean is_friend(friend_node *list, char *name) {
	return (find_node(list, name) != NULL);
}
