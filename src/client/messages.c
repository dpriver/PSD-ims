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

#include "debug_def.h"

//#ifdef DEBUG
//#include "leak_detector_c.h"
//#endif


int _mes_del_message(messages *messages, int id) {
	DEBUG_TRACE_PRINT();
	int i;

	if( (id < 0) || (id > messages->n_messages-1) ) {
		return -1;
	}

	free(messages->list[id].sender);
	free(messages->list[id].text);
	if(messages->list[id].have_attach)
		free(messages->list[id].attach_path);

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
	DEBUG_TRACE_PRINT();
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
	DEBUG_TRACE_PRINT();
	free(messages->list);
	free(messages);
}


/*
 * Prints all messages line by line
 */
void mes_print_message_list(messages *messages) {
	DEBUG_TRACE_PRINT();
	int i;
	for( i = 0; i < messages->n_messages ; i++) {
		printf("[%d]%s: %s \n", mes_GET_SEND_DATE(messages->list[i]), (mes_GET_SENDER(messages->list[i]
) != NULL)? mes_GET_SENDER(messages->list[i]): "Yo" , mes_GET_TEXT(messages->list[i]));
	}
}


/*
 * Gets the last added message's send_date
 * Returns the send date or 0 if the list is empty
 */
int mes_get_last_message_date(messages *messages) {
	DEBUG_TRACE_PRINT();
	return messages->list[messages->n_messages-1].send_date;
}


/*
 * Creates a new message in the list with the provided info
 * Returns 0 or -1 if fails
 */
int mes_add_message(messages *messages, const char *sender, const char *text, int send_date, const char *attach_path) {
	DEBUG_TRACE_PRINT();

	//messages *aux_messages;

	if ( text == NULL ) {
		DEBUG_FAILURE_PRINTF("A message can not be added without 'text'");
		return -1;
	}
	if( messages->lenght == 0 ) {
		if( (messages->list = malloc(sizeof(message_info)*3) ) == NULL) {
			return -1;
		}
		messages->lenght += 3;
	}

	if( messages->lenght == messages->n_messages  ) {
		if( (messages->list = realloc(messages->list, sizeof(message_info)*(messages->lenght+5))) == NULL ) {
			return -1;
		}
		DEBUG_INFO_PRINTF("Made  %p", messages->list);
		messages->lenght += 5;
	}

	// Copy sender in a new string if not NULL
	if( sender != NULL) {
		if( (messages->list[messages->n_messages].sender = malloc( strlen(sender) + sizeof(char) ) ) == NULL) {
			return -1;
		}
		strcpy(messages->list[messages->n_messages].sender, sender);
	}
	else {
		messages->list[messages->n_messages].sender = NULL;
	}

	if (text != NULL) {
		if( (messages->list[messages->n_messages].text = malloc( strlen(text) + sizeof(char) ) ) == NULL) {
			return -1;
		}
		strcpy(messages->list[messages->n_messages].text, text);
		}
	else {
		messages->list[messages->n_messages].text = NULL;
	}

	if( attach_path != NULL ) {
		if( (messages->list[messages->n_messages].attach_path = malloc( strlen(attach_path) + sizeof(char) ) ) == NULL) {
			return -1;
		}
		strcpy(messages->list[messages->n_messages].attach_path, attach_path);
		messages->list[messages->n_messages].have_attach = TRUE;
	}
	else {
		messages->list[messages->n_messages].attach_path = NULL;
		messages->list[messages->n_messages].have_attach = FALSE;
	}
	messages->list[messages->n_messages].send_date = send_date;

	messages->n_messages++;
	
	return 0;
}


/*
 * Removes the last "n_messages" messages
 * Returns 0 or -1 if "id" does not exist in the list
 */
int mes_del_last_messages(messages *messages, int n_messages) {
	DEBUG_TRACE_PRINT();
	int i;

	if( (n_messages > messages->n_messages) ) {
		n_messages = messages->n_messages;
	}

	for( i = 1 ; i <= n_messages ; i++ ) {
		free(messages->list[ messages->n_messages - i ].sender);
		free(messages->list[ messages->n_messages - i ].text);
		if(messages->list[ messages->n_messages - i ].have_attach)
			free(messages->list[ messages->n_messages - i ].attach_path);
	}
	messages->n_messages -= n_messages;

	if( (messages->lenght - messages->n_messages) > 5 ) {
		if ( (messages->list = realloc(messages->list, sizeof(message_info)*(messages->lenght - 5)) ) == NULL ) {
			return -1;
		}
	}
	messages->lenght -= 5;
}

