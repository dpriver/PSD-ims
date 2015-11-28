/*******************************************************************************
 *	client_graphic.c
 *
 *  Graphic (console) interface for the client
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


#include <stdio.h>
#include <stdlib.h>
#include "psd_ims_client.h"

#define MAX_USER_NAME_CHARS 20
#define MAX_USER_PASS_CHARS 20
#define MAX_USER_INFO_CHARS 100
#define MAX_MESSAGE_CHARS 300

#define SCAN_INPUT_STRING(string, index, max_chars) \
	index = 0; \
	while( (index < max_chars) && (string[index] = getchar()) != '\n' ); \
	string[index] = '\0'


#define FLUSH_INPUT(ch) \
	while ( ((ch = getchar()) != '\n') && (ch != EOF) )

typedef enum {DEFAULT, EXIT, LOGIN, USER_MAIN, USER_LIST, USER_SEND, USER_RECEIVE} menu_type;



void menu_header_show(const char *string) {
	write(1,"\E[H\E[2J",7);
	printf("=============================================\n");
	printf("              %s\n", string);
	printf("=============================================\n");
}

void menu_footer_show() {
	printf("\n");
	printf(" Opcion -> ");
}

int get_user_input() {
	char input;
	char aux_char;
	input = getchar();
	FLUSH_INPUT(aux_char);

	return ( (input >= '0') && (input <= '9'))?((int)input - '0'):(-1);
}

void wait_user() {
	char aux_char;
	printf("\n Press ENTER to continue...");
	FLUSH_INPUT(aux_char);
}

void save_state(psd_ims_client *client) {
	printf(" = Saving state =\n");
	printf(" (NOT implemented)\n");
}


/* =========================================================================
 *  User receive Menu
 * =========================================================================*/
 
int recv_notifications(psd_ims_client *client) {
	printf("\n\n Retrieving notifications...\n");
	if (psd_recv_notifications(client) != 0 ) {
		printf(" Failed to retrieve the notifications\n");
		wait_user();
		return -1;	
	}

	printf(" Notifications obtained from server\n");
	wait_user();
	return 0;
}

int recv_pending_messages(psd_ims_client *client) {
	//printf("\n\n Retrieving pending messages...\n");
	printf(" (NOT implemented)\n");
	wait_user();
	return 0;
}

int recv_new_chats(psd_ims_client *client) {
	//printf("\n\n Retrieving new chats...\n");
	printf(" (NOT implemented)\n");
	wait_user();
	return 0;
}


void screen_menu_recv_show() {
	menu_header_show("PSD IMS - Receive menu");
	printf(" 1. Recibir notificaciones pendientes\n");
	printf(" 2. Recibir mensajes pendientes\n");
	printf(" 3. Recibir nuevos chats\n");
	printf("\n 0. Salir\n");
	menu_footer_show();
}

int menu_recv(psd_ims_client *client, menu_type *next_menu_ret) {
	int option = -1;
	menu_type default_next_menu = USER_MAIN;
	menu_type next_menu = default_next_menu;

	do {
		screen_menu_recv_show();
		option = get_user_input() ;
		switch(option) {
			case 0: break;   // salir
			case 1:	// go to listing menu
				recv_notifications(client);
				break;
			case 2: // go to send menu
				recv_pending_messages(client);
				break;
			case 3: // go to receive menu
				recv_new_chats(client);
				break;
		}		
	} while( option != 0 );
	
	next_menu_ret = DEFAULT;

	return next_menu;
}


/* =========================================================================
 *  User send Menu
 * =========================================================================*/

int send_message(psd_ims_client *client) {
	char text[MAX_MESSAGE_CHARS];
	char aux_char;
	int chat_id;
	int i;

	printf("\n\n = Chats =\n");
	psd_print_chats(client);

	printf("\n Chat id: ");
	scanf("%d", &chat_id);
	FLUSH_INPUT(aux_char);
	printf("\n Text: ");
	SCAN_INPUT_STRING(text, i, MAX_MESSAGE_CHARS);

	if( psd_send_message(client, chat_id, text, NULL) != 0 ) {
		printf(" Failed to send the message\n");
		wait_user();
		return -1;
	}

	printf(" Message sended\n");
	wait_user();

	return 0;
}

int send_friend_request(psd_ims_client *client) {
	char name[MAX_USER_NAME_CHARS];
	char aux_char;

	printf("\n\n User name: ");
	scanf("%s", name);
	FLUSH_INPUT(aux_char);

	if( psd_send_friend_request(client, name) != 0 ) {
		printf(" Failed the request friendship to '%s'\n", name);
		wait_user();
		return -1;
	}

	printf(" Sended a friend request to '%s'\n", name);
	wait_user();

	return 0;
}

int send_request_accept(psd_ims_client *client) {
	char name[MAX_USER_NAME_CHARS];
	char aux_char;

	printf("\n\n = Friend requests =\n");
	psd_print_friend_requests(client);
	printf("\n User name: ");
	scanf("%s", name);
	FLUSH_INPUT(aux_char);	

	if( psd_send_request_accept(client, name) != 0 ) {
		printf(" Could not accept the friend request\n");
		wait_user();
		return -1;
	}

	printf(" Friend '%s' accepted\n", name);
	wait_user();
	return 0;
}

int send_request_decline(psd_ims_client *client) {
	char name[MAX_USER_NAME_CHARS];
	char aux_char;

	printf("\n\n = Friend requests =\n");
	psd_print_friend_requests(client);
	printf("\n User name: ");
	scanf("%s", name);
	FLUSH_INPUT(aux_char);

	if( psd_send_request_decline(client, name) != 0 ) {
		printf(" Could not accept the friend request\n");
		wait_user();
		return -1;
	}

	printf(" User '%s' rejected as friend\n", name);
	wait_user();

	return 0;
}


void screen_menu_send_show() {
	menu_header_show("PSD IMS - Send menu");
	printf(" 1. Enviar mensaje\n");
	printf(" 2. Enviar peticion de amistad\n");
	printf(" 3. Aceptar peticion de amistad\n");
	printf(" 4. Rechazar peticion de amistad\n");
	printf("\n 0. Salir\n");
	menu_footer_show();
}

int menu_send(psd_ims_client *client, menu_type *next_menu_ret) {
	int option = -1;
	menu_type default_next_menu = USER_MAIN;
	menu_type next_menu = default_next_menu;

	do {
		screen_menu_send_show();
		option = get_user_input() ;
		switch(option) {
			case 0: break;   // salir
			case 1:	// go to listing menu
				send_message(client);
				break;
			case 2: // go to send menu
				send_friend_request(client);
				break;
			case 3: // go to receive menu
				send_request_accept(client);
				break;
			case 4:
				send_request_decline(client);
				break;
		}		
	} while( option != 0 );
	
	next_menu_ret = DEFAULT;

	return next_menu;
}


/* =========================================================================
 *  User list Menu
 * =========================================================================*/

void list_friends(psd_ims_client *client) {
	printf("\n\n = Friends =\n");
	psd_print_friends(client);
	wait_user();
}

void list_chats(psd_ims_client *client) {
	printf("\n\n = Chats =\n");
	psd_print_chats(client);
	wait_user();
}

void list_chat_members(psd_ims_client *client) {
	int chat_id;
	char aux_char;

	printf("\n\n = Chats =\n");
	psd_print_chats(client);
	printf("\n Select a chat id: ");
	scanf("%d", &chat_id);
	FLUSH_INPUT(aux_char);
	psd_print_chat_members(client, chat_id);
	wait_user();
}

void list_pending_notification(psd_ims_client *client) {
	printf("\n\n = Pending notifications =\n");
	printf(" No notifications...(NOT implemented)\n");
	wait_user();
}

void list_friend_requests(psd_ims_client *client) {
	printf("\n\n = Friend requests =\n");
	psd_print_friend_requests(client);
	wait_user();
}

void screen_menu_list_show() {
	menu_header_show("PSD IMS - List menu");
	printf(" 1. Listar amigos\n");
	printf(" 2. Listar chats\n");
	printf(" 3. Listar miembros de un chat\n");
	printf(" 4. Listar notificaciones pendientes\n");
	printf(" 5. Listar peticiones de amistad\n");
	printf("\n 0. Salir\n");
	menu_footer_show();
}

int menu_list(psd_ims_client *client, menu_type *next_menu_ret) {
	int option = -1;
	menu_type default_next_menu = USER_MAIN;
	menu_type next_menu = default_next_menu;

	do {
		screen_menu_list_show();
		option = get_user_input() ;
		switch(option) {
			case 0: break;   // salir
			case 1:	// go to listing menu
				list_friends(client);
				break;
			case 2: // go to send menu
				list_chats(client);
				break;
			case 3: // go to receive menu
				list_chat_members(client);
				break;
			case 4:
				list_pending_notification(client);
				break;
			case 5:
				list_friend_requests(client);
				break;
		}		
	} while( option != 0 );
	
	next_menu_ret = DEFAULT;

	return next_menu;
}


/* =========================================================================
 *  User Menu
 * =========================================================================*/


void screen_menu_main_show() {
	menu_header_show("PSD IMS - Main user menu");
	printf(" 1. Listar\n");
	printf(" 2. Enviar\n");
	printf(" 3. Recibir\n");
	printf(" 4. logout\n");
	printf("\n 0. Salir de la aplicacion\n");
	menu_footer_show();
}

int menu_user(psd_ims_client *client, menu_type *next_menu_ret) {
	int option = -1;
	menu_type default_next_menu = EXIT;
	menu_type next_menu = default_next_menu;

	do {
		screen_menu_main_show();
		option = get_user_input() ;
		switch(option) {
			case 0: break;   // salir
			case 1:	// go to listing menu
				next_menu = USER_LIST;
				option = 0;
				break;
			case 2: // go to send menu
				next_menu = USER_SEND;
				option = 0; 
				break;
			case 3: // go to receive menu
				next_menu = USER_RECEIVE;
				option = 0;
				break;
			case 4: // go to login menu
				save_state(client);
				next_menu = LOGIN;
				option = 0;
				break;
		}		
	} while( option != 0 );
	
	next_menu_ret = DEFAULT;

	return next_menu;
}


/* =========================================================================
 *  Login Menu
 * =========================================================================*/

int user_register(psd_ims_client *client) {
	char name[MAX_USER_NAME_CHARS];
	char pass[MAX_USER_PASS_CHARS];
	char info[MAX_USER_INFO_CHARS];
	int i = 0;
	char aux_char;

	printf("\n\n User name: ");
	scanf("%s", name);
	FLUSH_INPUT(aux_char);
	printf(" User password: ");
	scanf("%s", pass);
	FLUSH_INPUT(aux_char);
	printf(" Describe yourself: ");
	SCAN_INPUT_STRING(info, i, MAX_USER_INFO_CHARS);

	if ( psd_user_register(client, name, pass, info) != 0 ) {
		printf(" Register failed, maybe the name is already registered or the conection is failing\n");
		wait_user();
		return -1;
	}

	printf(" User '%s' correctly registered\n", name);
	wait_user();

	return 0;
}

int login(psd_ims_client *client) {
	char name[MAX_USER_NAME_CHARS];
	char pass[MAX_USER_PASS_CHARS];
	char aux_char;

	printf("\n\n User name: ");
	scanf("%s", name);
	FLUSH_INPUT(aux_char);
	printf(" User password: ");
	scanf("%s", pass);
	FLUSH_INPUT(aux_char);

	if ( psd_login(client, name, pass) != 0 ) {
		printf(" Login failed, maybe the credentials are incorrect or the conection is failing\n");
		wait_user();
		return -1;
	}

	printf(" Logged in as '%s'\n", name);
	wait_user();

	return 0;
}


void screen_login_show() {
	menu_header_show("PSD IMS - Login");
	printf(" 1. Alta\n");
	printf(" 2. Login\n");
	printf("\n 0. Salir\n");
	menu_footer_show();
}

int menu_login(psd_ims_client *client, menu_type *next_menu_ret) {
	int option = -1;
	menu_type default_next_menu = EXIT;
	menu_type next_menu = default_next_menu;

	do {
		screen_login_show();
		option = get_user_input() ;
		switch(option) {
			case 0: break;   // salir
			case 1:	
				user_register(client);
				break;
			case 2: 
				if( login(client) == 0) {
					next_menu = USER_MAIN;
					option = 0;
				}
				break;
		}		
	} while( option != 0 );
	
	next_menu_ret = DEFAULT;
	return next_menu;
}


/* =========================================================================
 *  Main function
 * =========================================================================*/



int graphic_client_run(psd_ims_client *client) {
	menu_type ret_menu = DEFAULT;
	menu_type next_menu = DEFAULT;

	next_menu = (client != NULL)? USER_MAIN: LOGIN;

	do {	
		switch(next_menu) {
			case EXIT: break;
			case LOGIN:        next_menu = menu_login(client, &ret_menu); break;
			case USER_MAIN:    next_menu = menu_user(client, &ret_menu); break;
			case USER_LIST:    next_menu = menu_list(client, &ret_menu); break;
			case USER_SEND:    next_menu = menu_send(client, &ret_menu); break;
			case USER_RECEIVE: next_menu = menu_recv(client, &ret_menu); break;
		}
	} while(next_menu != EXIT);
}
