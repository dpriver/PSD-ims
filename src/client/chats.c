/*******************************************************************************
 *	chats.c
 *
 *  Client chat list
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

#include "bool.h"
#include "list.h"
#include "chats.h"
#include "friends.h"
#include "messages.h"
#include "chat_members.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// The macros defined here will check for DEBUG definition
#include "debug_def.h"

#ifdef DEBUG
#include "leak_detector_c.h"
#endif


#define sizeofstring(string) \
	(strlen(string) + sizeof(char))

void list_info_free(void *info) {
	free(info);
}

void chat_free(void *chat) {
	free(((chat_info*)chat)->description);
	free(((chat_info*)chat)->admin);
	mes_free(((chat_info*)chat)->messages);
	member_free(((chat_info*)chat)->members);
	free(chat);
}

int chat_id_comp(const void *chat, const void *id) {
	return ((chat_info*)chat)->id - *(int*)id;
}

int chat_comp(const void *chat1, const void *chat2) {
	return ((chat_info*)chat1)->id - ((chat_info*)chat2)->id;
}


/* =========================================================================
 *  Chat struct API
 * =========================================================================*/

/*
 * Allocates a new chat list
 * Returns a pointer to the list or NULL if fails
 */
chats *cha_new(int max) {
	DEBUG_TRACE_PRINT();
	chats *chats_new;
	chat_list_info *list_info;
	
	if ( (list_info = malloc(sizeof(chat_list_info))) == NULL ) {
		return NULL;
	}
	
	list_info->timestamp = 0;
	
	if ( (chats_new = list_new(list_info, max, list_info_free, chat_free)) == NULL ) {
		list_info_free(list_info);
		return NULL;
	}
	
	
	chats_new->item_value_comp = chat_id_comp;
	chats_new->item_comp = chat_comp;
	
	return chats_new;
}


/*
 * Frees the chat list
 */
void cha_free(chats *chats) {
	DEBUG_TRACE_PRINT();

	list_free(chats);
}


/*
 * Creates a new chat in the list with the provided info
 * Returns 0 or -1 if fails
 */
int cha_add_chat(chats *chats, int chat_id, const char *description, char *admin, friend_info *members[], char *member_names[], int n_members, int max_members, int max_messages, int read_timestamp, int all_read_timestamp) {

	DEBUG_TRACE_PRINT();
	chat_info *info;
	int i;
	
	if ( (info = malloc(sizeof(chat_info))) == NULL ) {
		return -1;
	}

	if ( (info->description = malloc(sizeofstring(description))) == NULL ) {
		free(info);
		return -1;
	}
	if ( (info->admin = malloc(sizeofstring(admin))) == NULL ) {
		free(info);
		return -1;
	}
	
	if ( (info->members = members_new(max_members)) == NULL ) {
		free(info->description);
		free(info);
		return -1;
	}
	
	if ( (info->messages = mes_new(max_messages)) == NULL ) {
		members_free(info->members);
		free(info->description);
		free(info);
		return -1;
	}
	info->id = chat_id;
	info->read_timestamp = read_timestamp;
	info->all_read_timestamp = all_read_timestamp;
	info->unread_messages = 0;
	info->pending_messages = 0;
	strcpy(info->admin, admin);
	strcpy(info->description, description);

	for( i = 0; i < n_members ; i++ ) {
		if ( members_add_member(info->members, member_names[i], members[i]) != 0 ) {
			DEBUG_FAILURE_PRINTF("Could not add member");
			mes_free(info->messages);
			members_free(info->members);
			free(info->description);
			free(info);
			return -1;
		}
	}

	// add chat_info
	if( list_add_item(chats, info) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not add chat");
		chat_free(info);
		return -1; // can not add chat
	}

	return 0;
}


/*
 * Adds the message in the chat
 * Returns 0 or -1 if fails
 */
int cha_add_message(chat_info *chat, const char *sender, const char *text, int send_timestamp, const char *attach_path) {
	DEBUG_TRACE_PRINT();

	if( mes_list_full(chat->messages) ) {
		mes_del_first_messages(chat->messages, 1);
	}

	if( mes_add_message(chat->messages, sender, text, send_timestamp, attach_path) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not add the message");
		return -1;
	}

	if(send_timestamp > chat->read_timestamp){
		chat->unread_messages++;
	}

	return 0;
}


/*
 * Adds the messages in the chat
 * Returns 0 or -1 if fails
 */
int cha_add_messages(chat_info *chat, char *sender[], char *text[], int send_timestamp[], char *attach_path[], int n_messages) {
	DEBUG_TRACE_PRINT();
	
	int i = 0;
	int gaps_needed;

	if (n_messages > mes_max_elems(chat->messages)) {
		i = n_messages - mes_max_elems(chat->messages);
	}

	gaps_needed =  n_messages - i - mes_list_gaps(chat->messages);
	if (gaps_needed > 0) {
		mes_del_first_messages(chat->messages, gaps_needed);
	}

	for( i ; i< n_messages; i++ ) {
		if( mes_add_message(chat->messages, sender[i], text[i], send_timestamp[i], attach_path[i]) != 0 ) {
			DEBUG_FAILURE_PRINTF("Could not add the message");
			mes_del_last_messages(chat->messages, i); // (i+1) messages (-1) the last failed
			return -1;
		}
		if(send_timestamp[i] > chat->read_timestamp ){
			chat->unread_messages++;
		}
	}

	return 0;
}


/*
 * Creates a new chat member in the list with the provided info
 * Returns 0 or -1 if fails
 */
int cha_add_member(chat_info *chat, friend_info *member, const char *member_name) {
	DEBUG_TRACE_PRINT();

	if( members_add_member(chat->members, member_name, member) == -1 ) {
		DEBUG_FAILURE_PRINTF("Could not add member to chat");
		return -1; // can not add member to chat
	}

	return 0;
}


/*
 * Deletes chat member from the chat
 * Returns 0 or -1 if fails
 */
int cha_del_member(chat_info *chat, const char *member_name) {
	DEBUG_TRACE_PRINT();

	if( members_del_member(chat->members, member_name) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not remove member from chat");
		return -1;
	}

	return 0;
}


/*
 * Switches the current admin with the chat member named "name"
 * that means that the previous admin becomes a normal member
 * Returns 0 or -1 if fails
 */
int cha_change_admin(chat_info *chat, const char *member_name) {
	DEBUG_TRACE_PRINT();
	char *aux_admin;
	
	if( member_find_member(chat->members, member_name) == NULL ) {
		DEBUG_FAILURE_PRINTF("New admin is not a chat member");
		return -1;
	}
	
	if ( (aux_admin = malloc(sizeofstring(member_name))) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory for admin name");
		return -1;
	}
	strcpy(aux_admin, member_name);
	
	free(chat->admin);
	chat->admin = aux_admin;
	
	return 0;
}


/*
 * Deletes chat from the list
 * Returns 0 or -1 if fails
 */
int cha_del_chat(chats *chats, int chat_id) {
	DEBUG_TRACE_PRINT();
	list_node *node;
	node = list_find_node(chats, &chat_id);
	if (node == NULL) {
		DEBUG_FAILURE_PRINTF("Chat does not exists in list");
		return -1;
	}
	
	list_delete_node(chats, node);
	return 0;
}


chat_info *cha_find_chat(chats *chats, int chat_id) {
	DEBUG_TRACE_PRINT();
	return list_find_item(chats , &chat_id);
}
