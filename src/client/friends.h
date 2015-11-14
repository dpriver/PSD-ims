/*******************************************************************************
 *	friends.h
 *
 *  Client friend list
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

#ifndef __FRIENDS
#define __FRIENDS

#include "bool.h"

typedef struct friend_info friend_info;
struct friend_info{
	char *name;
	char *information;
};

typedef struct friend_node friend_node;
struct friend_node {
	friend_info *info;
	friend_node *next;
	friend_node *prev;
};


/*
 * Prints all friends line by line
 */
void print_friend_list(friend_node *list);


/*
 * Allocates a new friend list
 *
 * Returns a pointer to the list phantom node or NULL if fails
 */
friend_node *new_friend_list();


/*
 * Frees the friend list
 */
void free_friend_list(friend_node *list);


/*
 * Allocates a new friend_info struct with the provided data
 *
 * Returns a pointer to the structure or NULL if fails
 */
friend_info *new_friend_info(const char *name, const char *information);


/*
 * Creates a new friend_node in the list with the provided info
 * "*info" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int add_friend(friend_node *list, friend_info *info);


/*
 * Removes and frees the first node that matches the provided "name"
 *
 * Returns 0 or -1 if "name" does not exist in the list
 */
int del_friend(friend_node *list, const char *name);


/*
 * Returns true of false whether "name" is in the list or not
 */
boolean is_friend(friend_node *list, char *name);

#endif /* __FRIENDS */
