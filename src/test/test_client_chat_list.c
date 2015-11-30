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
#include "chats.h"
#include "leak_detector_c.h"


void print_chats(chat_list *list);

int main (int argc, char **argv) {

	friends *friends;	
	chats *chats;

	chat_info *aux_chat;
	friend_info *aux_friend;
	friend_info *aux_friend_list[5];
	char *friend_names[5];

	atexit(report_mem_leak);

	// init lists
	friends = fri_new();
	chats = cha_new();

	// adding friends to friend list
	fri_add_friend(friends, "Juanito", "Es mi amigo de toda la vida...");
	fri_add_friend(friends, "pepito", "Es mi Enemigoo de toda la vida...");
	fri_add_friend(friends, "Manolito", "Es Culero de toda la vida...");
	fri_add_friend(friends, "Qewdqew", "Es ...");

	printf("= Printing empty list =====\n");
	print_chats(chats);
	

	aux_friend_list[0] = fri_find_friend(friends, "Juanito");
	aux_friend_list[1] = fri_find_friend(friends, "Manolito");
	aux_friend_list[2] = fri_find_friend(friends, "pepito");
	aux_friend_list[3] = fri_find_friend(friends, "Qewdqew");

	friend_names[0] = malloc(sizeof(char) + strlen("Juanito"));
	friend_names[1] = malloc(sizeof(char) + strlen("Manolito"));
	friend_names[2] = malloc(sizeof(char) + strlen("pepito"));
	friend_names[3] = malloc(sizeof(char) + strlen("Qewdqew"));

	strcpy(friend_names[0], "Juanito");
	strcpy(friend_names[1], "Manolito");
	strcpy(friend_names[2], "pepito");
	strcpy(friend_names[3], "Qewdqew");

	printf("= Adding chats =====\n");

	if( cha_add_chat(chats, 0, "Chat con mi colega Manolito", NULL, &aux_friend_list[1],"pepe", &friend_names[1], 1) == -1 ) {
		printf("Could not add chat\n");
	}
	if( cha_add_chat(chats, 1, "Chat con mi colega pepito", NULL, &aux_friend_list[2],"pepe", &friend_names[2], 1) == -1 ) {
		printf("Could not add chat\n");
	}
	if( cha_add_chat(chats, 2, "Chat con mis colegas Juanito y Manolito", aux_friend_list[0],  &aux_friend_list[1], friend_names[0], &friend_names[1], 1) == -1 ) {
		printf("Could not add chat\n");
	}
	print_chats(chats);

	free(friend_names[0]);
	free(friend_names[1]);
	free(friend_names[2]);
	free(friend_names[3]);
	
	cha_free(chats);
	fri_free(friends);
	


}

void print_chats(chat_list *list) {
	printf("-------------------------------\n");
	cha_print_chat_list(list);
	printf("-------------------------------\n");
}
