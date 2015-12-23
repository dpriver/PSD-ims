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

#ifndef __FRIEND_REQUEST
#define __FRIEND_REQUEST

#include "list.h"


typedef struct request_info request_info;
struct request_info{
	char *name;
	int timestamp;
};

typedef struct request_list_info request_list_info;
struct request_list_info {
	int timestamp;
};

typedef list friend_requests;
typedef list_iterator req_iterator;

/* =========================================================================
 *  Structs access
 * =========================================================================*/

#define req_name(request_info) \
		(request_info->name)

#define req_time(request_info) \
		(request_info->timestamp)

#define req_num_requests(friend_requests) \
		list_num_elems(friend_requests)	

#define req_list_timestamp(requests, req_timestamp) \
	do{ \
		request_list_info *aux; \
		aux = list_info(requests); \
		req_timestamp = aux->timestamp; \
	}while(0)

#define req_list_set_timestamp(requests, req_timestamp) \
	do{ \
		request_list_info *aux; \
		aux = list_info(requests); \
		aux->timestamp = req_timestamp; \
	}while(0)


/* =========================================================================
 *  Friend requests iterator
 * =========================================================================*/
#define req_get_requests_iterator(list) \
		(req_iterator*)list_iterator(list)

#define req_iterator_next(list, iterator) \
		(req_iterator*)list_iterator_next(list, iterator)

#define req_get_info(iterator) \
		(request_info*)list_iterator_info(iterator)


/* =========================================================================
 *  Friend requests struct API
 * =========================================================================*/

/*
 * Allocates a new friend request list
 * Returns a pointer to the list or NULL if fails
 */
friend_requests *req_new(int max);

/*
 * Frees the request list
 */
void req_free(friend_requests *requests);

/*
 * Prints all requests line by line
 */
void req_print_friend_list(friend_requests *requests);

/*
 * Creates a new request in the list with the provided info
 * Returns 0 or -1 if fails
 */
int req_add_request(friend_requests *requests, const char *name, int timestamp);

/*
 * Removes the request from the list
 * Returns 0 or -1 if it does not exist
 */
int req_del_request(friend_requests *requests, const char *name);

/*
 * Finds the request especified by "name"
 * Returns a pointer to the request_info or NULL if fails
 */
request_info *req_find_request(friend_requests *requests, const char *name);


#endif /* __FRIEND_REQUEST */
