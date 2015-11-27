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

#ifndef __PENDING_CHATS
#define __PENDING_CHATS

#include <stdlib.h>


typedef struct pending_chat pending_chat;
struct pending_chat {
	int chat_id;
};

typedef struct pending_chat_list pending_chat_list;
struct pending_chat_list {
	pending_chat *chats;
	int n_chats;
	int list_lenght;
};


/*
 *
 *
 */
int pen_init(pending_chat_list *list);

/*
 *
 *
 */
void pen_free(pending_chat_list *list);

/*
 *
 *
 */
int pen_lst_add(pending_chat_list *list, int chat_id);

/*
 *
 *
 */
int pen_lst_del(pending_chat_list *list, int index);

#endif /* __PENDING_CHATS */
