/*******************************************************************************
 *	persistance.h
 *
 *  functionality to allow the server-side persistance of data
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

#ifndef __PERSISTENCE
#define __PERSISTENCE

#include <mysql.h>
#include "soapH.h"

typedef struct persistence persistence;
struct persistence {
	MYSQL *mysql;
	char *location;
	char *bd_name;
	char *user_name;
	char *user_pass;
};


persistence * init_persistence(char user[],char pass[]);

void free_persistence(persistence *persistence);

int add_user(persistence* persistence, char* name, char* pass, char* information);

int del_user(persistence* persistence, char* name);

int user_exist(persistence* persistence, char name[]);

char* get_user_pass(persistence* persistence, char name[]);

int get_user_id(persistence* persistence, char name[]);

int get_user_name(persistence* persistence, int user_id,char* buff);

int get_user_info(persistence* persistence, int user_id,char* buff);

int get_list_friends(persistence* persistence,int user_id,psdims__user_list *friends);

int decline_friend_request(persistence* persistence, int user_id1, int user_id2);

int accept_friend_request(persistence* persistence, int user_id1, int user_id2);

int send_request(persistence* persistence, int user_id1, int user_id2);

int del_friends(persistence* persistence, int user_id1, int user_id2);

int add_chat(persistence* persistence, int admin_id, char* description);

int del_chat(persistence* persistence, int user_id);

int add_user_chat(persistence* persistence, int user_id, int chat_id);

int del_user_chat(persistence* persistence, int user_id, int chat_id);


#endif /* __PERSISTENCE */

