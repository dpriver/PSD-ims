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

#include "psd_ims_client.h"
#include "leak_detector_c.h"



void print_friends(psd_ims_client *client);
void print_chats(psd_ims_client *client);


int main( int argc, char **argv ) { 

	psd_ims_client *client;

	atexit(report_mem_leak);


	if ( (client = psd_new_client()) == NULL ) {
		printf("Could not create psd_client structure\n");		
		return -1;
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
	psd_del_friend(client, "Juanito");
	psd_del_friend(client, "qewd");
	psd_del_friend(client, "Pepe");

	print_friends(client);
 


	if ( psd_set_name(client, "Pepito") ) {
		printf("Could not set client username\n");
	}
	if ( psd_set_pass(client, "abcd1234") ) {
		printf("Could not set client password\n");
	}



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
	chats_print_list(client->chats);
	printf("----------------------------------------\n");
}

