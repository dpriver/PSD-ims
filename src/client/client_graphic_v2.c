/*******************************************************************************
 *  client_graphic_v2.c
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
#include <signal.h>
#include <pthread.h>
#include <ctype.h>
#include "psd_ims_client.h"
#include "bool.h"

#include "debug_def.h"

#define MAX_USER_NAME_CHARS 20
#define MAX_USER_PASS_CHARS 20
#define MAX_USER_INFO_CHARS 100
#define MAX_DESCRIPTION_CHARS 100
#define MAX_MESSAGE_CHARS 300
#define MAX_INPUT_CHARS 100

#define COMMAND_CHAR '/'

#define FLUSH_INPUT(ch) \
	while ( ((ch = getchar()) != '\n') && (ch != EOF) )


int scan_input_string(char *buffer, int max_chars) {
	char aux_char;
	int index = 0;
	
	while( (index < max_chars) && ((aux_char = getchar()) != '\n') ) {
		if (aux_char == EOF)
			return -1;
		buffer[index++] = aux_char;
	}
	buffer[index] = '\0';
	if (index >= max_chars) FLUSH_INPUT(aux_char);
	
	return 0;
	
}
	

boolean is_numeric(char *str) {
  while(*str)
  {
    if(!isdigit(*str))
      return false;
    str++;
  }
  return true;
}


typedef enum {DEFAULT, EXIT, LOGIN, USER_MAIN, USER_FRIENDS} menu_type;

boolean continue_fetching;
pthread_mutex_t continue_fetching_mutex;
boolean continue_graphic;
pthread_mutex_t continue_graphic_mutex;

pthread_t notifications_tid = -1;

char input_buffer[MAX_INPUT_CHARS];

void retrieve_user_data(psd_ims_client *client);
void stop_client(int sig);
void *notifications_fetch(void *arg);

int run_notifications_thread(psd_ims_client *client);
int configure_signal_handling();




void menu_header_show(const char *string) {
	write(1,"\E[H\E[2J",7);
	printf("=============================================\n");
	printf("              %s\n", string);
	printf("=============================================\n");
}

void menu_footer_show() {
	printf("\n -----------------------------------------------\n");
	printf(" -> ");
}

int get_user_input(char input[], int max_chars) {
	scan_input_string(input, max_chars);
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
 *  Friend requests Menu
 * =========================================================================*/


int accept_friend_req(psd_ims_client *client, char *name) {
	if( psd_send_request_accept(client, name) != 0 ) {
		printf(" Could not accept the friend request\n");
		wait_user();
		return -1;
	}

	printf(" User '%s' accepted as friend\n", name);
	wait_user();
}


int decline_friend_req(psd_ims_client *client, char *name) {
	if( psd_send_request_decline(client, name) != 0 ) {
		printf(" Could not reject the friend request\n");
		wait_user();
		return -1;
	}

	printf(" User '%s' rejected as friend\n", name);
	wait_user();
}


void screen_friend_req_show(psd_ims_client *client) {
	menu_header_show("PSD IMS - Friend requests");
	printf(" (/e)exit, (/a<name>)accept <name>, (/d<name>)decline <name>\n");
	printf(" --------------------------------------------\n");
	psd_print_friend_requests(client);
	menu_footer_show();
}


void menu_friend_req(psd_ims_client *client) {
	boolean exit = false;
	boolean cont;

	do {
		screen_friend_req_show(client);
		get_user_input(input_buffer, MAX_INPUT_CHARS);
		if (input_buffer[0] == COMMAND_CHAR) {
			switch(input_buffer[1]) {
				case 'e':
					exit = true;
					break;
				case 'a':
					accept_friend_req(client, &input_buffer[2]);
					break;
				case 'd':
					decline_friend_req(client, &input_buffer[2]);
					break;
			}
		} 
		
		pthread_mutex_lock(&continue_graphic_mutex);
		cont = !exit && continue_graphic;
		pthread_mutex_unlock(&continue_graphic_mutex);
		
	} while( cont );
	
	psd_logout(client);
}


/* =========================================================================
 *  Chat Menu
 * =========================================================================*/

int add_member_to_chat(psd_ims_client *client, int chat_id) {
	char member[MAX_USER_NAME_CHARS];

	printf("\n\n = Friends =\n");
	psd_print_friends(client);
	printf("\n member: ");
	scan_input_string(member, MAX_USER_NAME_CHARS);

	if( psd_add_member_to_chat(client, member, chat_id) != 0 ) {
		printf(" FAIL: Could not add %s to the chat\n", member);
		wait_user();
		return -1;
	}
	return 0;
}


int delete_member_from_chat(psd_ims_client *client, int chat_id) {
	printf("Not implemented\n");
	wait_user();
	return -1;	
}


int leave_chat(psd_ims_client *client, int chat_id) {
	if( psd_quit_from_chat(client, chat_id) != 0 ) {
		printf(" Failed to leave the chat\n");
		wait_user();
		return -1;
	}

	return 0;
}


int attach_file(char *file_path, char *file_info, int max_info_chars) {
	FILE *fd;
	
	// An ugly way to see if a file exists...
	if( (fd = fopen(file_path, "r")) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not read the file");
		return -1;
	}
	fclose(fd);
		
	printf("\n file info: ");
	scan_input_string(file_info, max_info_chars);
	return 0;
}


int send_message(psd_ims_client *client, char *text, int chat_id, char *file_path, char *MIME_type, char *file_info) {

	if( psd_send_message(client, chat_id, text, file_path, MIME_type, file_info) != 0 ) {
		printf(" Failed to send the message\n");
		wait_user();
		return -1;
	}

	return 0;
}


void screen_chat_show(psd_ims_client *client, int chat_id, boolean has_attach, char *file_path) {
	menu_header_show("PSD IMS - Chats");
	printf(" (/e)exit, (/a)add member, (/d)delete member, (/l)leave\n");
	printf(" (/t)<file> attach <file> to message, (/r)remove attach\n");
	printf(" --------------------------------------------\n");
	psd_print_chat_messages(client, chat_id);
	if(has_attach)
		printf(" attached file: %s", file_path);
	menu_footer_show();
}


void menu_chat(psd_ims_client *client, int chat_id) {
	int ret = -1;
	boolean exit = false;
	boolean cont;
	boolean has_attach = false;

	char file_path[MAX_INPUT_CHARS];
	char file_type[] = "";
	char file_info[MAX_DESCRIPTION_CHARS];

	do {
		screen_chat_show(client, chat_id, has_attach, file_path);
		ret = get_user_input(input_buffer, MAX_INPUT_CHARS);
		if (input_buffer[0] == COMMAND_CHAR) {
			switch(input_buffer[1]) {
				case 'e':
					exit = true;
					break;
				case 'a':
					add_member_to_chat(client, chat_id);
					break;
				case 'd':
					delete_member_from_chat(client, chat_id);
					break;
				case 'l':
					leave_chat(client, chat_id);
					break;
				case 't':
					if (attach_file( &input_buffer[2], file_info, MAX_DESCRIPTION_CHARS) == 0) {
						has_attach = true;
					}
					break;
				case 'r':
					has_attach = false;
					break;
			}
		} 
		else {
			if (has_attach) 
				send_message(client, input_buffer, chat_id, file_path, file_type, file_info);
			else 
				send_message(client, input_buffer, chat_id, NULL, NULL, NULL);
			has_attach = false;
		}
		
		pthread_mutex_lock(&continue_graphic_mutex);
		cont = !exit && continue_graphic;
		pthread_mutex_unlock(&continue_graphic_mutex);
		
	} while( cont );
}


/* =========================================================================
 *  Main Menu
 * =========================================================================*/

int create_new_chat(psd_ims_client *client) {
	char description[MAX_DESCRIPTION_CHARS];
	char member[MAX_USER_NAME_CHARS];
	int chat_id;

	printf(" Friends\n");
	psd_print_friends(client);

	printf("\n member: ");
	scan_input_string(member, MAX_USER_NAME_CHARS);
	printf("\n description: ");
	scan_input_string(description, MAX_DESCRIPTION_CHARS);

	if( (chat_id = psd_create_chat(client, description, NULL)) < 0 ) {
		printf(" FAIL: Could not create the chat\n");
		wait_user();
		return -1;
	}

	if( psd_add_member_to_chat(client, member, chat_id) != 0 ) {
		psd_quit_from_chat(client, chat_id);
		printf(" Failed to create the chat\n");
		wait_user();
		return -1;
	}

	printf(" Chat with %s created\n", member);
	return 0;
}


int send_friend_request(psd_ims_client *client) {
	char name[MAX_USER_NAME_CHARS];

	printf("\n\n User name: ");
	scan_input_string(name, MAX_USER_NAME_CHARS);

	if( psd_send_friend_request(client, name) != 0 ) {
		printf(" FAIL: Could not request friendship to '%s'\n", name);
		wait_user();
		return -1;
	}

	printf(" Sended a friend request to '%s'\n", name);
	wait_user();

	return 0;
}


void screen_main_show(psd_ims_client *client) {
	menu_header_show("PSD IMS - Chats");
	printf(" (/e)exit, (/l)logout, (/n)new chat (/f)send friend request\n");
	printf(" (/r)review friend requests\n");
	printf(" --------------------------------------------\n");
	psd_print_chats(client);
	menu_footer_show();
}


void menu_main(psd_ims_client *client, boolean *global_exit) {
	boolean exit = false;
	boolean cont;
	int ret;

	do {
		screen_main_show(client);
		ret = get_user_input(input_buffer, MAX_INPUT_CHARS);
		if (ret < 0) return;
		if (input_buffer[0] == COMMAND_CHAR) {
			switch(input_buffer[1]) {
				case 'e':
					exit = true;
					*global_exit = true;
					break;
				case 'l':
					exit = true;
					*global_exit = false;
					break;
				case 'n':
					create_new_chat(client);
					break;
				case 'f':
					send_friend_request(client);
					break;
				case 'r':
					menu_friend_req(client);
					break;
			}
		} 
		else if (is_numeric(input_buffer)) {
			menu_chat(client, atoi(input_buffer));
		}
		
		pthread_mutex_lock(&continue_graphic_mutex);
		cont = !exit && continue_graphic;
		pthread_mutex_unlock(&continue_graphic_mutex);
		
	} while( cont );
	
	psd_logout(client);
}



/* =========================================================================
 *  Login Menu
 * =========================================================================*/

int user_register(psd_ims_client *client) {
	char name[MAX_USER_NAME_CHARS];
	char pass[MAX_USER_PASS_CHARS];
	char info[MAX_USER_INFO_CHARS];
	int ret;

	printf("\n User name: ");
	ret = scan_input_string(name, MAX_USER_NAME_CHARS);
	if (ret < 0) return -1;
	printf(" User password: ");
	ret = scan_input_string(pass, MAX_USER_PASS_CHARS);
	if (ret < 0) return -1;
	printf(" Describe yourself: ");
	ret = scan_input_string(info, MAX_USER_INFO_CHARS);
	if (ret < 0) return -1;

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
	int ret;

	printf("\n User name: ");
	ret = scan_input_string(name, MAX_USER_NAME_CHARS);
	if (ret < 0) return -1;
	printf(" User password: ");
	ret = scan_input_string(pass, MAX_USER_PASS_CHARS);
	if (ret < 0) return -1;


	printf(" %s %s\n", name, pass);
	if ( psd_login(client, name, pass) != 0 ) {
		printf(" Login failed, maybe the credentials are incorrect or the conection is failing\n");
		wait_user();
		return -1;
	}

	printf(" Logged in as '%s'\n", name);
	printf(" Retrieving user data ...");	
	retrieve_user_data(client);

	printf(" Done\n");
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


void menu_login(psd_ims_client *client) {
	boolean exit = false;
	boolean cont;

	do {
		screen_login_show();
		get_user_input(input_buffer, MAX_INPUT_CHARS);
		switch(input_buffer[0]) {
			case '0': break;   // salir
			case '1':	
				user_register(client);
				break;
			case '2': 
				if( login(client) == 0) {
					configure_signal_handling();
					run_notifications_thread(client);
					menu_main(client, &exit);
				}
				break;
		}
		
		pthread_mutex_lock(&continue_graphic_mutex);
		cont = !exit && continue_graphic;
		pthread_mutex_unlock(&continue_graphic_mutex);
			
	} while( cont );
}


/* =========================================================================
 *  Main function
 * =========================================================================*/


int graphic_client_run(psd_ims_client *client) {

	continue_graphic = true;
	configure_signal_handling();
	menu_login(client);
	
	DEBUG_INFO_PRINTF("Freeing client resources");
	
}


void retrieve_user_data(psd_ims_client *client) {
	if ( psd_recv_friends(client) < 0 ) {
		printf(" Failed to retrieve new friends");
		wait_user();
		return;
	}

	if ( psd_recv_chats(client) < 0 ) {
		printf(" Failed to retrieve new chats");
		wait_user();
		return;
	}

	if ( psd_recv_all_messages(client) < 0 ) {
		printf(" Failed to retrieve the chats messages");
		wait_user();
		return;
	}
}


int configure_signal_handling() {
	if (signal(SIGINT, stop_client) == SIG_ERR) {
		DEBUG_FAILURE_PRINTF("Could not attach SIGINT handler");
		return -1;
	}
	if (signal(SIGTERM, stop_client) == SIG_ERR) {
		DEBUG_FAILURE_PRINTF("Could not attach SIGTERM handler");
		return -1;
	}
	if (signal(SIGABRT, stop_client) == SIG_ERR) {
		DEBUG_FAILURE_PRINTF("Could not attach SIGABRT handler");
		return -1;
	}
}

int run_notifications_thread(psd_ims_client *client) {
	// start the thread to get notifications
	if (pthread_create(&notifications_tid, NULL, &notifications_fetch, client) != 0 ) {
		printf("Could not create the notifications thread\n");
		return 0;
	}
}

void *notifications_fetch(void *arg) {
	psd_ims_client *client;
	sigset_t sig_blocked_mask;
	sigset_t old_sig_mask;

	client = (psd_ims_client *)arg;

	continue_fetching = TRUE;
	pthread_mutex_init(&continue_fetching_mutex, NULL);

	while(1) {
		sleep(1);
		psd_recv_notifications(client);

		pthread_mutex_lock(&continue_fetching_mutex);
		if( !continue_fetching ) {
			pthread_mutex_unlock(&continue_fetching_mutex);
			DEBUG_INFO_PRINTF("Ending notifications service...");
			break;
		}
		pthread_mutex_unlock(&continue_fetching_mutex);
	}
}


void stop_client(int sig) {

	pthread_mutex_lock(&continue_fetching_mutex);
	continue_fetching = FALSE;
	pthread_mutex_unlock(&continue_fetching_mutex);

	// wait for the thread to end
	if( notifications_tid != -1) {
		pthread_join(notifications_tid, NULL);
	}

	// Now the notification service is not running
	
	// Save the state
	// To save the state, I should be sure that the system is stable
	// but the process could be accesing a structure when the signal was thrown...
	pthread_mutex_lock(&continue_graphic_mutex);
	continue_graphic = FALSE;
	pthread_mutex_unlock(&continue_graphic_mutex);
	fclose(stdin);

}
