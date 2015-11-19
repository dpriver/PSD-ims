/*******************************************************************************
 *	messages.c
 *
 *  Message list
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

#include "messages.h"
#include "bool.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef DEBUG
#include "leak_detector_c.h"
#endif


int _mes_del_message(messages *messages, int id) {
	int i;

	if( (id < 0) || (id > messages->n_messages-1) ) {
		return -1;
	}

	for(i = id ; i < messages->n_messages-1 ; i++) {
		messages->list[i] = messages->list[i+1];
	}
	messages->n_messages--;

	if( (messages->lenght - messages->n_messages) > 5 ) {
		if ( (messages->list = realloc(messages->list, sizeof(message_info)*(messages->lenght - 5)) ) == NULL ) {
			return -1;
		}
	}
	messages->lenght -= 5;
}


/* =========================================================================
 *  Message struct API
 * =========================================================================*/

/*
 * Allocates a new message list
 * Returns a pointer to the list or NULL if fails
 */
messages *mes_new() {
	messages *messages_new;
	if( ( messages_new = malloc(sizeof(messages)) ) == NULL) {
		return NULL;
	}
	if( (messages_new->list = malloc(sizeof(message_info)*3) ) == NULL) {
		free(messages_new);
		return NULL;
	}

	messages_new->n_messages = 0;
	messages_new->lenght = 3;

	return messages_new;
}


/*
 * Frees the message list
 */
void mes_free(messages *messages) {
	free(messages->list);
	free(messages);
}


/*
 * Prints all messages line by line
 */
void mes_print_message_list(messages *messages) {
	int i;
	for( i = 0; i < messages->n_messages ; i++) {
		printf("%s: %s \n", mes_GET_SENDER(messages->list[i]), mes_GET_TEXT(messages->list[i]));
	}
}


/*
 * Creates a new message in the list with the provided info
 * Returns 0 or -1 if fails
 */
int mes_add_message(messages *messages, const char *sender, const char *text, int send_date, const char *attach_path) {

	if( messages->lenght == 0 ) {
		if( (messages->list = malloc(sizeof(message_list)*3) ) == NULL) {
			return -1;
		}
		messages->lenght += 3;
	}

	if( messages->lenght = messages->n_messages  ) {
		if( (messages->list = realloc(messages->list, sizeof(message_list)*(messages->lenght+5))) == NULL ) {
			return -1;
		}
		messages->lenght += 5;
	}
	
	if( (messages->list[messages->n_messages].sender = malloc( sizeof(strlen(sender) + sizeof(char)) ) ) == NULL) {
		return -1;
	}
	strcpy(messages->list[messages->n_messages].sender, sender);
	if( (messages->list[messages->n_messages].text = malloc( sizeof(strlen(text) + sizeof(char)) ) ) == NULL) {
		return -1;
	}
	strcpy(messages->list[messages->n_messages].text, text);
	if( attach_path != NULL ) {
		if( (messages->list[messages->n_messages].attach_path = malloc( sizeof(strlen(attach_path) + sizeof(char)) ) ) == NULL) {
			return -1;
		}
		strcpy(messages->list[messages->n_messages].attach_path, attach_path);
	}
	else {
		messages->list[messages->n_messages].attach_path = NULL;
	}
	messages->list[messages->n_messages].send_date = send_date;

	return 0;
}


/*
 * Removes the last "n_messages" messages
 * Returns 0 or -1 if "id" does not exist in the list
 */
int mes_del_last_messages(messages *messages, int n_messages) {
	int i;

	if( (n_messages > messages->n_messages) ) {
		n_messages = messages->n_messages;
	}

	messages->n_messages -= n_messages;

	if( (messages->lenght - messages->n_messages) > 5 ) {
		if ( (messages->list = realloc(messages->list, sizeof(message_info)*(messages->lenght - 5)) ) == NULL ) {
			return -1;
		}
	}
	messages->lenght -= 5;
}

