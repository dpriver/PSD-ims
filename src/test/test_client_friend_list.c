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

#include <string.h>
#include <stdio.h>

#include "friends.h"
#include "leak_detector_c.h"


void _free_node(friend_node *node) {
	if (node->info != NULL) {
		free(node->info->name);
		free(node->info->information);
		free(node->info);
	}
	free(node);
}

void _delete_node(friend_node *node) {	

		// link list
		node->prev->next = node->next;
		node->next->prev = node->prev;

		// free node
		_free_node(node);
}

int _add_friend(friend_node *list, friend_info *info) {
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

int _free_friend_list(friend_node *list) {

	while ( list->next != list ) {
		printf("Deleting node: %s\n", list->next->info->name);
		_delete_node(list->next);
	}

	free(list);
	return 0;
}

int main (int argc, char **argv) {
	
	friend_node *friend_list;
	friend_info *info;
		
	atexit(report_mem_leak);

	printf("Creating new friend list\n");
	friend_list = new_friend_list();

	printf("Lista de amigos:\n");
	printf("---------------------------\n");
	print_friend_list(friend_list);
	printf("---------------------------\n");

	printf("Adding friends to list\n");
	info = new_friend_info("Juanito", "Es mi amigo de toda la vida...");
	add_friend(friend_list, info);
	info = new_friend_info("pepito", "Es mi Enemigoo de toda la vida...");
	add_friend(friend_list, info);
	info = new_friend_info("Manolito", "Es Culero de toda la vida...");
	add_friend(friend_list, info);
	info = new_friend_info("Qewdqew", "Es ...");
	add_friend(friend_list, info);
	printf("---------------------------\n");
	print_friend_list(friend_list);
	printf("---------------------------\n");
	

	printf("Removing friend list\n");
	if(del_friend(friend_list, "Manolito") != 0) {
		printf("Fallo al borrar Manolito\n");
	}
	if(del_friend(friend_list, "Qewdqew") != 0) {
		printf("Fallo al borrar Qewdqew\n");
	}
	if(del_friend(friend_list, "Juanito") != 0) {
		printf("Fallo al borrar Juanito\n");
	}
	printf("---------------------------\n");
	print_friend_list(friend_list);
	printf("---------------------------\n");

	free_friend_list(friend_list);

	return 0;
}
