/*******************************************************************************
 *	friends.c
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

#include "friends.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef DEBUG_TRACE
	#undef DEBUG_TRACE
#endif

#include "debug_def.h"

#ifdef DEBUG
#include "leak_detector_c.h"
#endif


void _friends_free_info(friend_info *info) {
	DEBUG_TRACE_PRINT();
	if (info != NULL) {
		free(info->name);
		free(info->information);
		free(info);
	}
}

void _friends_delete_node(friend_node *node) {	
	DEBUG_TRACE_PRINT();
	// link list
	node->prev->next = node->next;
	node->next->prev = node->prev;

	// free node
	_friends_free_info(node->info);
	free(node);
}

friend_node *_friends_find_node(friend_list *list, const char *friend_name) {
	DEBUG_TRACE_PRINT();
	friend_node *aux_node;

	aux_node = list->next;
	while ( aux_node != list ) {
		if (strcmp(aux_node->info->name, friend_name) == 0)
			return aux_node;
		aux_node = aux_node->next;
	}

	return NULL;
}

void _friends_free_req_info(request_info *info) {
	DEBUG_TRACE_PRINT();
	if (info != NULL) {
		free(info->user_name);
		free(info);
	}
}

void _friends_delete_req_node(friend_request_node *node) {
	DEBUG_TRACE_PRINT();
	// link list
	node->prev->next = node->next;
	node->next->prev = node->prev;

	// free node
	_friends_free_req_info(node->info);
	free(node);
}


friend_request_node *_friends_find_req_node(friend_request_list *list, const char *user_name) {
	DEBUG_TRACE_PRINT();
	friend_request_node *aux_node;

	aux_node = list->next;
	while ( aux_node != list ) {
		if (strcmp(aux_node->info->user_name, user_name) == 0)
			return aux_node;
		aux_node = aux_node->next;
	}

	return NULL;
}


/* =========================================================================
 *  Friend struct API
 * =========================================================================*/

/*
 * Allocates a new friend list
 * Returns a pointer to the list phantom node or NULL if fails
 */
friends *fri_new() {
	DEBUG_TRACE_PRINT();
	friends *friends_new;

	if ( (friends_new = malloc( sizeof(friends) )) == NULL ) {
		return NULL; // could not allocate friend struct
	}

	if ( (friends_new->friend_list = fri_lst_new()) == NULL ) {
		free(friends_new);
		return NULL; // could not allocate friend list struct
	}

	if ( (friends_new->sended_requests = fri_reqlst_new()) == NULL ) {
		free(friends_new);
		fri_lst_free(friends_new->friend_list);
		return NULL; // could not allocate sended request list struct
	}

	if ( (friends_new->received_requests = fri_reqlst_new()) == NULL ) {
		free(friends_new);
		fri_lst_free(friends_new->friend_list);
		fri_reqlst_free(friends_new->sended_requests);
		return NULL; // could not allocate received request struct
	}

	friends_new->n_received_requests = 0;

	return friends_new;
}


/*
 * Frees the friend list
 */
void fri_free(friends *friends) {
	DEBUG_TRACE_PRINT();

	fri_lst_free(friends->friend_list);
	fri_reqlst_free(friends->sended_requests);
	fri_reqlst_free(friends->received_requests);
	free(friends);
}


/*
 * Prints all friends line by line
 */
void fri_print_friend_list(friends *friends) {
	DEBUG_TRACE_PRINT();
	fri_lst_print(friends->friend_list);
}


/*
 * Prints all sended request line by line
 */
void fri_print_snd_request_list(friends *friends) {
	DEBUG_TRACE_PRINT();
	fri_reqlst_print(friends->sended_requests);
}


/*
 * Prints all received request line by line
 */
void fri_print_rcv_request_list(friends *friends) {
	DEBUG_TRACE_PRINT();
	fri_reqlst_print(friends->received_requests);
}


/*
 * Creates a new friend_node in the list with the provided info
 * "*info" is attached, not copied
 * Returns 0 or -1 if fails
 */
int fri_add_friend(friends *friends, const char *name, const char *information) {
	DEBUG_TRACE_PRINT();
	friend_info *info;

	if ( (info = fri_info_new(name, information)) == NULL ) {
		return -1;
	}

	if ( fri_lst_add(friends->friend_list, info) == -1 ) {
		DEBUG_FAILURE_PRINTF("Could not add friend to list");
		return -1;
	}

	return 0;
}


/*
 * Add a sended friend request with the provided info
 * Returns 0 or -1 if fails
 */
int fri_add_snd_request(friends *friends, const char *user_name, int send_date) {
	DEBUG_TRACE_PRINT();
	request_info *info;

	if ( (info = fri_reqinfo_new(user_name, send_date)) == NULL ) {
		return -1;
	}

	if ( fri_reqlst_add(friends->sended_requests, info) == -1 ) {
		DEBUG_FAILURE_PRINTF("Could not add snd request to list");
		return -1;
	}

	return 0;
}


/*
 * Add a received friend request with the provided info
 * Returns 0 or -1 if fails
 */
int fri_add_rcv_request(friends *friends, const char *user_name, int send_date) {
	DEBUG_TRACE_PRINT();
	request_info *info;

	if ( (info = fri_reqinfo_new(user_name, send_date)) == NULL ) {
		return -1;
	}

	if ( fri_reqlst_add(friends->received_requests, info) == -1 ) {
		DEBUG_FAILURE_PRINTF("Could not add recv request to list");
		return -1;
	}

	return 0;
}


/*
 * Removes and frees the first node that matches the provided "name"
 * Returns 0 or -1 if "name" does not exist in the list
 */
int fri_del_friend(friends *friends, const char *name) {
	DEBUG_TRACE_PRINT();
	return fri_lst_del(friends->friend_list, name);
}


/*
 * Deletes the sended friend request coincident with "name"
 * Returns 0 or -1 if fails
 */
int fri_del_snd_request(friends *friends, const char *user_name) {
	DEBUG_TRACE_PRINT();
	return fri_reqlst_del(friends->sended_requests, user_name);
}


/*
 * Deletes the received friend request coincident with "name"
 * Returns 0 or -1 if fails
 */
int fri_del_rcv_request(friends *friends, const char *user_name) {
	DEBUG_TRACE_PRINT();
	return fri_reqlst_del(friends->received_requests, user_name);
}


/*
 * Finds the chat whos id is chat_id
 * Returns a pointer to the chat_info of NULL if fails
 */
friend_info *fri_find_friend(friends *friends, const char *name) {
	DEBUG_TRACE_PRINT();
	friend_node *node;

	if( (node = _friends_find_node(friends->friend_list, name)) == NULL ) {
		return NULL;
	}	
	
	return node->info;
}


/*
 * Finds the chat whos id is chat_id
 * Returns a pointer to the chat_info of NULL if fails
 */
request_info *fri_find_snd_request(friends *friends, const char *name) {
	DEBUG_TRACE_PRINT();
	friend_request_node *node;
	node = _friends_find_req_node(friends->sended_requests, name);	
	return node->info;
}


/*
 * Finds the chat whos id is chat_id
 * Returns a pointer to the chat_info of NULL if fails
 */
request_info *fri_find_rcv_request(friends *friends, const char *name) {
	DEBUG_TRACE_PRINT();
	friend_request_node *node;
	node = _friends_find_req_node(friends->received_requests, name);	
	return node->info;
}


/* =========================================================================
 *  Friend list
 * =========================================================================*/

/*
 * Allocates a new friend list
 * Returns a pointer to the list phantom node or NULL if fails
 */
friend_list *fri_lst_new() {
	DEBUG_TRACE_PRINT();
	friend_node *new_list = NULL;

	if ( new_list = malloc( sizeof(friend_node) ) ) {
		new_list->info = NULL;
		new_list->next = new_list;
		new_list->prev = new_list;
	}

	return new_list;
}


/*
 * Frees the friend list
 */
void fri_lst_free(friend_list *list) {
	DEBUG_TRACE_PRINT();
	while ( list->next != list ) {
		_friends_delete_node(list->next);
	}

	free(list);
}


/*
 * Prints all friends line by line
 */
void fri_lst_print(friend_list *list) {
	DEBUG_TRACE_PRINT();
	friend_node *aux_node;

	aux_node = list->next;
	while ( aux_node != list ) {
		printf("%s :%s\n", aux_node->info->name, aux_node->info->information);
		aux_node = aux_node->next;
	}
}


/*
 * Creates a new friend_node in the list with the provided info
 * "*info" is attached, not copied
 * Returns 0 or -1 if fails
 */
int fri_lst_add(friend_list *list, friend_info *info) {
	DEBUG_TRACE_PRINT();
	friend_node *node;

	if (node = malloc(sizeof(friend_node))) {
		node->info = info;
		node->next = list;
		node->prev = list->prev;
		list->prev->next = node;
		list->prev = node;
		return 0;
	}

	return -1;
}


/*
 * Removes and frees the first node that matches the provided "name"
 * Returns 0 or -1 if "name" does not exist in the list
 */
int fri_lst_del(friend_list *list, const char *name) {
	DEBUG_TRACE_PRINT();
	friend_node *aux_node;	
	if ( aux_node = _friends_find_node(list, name) ) {
		_friends_delete_node(aux_node);
		return 0;	
	}
	DEBUG_FAILURE_PRINTF("Could not find friend in list");
	return -1;
}


/*
 * Finds the chat whos id is chat_id
 * Returns a pointer to the chat_info of NULL if fails
 */
friend_info *fri_lst_find(friend_list *list, const char *name) {
	DEBUG_TRACE_PRINT();
	friend_node *friend_node;
	friend_node = _friends_find_node(list, name);	
	return friend_node->info;
}


/* =========================================================================
 *  Friends
 * =========================================================================*/

/*
 * Allocates a new friend_info struct with the provided data
 * Returns a pointer to the structure or NULL if fails
 */
friend_info *fri_info_new(const char *name, const char *information) {
	DEBUG_TRACE_PRINT();
	friend_info *info;

	if (info = malloc(sizeof(friend_info))) {
		info->name = malloc(strlen(name) + sizeof(char));
		info->information = malloc(strlen(information) + sizeof(char));

		strcpy(info->name, name);
		strcpy(info->information, information);

		return info;
	}

	return NULL;
}


/*
 * Frees the friend_info struct
 */
void fri_info_free(friend_info *info) {
	DEBUG_TRACE_PRINT();
	_friends_free_info(info);
}


/* =========================================================================
 *  Friend request
 * =========================================================================*/

/*
 * Allocates a new request list
 * Returns a pointer to the list or NULL if fails
 */
friend_request_list *fri_reqlst_new() {
	DEBUG_TRACE_PRINT();
	friend_request_node *new_list = NULL;

	if ( new_list = malloc( sizeof(friend_request_node) ) ) {
		new_list->info = NULL;
		new_list->next = new_list;
		new_list->prev = new_list;
	}

	return new_list;
}


/*
 * Frees the request list
 */
void fri_reqlst_free(friend_request_list *list) {
	DEBUG_TRACE_PRINT();
	while ( list->next != list ) {
		_friends_delete_req_node(list->next);
	}

	free(list);
}


/*
 * Prints all request line by line
 */
void fri_reqlst_print(friend_request_list *list) {
	DEBUG_TRACE_PRINT();
	friend_request_node *aux_node;

	aux_node = list->next;
	while ( aux_node != list ) {
		printf("%d :%s\n", aux_node->info->send_date, aux_node->info->user_name);
		aux_node = aux_node->next;
	}
}


/*
 * Creates a new request_node in the list with the provided info
 * "*info" is attached, not copied
 *
 * Returns 0 or -1 if fails
 */
int fri_reqlst_add(friend_request_list *list, request_info *info) {
	DEBUG_TRACE_PRINT();
	friend_request_node *node;

	if (node = malloc(sizeof(friend_request_node))) {
		node->info = info;
		node->next = list;
		node->prev = list->prev;
		list->prev->next = node;
		list->prev = node;
		return 0;
	}

	return -1;
}


/*
 * Removes and frees the first node that matches the provided "name"
 * Returns 0 or -1 if "name" does not exist in the list
 */
int fri_reqlst_del(friend_request_list *list, const char *name) {
	DEBUG_TRACE_PRINT();
	friend_request_node *aux_node;	
	if ( aux_node = _friends_find_req_node(list, name) ) {
		_friends_delete_req_node(aux_node);
		return 0;	
	}
	DEBUG_FAILURE_PRINTF("Could not find request in list");
	return -1;
}


/*
 * Finds the request coincident with "name"
 * Returns a pointer to the chat_info of NULL if fails
 */
request_info *fri_reqlst_find(friend_request_list *list, const char *name) {
	DEBUG_TRACE_PRINT();
	friend_request_node *node;
	node = _friends_find_req_node(list, name);	
	return node->info;
}


/* =========================================================================
 *  Requests
 * =========================================================================*/

/*
 * Allocates a new request_info struct with the provided data
 * Returns a pointer to the structure or NULL if fails
 */
request_info *fri_reqinfo_new(const char *user_name, int send_date) {
	DEBUG_TRACE_PRINT();
	request_info *info;

	if (info = malloc(sizeof(request_info))) {
		info->user_name = malloc(strlen(user_name) + sizeof(char));
		info->send_date = send_date;

		strcpy(info->user_name, user_name);

		return info;
	}

	return NULL;
}


/*
 * Frees the request_info struct
 */
void fri_reqinfo_free(request_info *info) {
	DEBUG_TRACE_PRINT();
	_friends_free_req_info(info);
}


