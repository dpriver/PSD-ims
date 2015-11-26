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
#include <stdlib.h>

#include "friends.h"
#include "leak_detector_c.h"


int main (int argc, char **argv) {
	
	friends *friends;
		
	atexit(report_mem_leak);

	printf("Creating new friend list\n");
	friends = fri_new();

	printf("Lista de amigos:\n");
	printf("---------------------------\n");
	fri_print_friend_list(friends);
	printf("---------------------------\n");

	printf("Adding friends to list\n");
	fri_add_friend(friends, "Juanito", "Es mi amigo de toda la vida...");
	fri_add_friend(friends, "pepito", "Es mi Enemigoo de toda la vida...");
	fri_add_friend(friends, "Manolito", "Es Culero de toda la vida...");
	fri_add_friend(friends, "Qewdqew", "Es ...");
	printf("---------------------------\n");
	fri_print_friend_list(friends);
	printf("---------------------------\n");
	

	printf("Removing friend list\n");
	if(fri_del_friend(friends, "Manolito") != 0) {
		printf("Fallo al borrar Manolito\n");
	}
	if(fri_del_friend(friends, "Qewdqew") != 0) {
		printf("Fallo al borrar Qewdqew\n");
	}
	if(fri_del_friend(friends, "Juanito") != 0) {
		printf("Fallo al borrar Juanito\n");
	}
	printf("---------------------------\n");
	fri_print_friend_list(friends);
	printf("---------------------------\n");

	fri_free(friends);

	return 0;
}
