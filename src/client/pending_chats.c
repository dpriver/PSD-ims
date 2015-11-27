/*******************************************************************************
 *	pending_chats.c
 *
 *  List to keep the chats pending to retrieve
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

#include "pending_chats.h"
#include <stdlib.h>

#include "debug_def.h"

#ifdef DEBUG
#include "leak_detector_c.h"
#endif

/*
 *
 *
 */
int pen_init(pending_chat_list *list) {
	list->chats = NULL;
	list->n_chats = 0;
	list->list_lenght = 0;

	return 0;
}


/*
 *
 *
 */
void pen_free(pending_chat_list *list) {
	free(list->chats);
}


/*
 *
 *
 */
int pen_lst_add(pending_chat_list *list, int chat_id) {

	if( list->chats == NULL) {
		if ( (list->chats = malloc(sizeof(pending_chat)*(list->list_lenght + 3)) ) == NULL ) {
			return -1;
		}
		list->list_lenght += 3;
	}
	else if(list->list_lenght = list->n_chats) {
		if ( (list->chats = realloc(list->chats, sizeof(pending_chat)*(list->list_lenght + 5)) ) == NULL ) {
			return -1;
		}
		list->list_lenght += 5;
	}

	DEBUG_INFO_PRINTF("Add chat %d to list", chat_id);
	list->chats[list->n_chats].chat_id = chat_id;
	list->n_chats++;

	return 0;
}


/*
 *
 *
 */
int pen_lst_del(pending_chat_list *list, int index) {
	int i;

	for(i = index ; index < list->n_chats-1 ; i++) {
		list->chats[i] = list->chats[i+1];
	}
	list->n_chats--;

	if( (list->list_lenght - list->n_chats) > 5 ) {
		if ( (list->chats = realloc(list->chats, sizeof(pending_chat)*(list->list_lenght - 5)) ) == NULL ) {
			return -1;
		}
	}
	list->list_lenght -= 5;

	return 0;
}

