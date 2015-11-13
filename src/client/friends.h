/*******************************************************************************
 *	friends.h
 *
 *  <ONE LINE DESCRIPTION.>
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


void print_friend_list(friend_node *list);

/*
 * Returns a pointer to the new created list if success
 * Returns NULL if fail
 */
friend_node* new_friend_list();

/*
 * Returns 0 if success 
 * Returns < 0 if error
 */
int free_friend_list(friend_node *list);

/*
 * Returns 0 if success 
 * Returns < 0 if error
 */
int add_friend(friend_node *list, friend_info *info);

/*
 * Returns 0 if success 
 * Returns < 0 if error
 */
int del_friend(friend_node *list, const char *name);

/*
 * Returns true of false wheter friend_name is in the list or not
 */
boolean is_friend(friend_node *list, char *name);

#endif
