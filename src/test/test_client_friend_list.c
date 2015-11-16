/*******************************************************************************
 *	friends.c
 *
 *  Program to test the client friend list
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


int main (int argc, char **argv) {
	
	friend_node *friend_list;
	friend_info *info;
		
	atexit(report_mem_leak);

	printf("Creating new friend list\n");
	friend_list = friends_new_list();

	printf("Lista de amigos:\n");
	printf("---------------------------\n");
	friends_print_list(friend_list);
	printf("---------------------------\n");

	printf("Adding friends to list\n");
	info = friends_new_info("Juanito", "Es mi amigo de toda la vida...");
	friends_add(friend_list, info);
	info = friends_new_info("pepito", "Es mi Enemigoo de toda la vida...");
	friends_add(friend_list, info);
	info = friends_new_info("Manolito", "Es Culero de toda la vida...");
	friends_add(friend_list, info);
	info = friends_new_info("Qewdqew", "Es ...");
	friends_add(friend_list, info);
	printf("---------------------------\n");
	friends_print_list(friend_list);
	printf("---------------------------\n");
	

	printf("Removing friend list\n");
	if(friends_del(friend_list, "Manolito") != 0) {
		printf("Fallo al borrar Manolito\n");
	}
	if(friends_del(friend_list, "Qewdqew") != 0) {
		printf("Fallo al borrar Qewdqew\n");
	}
	if(friends_del(friend_list, "Juanito") != 0) {
		printf("Fallo al borrar Juanito\n");
	}
	printf("---------------------------\n");
	friends_print_list(friend_list);
	printf("---------------------------\n");

	friends_free_list(friend_list);

	return 0;
}
