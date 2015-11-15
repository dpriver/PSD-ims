/*******************************************************************************
 *	chats.h
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

#define __CHATS

#ifndef __CHATS
#define __CHATS

#include "bool.h"
#include "friend_list.h"

typedef struct chat_member;
struct chat_member {
	char *name;
	
}

typedef struct chat_info chat_info;
struct chat_info {
	char *description;
	friend_info *admin;
	friend_info **members;
	int n_members;
};

typedef struct chat_node chat_node;
struct chat_node {
	chat_info *info;
	chat_node *next;
	chat_node *prev;
};


/*
 * Prints all chats line by line
 */
void print_chat_list(chat_node *list);


/*
 * Allocates a new chat list
 *
 * Returns a pointer to the list phantom node or NULL if fails
 */
chat_node *new_chat_list();


/*
 * Frees the chat list
 */
void free_chat_list(chat_node *list);


/*
 * Allocates a new chat_info struct with the provided data
 *
 * Returns a pointer to the structure or NULL if fails
 */
chat_info *new_chat_info(const char *description, friend_info *admin, friend_info **members
		int n_members);


chat_member_list *new_chat_member();


/*
 * Creates a new chat_node in the list with the provided info
 * "*info" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int add_chat(chat_node *list, chat_info *info);


/*
 * Removes and frees the first node that matches the provided "name"
 *
 * Returns 0 or -1 if "name" does not exist in the list
 */
int del_chat(friend_node *list, const char *name);


/*
 * Returns true of false whether "name" is in the list or not
 */
boolean is_chat_in_list(friend_node *list, char *name);

#endif /* __CHATS */
