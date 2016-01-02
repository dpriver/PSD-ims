/*******************************************************************************
 *	messages.h
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

#ifndef __MESSAGES
#define __MESSAGES

#include "bool.h"
#include "list.h"


typedef struct message_info message_info;
struct message_info{
	char *sender;
	char *text;
	boolean has_attach;
	char *attach_path;
	int timestamp;
};

typedef struct message_list_info message_list_info;
struct message_list_info {
	int timestamp;
};

typedef list messages;
typedef list_iterator mes_iterator;


/* =========================================================================
 *  Structs access macros
 * =========================================================================*/

#define mes_list_full(messages) \
		list_full(messages)

#define mes_list_gaps(messages) \
		list_gaps(messages)
	
#define mes_max_elems(messages) \
		list_max_elems(messages)

#define mes_sender(message_info) \
		(message_info->sender)

#define mes_text(message_info) \
		(message_info->text)

#define mes_attach_path(message_info) \
		(message_info->attach_path)

#define mes_message_timestamp(message_info) \
		(message_info->timestamp)


#define mes_get_num_messages(messages) \
		list_num_elems(messages)	

#define mes_get_timestamp(messages, messages_timestamp) \
	do{ \
		message_list_info *aux; \
		aux = list_info(messages); \
		messages_timestamp = aux->timestamp; \
	}while(0)

#define mes_set_timestamp(messages, messages_timestamp) \
	do{ \
		message_list_info *aux; \
		aux = list_info(messages); \
		aux->timestamp = messages_timestamp; \
	}while(0)



/* =========================================================================
 *  Messages iterator
 * =========================================================================*/
#define mes_get_messages_iterator(list) \
		(mes_iterator*)list_iterator(list)

#define mes_iterator_next(list, iterator) \
		(mes_iterator*)list_iterator_next(list, iterator)

#define mes_get_info(iterator) \
		(message_info*)list_iterator_info(iterator)


/* =========================================================================
 *  Message struct API
 * =========================================================================*/

/*
 * Allocates a new message list
 * Returns a pointer to the list or NULL if fails
 */
messages *mes_new(int max);

/*
 * Frees the message list
 */
void mes_free(messages *messages);

/*
 * Prints all messages line by line
 */
void mes_print_message_list(messages *messages);

/*
 * Creates a new message in the list with the provided info
 * Returns 0 or -1 if fails
 */
int mes_add_message(messages *messages, const char *sender, const char *text, int send_timestamp, const char *attach_path);

/*
 * Removes the first "n_messages" messages
 * Returns 0 or -1 if "id" does not exist in the list
 */
void mes_del_first_messages(messages *messages, int n_messages);

/*
 * Removes the last "n_messages" messages
 * Returns 0 or -1 if "id" does not exist in the list
 */
int mes_del_last_messages(messages *messages, int n_messages);

/*
 * Searches the message with the provided timestamp
 * Returns a pointer to the message_info or NULL if fails
 */
message_info *mes_find_message(messages *messages, int send_timestamp);

#endif /* __MESSAGES */
