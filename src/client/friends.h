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

typedef friend_node friend_list;

typedef struct request_info request_info;
struct request_info {
	char *user_name;
	int send_date;
};

typedef struct friend_request_node friend_request_node;
struct friend_request_node {
	request_info *info;
	friend_request_node *next;
	friend_request_node *prev;
};

typedef friend_request_node friend_request_list;

typedef struct friends friends;
struct friends {
	friend_list *friend_list;
	friend_request_list *sended_requests;
	friend_request_list *received_requests;
	int n_received_requests;
};

/* =========================================================================
 *  Structs access macros
 * =========================================================================*/
#define fri_GET_FRIEND_NAME(friend_info) \
		friend_info->name;

#define fri_GET_FRIEND_INFORMATION(friend_info) \
		friend_info->information;

#define fri_GET_REQUEST_NAME(request_info) \
		request_info->user_name;

#define fri_SET_REQUEST_SEND_DATE(request_info) \
		request_info->send_date;

#define fri_GET_N_REQUEST(friend_struct) \
		friend_struct->n_received_requests


/* =========================================================================
 *  Friend struct API
 * =========================================================================*/

/*
 * Allocates a new friend list
 * Returns a pointer to the list phantom node or NULL if fails
 */
friends *fri_new();

/*
 * Frees the friend list
 */
void fri_free(friends *friends);

/*
 * Prints all friends line by line
 */
void fri_print_friend_list(friends *friends);

/*
 * Prints all sended request line by line
 */
void fri_print_snd_request_list(friends *friends);

/*
 * Prints all received request line by line
 */
void fri_print_rcv_request_list(friends *friends);

/*
 * Creates a new friend_node in the list with the provided info
 * "*info" is attached, not copied
 * Returns 0 or -1 if fails
 */
int fri_add_friend(friends *friends, const char *name, const char *information);

/*
 * Add a sended friend request with the provided info
 * Returns 0 or -1 if fails
 */
int fri_add_snd_request(friends *friends, const char *user_name, int send_date);

/*
 * Add a received friend request with the provided info
 * Returns 0 or -1 if fails
 */
int fri_add_rcv_request(friends *friends, const char *user_name, int send_date);

/*
 * Removes and frees the first node that matches the provided "name"
 * Returns 0 or -1 if "name" does not exist in the list
 */
int fri_del_friend(friends *friends, const char *name);

/*
 * Deletes the sended friend request coincident with "name"
 * Returns 0 or -1 if fails
 */
int fri_del_snd_request(friends *friends, const char *user_name);

/*
 * Deletes the received friend request coincident with "name"
 * Returns 0 or -1 if fails
 */
int fri_del_rcv_request(friends *friends, const char *user_name);

/*
 * Finds the chat whos id is chat_id
 * Returns a pointer to the chat_info of NULL if fails
 */
friend_info *fri_find_friend(friends *friends, const char *name);

/*
 * Finds the chat whos id is chat_id
 * Returns a pointer to the chat_info of NULL if fails
 */
request_info *fri_find_snd_request(friends *friends, const char *name);

/*
 * Finds the chat whos id is chat_id
 * Returns a pointer to the chat_info of NULL if fails
 */
request_info *fri_find_rcv_request(friends *friends, const char *name);


/* =========================================================================
 *  Friend list
 * =========================================================================*/

/*
 * Allocates a new friend list
 *
 * Returns a pointer to the list phantom node or NULL if fails
 */
friend_list *fri_lst_new();

/*
 * Frees the friend list
 */
void fri_lst_free(friend_list *list);

/*
 * Prints all friends line by line
 */
void fri_lst_print(friend_list *list);

/*
 * Creates a new friend_node in the list with the provided info
 * "*info" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int fri_lst_add(friend_list *list, friend_info *info);

/*
 * Removes and frees the first node that matches the provided "name"
 *
 * Returns 0 or -1 if "name" does not exist in the list
 */
int fri_lst_del(friend_list *list, const char *name);

/*
 * Finds the chat whos id is chat_id
 *
 * Returns a pointer to the chat_info of NULL if fails
 */
friend_info *fri_lst_find(friend_list *list, const char *name);


/* =========================================================================
 *  Friends
 * =========================================================================*/

/*
 * Allocates a new friend_info struct with the provided data
 *
 * Returns a pointer to the structure or NULL if fails
 */
friend_info *fri_info_new(const char *name, const char *information);

/*
 * Frees the friend_info struct
 */
void fri_info_free(friend_info *info);


/* =========================================================================
 *  Friend request
 * =========================================================================*/

/*
 * Allocates a new request list
 *
 * Returns a pointer to the list or NULL if fails
 */
friend_request_list *fri_reqlst_new();

/*
 * Frees the request list
 */
void fri_reqlst_free(friend_request_list *list);

/*
 * Prints all request line by line
 */
void fri_reqlst_print(friend_request_list *request_list);

/*
 * Creates a new request_node in the list with the provided info
 * "*info" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int fri_reqlst_add(friend_request_list *list, request_info *info);

/*
 * Removes and frees the first node that matches the provided "name"
 *
 * Returns 0 or -1 if "name" does not exist in the list
 */
int fri_reqlst_del(friend_request_list *list, const char *name);

/*
 * Finds the request coincident with "name"
 *
 * Returns a pointer to the chat_info of NULL if fails
 */
request_info *fri_reqlst_find(friend_request_list *list, const char *name);


/* =========================================================================
 *  Requests
 * =========================================================================*/

/*
 * Allocates a new request_info struct with the provided data
 *
 * Returns a pointer to the structure or NULL if fails
 */
request_info *fri_reqinfo_new(const char *user_name, int send_date);

/*
 * Frees the request_info struct
 */
void fri_reqinfo_free(request_info *info);

#endif /* __FRIENDS */
