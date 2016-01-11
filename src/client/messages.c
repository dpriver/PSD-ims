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
#include "list.h"
#include "bool.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "debug_def.h"

#ifdef DEBUG
#include "leak_detector_c.h"
#endif


#define sizeofstring(string) \
	(strlen(string) + sizeof(char))

void message_list_info_free(void *info) {
	free(info);
}

void message_free(void *message) {
	free(((message_info*)message)->sender);
	free(((message_info*)message)->text);
	free(((message_info*)message)->attach_path);
	free(message);

}

int message_timestamp_comp(const void *message, const void *timestamp) {
	return ((message_info*)message)->timestamp - *((int*)timestamp);
}

int message_comp(const void *message1, const void *message2) {
	return ((message_info*)message1)->timestamp - ((message_info*)message2)->timestamp;
}


/* =========================================================================
 *  Message struct API
 * =========================================================================*/

/*
 * Allocates a new message list
 * Returns a pointer to the list or NULL if fails
 */
messages *mes_new(int max) {
	DEBUG_TRACE_PRINT();
	messages *messages_new;
	message_list_info *list_info;

	if ( (list_info = malloc(sizeof(message_list_info))) == NULL ) {
		return NULL;
	}
	
	list_info->timestamp = 0;
	
	if ( (messages_new = list_new(list_info, max, message_list_info_free, message_free)) == NULL ) {
		list_info_free(list_info);
		return NULL; // could not allocate list
	}
	messages_new->item_value_comp = message_timestamp_comp;
	messages_new->item_comp = message_comp;

	return messages_new;
}


/*
 * Frees the message list
 */
void mes_free(messages *messages) {
	DEBUG_TRACE_PRINT();
	
	list_free(messages);
}


/*
 * Prints all messages line by line
 */
void mes_print_messages(messages *messages) {
	DEBUG_TRACE_PRINT();
}


/*
 * Creates a new message in the list with the provided info
 * Returns 0 or -1 if fails
 */
int mes_add_message(messages *messages, const char *sender, const char *text, int send_timestamp, const char *attach_path) {
	DEBUG_TRACE_PRINT();
	message_info *info;
	//messages *aux_messages;

	if ( text == NULL ) {
		DEBUG_FAILURE_PRINTF("A message can not be added without 'text'");
		return -1;
	}
	
	if ( (info = malloc(sizeof(message_info))) == NULL ) {
		return -1;
	}

	if (sender != NULL) {
		if ( (info->sender = malloc(sizeofstring(sender)) ) == NULL ) {	
			free(info);
			return -1;
		}
		strcpy(info->sender, sender);
	}
	else {
		info->sender = NULL;
	}
	if ( (info->text = malloc(sizeofstring(text)) ) == NULL ) {
		free(info->sender);
		free(info);
		return -1;
	}
	strcpy(info->text, text);
	
	if( attach_path != NULL ) {
		if ( (info->attach_path = malloc(sizeofstring(attach_path)) ) == NULL ) {	
			free(info->sender);
			free(info->text);
			free(info);
			return -1;
		}
		strcpy(info->attach_path, attach_path);
		info->has_attach = TRUE;
	}
	else {
		info->attach_path = NULL;
		info->has_attach = FALSE;
	}

	info->timestamp = send_timestamp;

	if ( list_add_item(messages, info) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not add message to list");
		message_free(info);
		return -1;
	}
	
	return 0;
}


void mes_del_first_messages(messages *messages, int n_messages) {
	DEBUG_TRACE_PRINT();
	
	list_delete_first(messages, n_messages);
}


/*
 * Removes the last "n_messages" messages
 * Returns 0 or -1 if fails
 */
int mes_del_last_messages(messages *messages, int n_messages) {
	DEBUG_TRACE_PRINT();
	
}


/*
 * Searches the message with the provided timestamp
 * Returns a pointer to the message_info or NULL if fails
 */
message_info *mes_find_message(messages *messages, int send_timestamp) {
	DEBUG_TRACE_PRINT();
	
	return (message_info*)list_find_item(messages, &send_timestamp);
}
