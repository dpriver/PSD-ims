/*******************************************************************************
 *	test_psd_client_api.c
 *
 *  program to test the PSD-IMS client API
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

#include "psd_ims_client.h"
#include "leak_detector_c.h"


void print_friends(psd_ims_client *client);
void print_chats(psd_ims_client *client);


int main( int argc, char **argv ) { 

	psd_ims_client *client;
	char user_name[] = "Maguango";
	char user_pass[] = "abcd1234";

	char *friends[5] = {"Juanito", "Manolito", "Pepe", "Culero", "qewd"};
	char *aux_chat_members[5]; 

	atexit(report_mem_leak);

	if ( (client = psd_new_client()) == NULL ) {
		printf("Could not create psd_client structure\n");		
		return -1;
	}

	printf("= Setting cliend params =====\n");
	if ( psd_set_name(client, user_name) ) {
		printf("Could not set client username\n");
	}
	if ( psd_set_pass(client, user_pass) ) {
		printf("Could not set client password\n");
	}

	printf("= Printing empty lists ======\n");
	print_friends(client);
	print_chats(client);

	printf("= Adding some friends =====\n");
	psd_add_friend(client, "Juanito", "Es mi amigo Juanito");
	psd_add_friend(client, "Manolito", "Es mi amigo Manolito");
	psd_add_friend(client, "Pepe", "Es mi amigo Pepe");
	psd_add_friend(client, "Culero", "Es mi amigo Culero");
	psd_add_friend(client, "qewd", "Es mi amigo qewd");

	print_friends(client);


	printf("= Deleting some friends =====\n");
	psd_del_friend(client, "qewd");
	psd_del_friend(client, "Pepe");
	print_friends(client);
 

	printf("= Creating some chats =====\n");
	aux_chat_members[0] = friends[1];
	if ( psd_add_chat(client, 0, "Chat con mi colega Manolito", user_name, aux_chat_members, 1) ) {
		printf("Could not create chat 0\n");
	}
	else {
		printf("Created chat 0 with members:\n");
		cha_print_chat_members(client->chats, 0);
	}

	aux_chat_members[0] = friends[0];
	if ( psd_add_chat(client, 1, "Chat con mi colega Juanito", user_name, aux_chat_members, 1) ) {
		printf("Could not create chat 1\n");
	}
	else {
		printf("Created chat 1 with members:\n");
		cha_print_chat_members(client->chats, 1);
	}
	
	aux_chat_members[0] = friends[3];
	aux_chat_members[1] = friends[0];
	if ( psd_add_chat(client, 2, "Chat con mis colegas Culero y Juanito", user_name, aux_chat_members, 2) ) {
		printf("Could not create chat 2\n");
	}
	else {
		printf("Created chat 2 with members:\n");
		cha_print_chat_members(client->chats, 2);
	}

	print_chats(client);

	if ( psd_add_friend_to_chat(client, 1, "Culero") != 0) {
		printf("Could not add user to chat\n");
	}
	else {
		cha_print_chat_members(client->chats, 1);
	}
	if ( psd_del_friend_from_chat(client, 1, "Culero") != 0 ) {
		printf("Could not delete user from chat\n");
	}
	else {
		cha_print_chat_members(client->chats, 1);
	}

	if ( psd_del_chat(client, 1) != 0) {
		printf("Could not delete chat\n");
	}
	if ( psd_del_chat(client, 0) != 0) {
		printf("Could not delete chat\n");
	}
	print_chats(client);

	psd_free_client(client);

	return 0;
}



void print_friends(psd_ims_client *client) {
	printf(" -------- friends ----------------------\n");
	fri_print_friend_list(client->friends);
	printf("----------------------------------------\n");
}

void print_chats(psd_ims_client *client) {
	printf(" --------- chats -----------------------\n");
	cha_print_chat_list(client->chats);
	printf("----------------------------------------\n");
}

