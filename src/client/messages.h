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

typedef struct message_info message_info;
struct message_info{
	char *sender;
	int send_date;
	char *text;
	boolean have_attach;
	char *attach_path;
};

typedef struct message_list message_list;
struct message_list {
	message_info *list;
	int n_messages;
	int lenght;
};

typedef struct message_list messages;

/* =========================================================================
 *  Structs access macros
 * =========================================================================*/
#define mes_GET_N_MESSAGES(message_list) \
		message_list->n_messages

#define mes_GET_SENDER(message_info) \
		message_info.sender

#define mes_GET_TEXT(message_info) \
		message_info.text

#define mes_GET_SEND_DATE(message_info) \
		message_info.send_date

#define mes_SET_ATTACH_PATH(message_info) \
		message_info.attach_path



/* =========================================================================
 *  Message struct API
 * =========================================================================*/

/*
 * Allocates a new message list
 * Returns a pointer to the list or NULL if fails
 */
messages *mes_new();

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
int mes_add_message(messages *messages, const char *sender, const char *text, int send_date, const char *attach_path);

/*
 * Removes the last "n_messages" messages
 * Returns 0 or -1 if "id" does not exist in the list
 */
int mes_del_last_messages(messages *messages, int n_messages);


#endif /* __MESSAGES */
