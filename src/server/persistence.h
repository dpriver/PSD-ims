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

#ifndef __PERSISTANCE
#define __PERSISTANCE

#include <mysql.h>

MYSQL* init_bd(char user[],char pass[],char name_bd[]);

void add_user(MYSQL* bd,char* name,char* pass, char* information);

void del_user(MYSQL* bd,char* name);

int exist_user(MYSQL* bd,char name[]);

int get_id_user(MYSQL* bd,char name[]);

void accept_friend(MYSQL* bd, int id1,int id2);

void send_request(MYSQL* bd,int id1,int id2);

void refuse_request(MYSQL* bd,int id1,int id2);

void del_friends(MYSQL* bd,int id1,int id2);

void add_chat(MYSQL* bd,int id_admin, char* description);

void del_chat(MYSQL* bd,int id);

void add_user_chat(MYSQL* bd,int id_user,int id_chat);

void del_user_chat(MYSQL* bd,int id_user,int id_chat);

#endif /* __PERSISTANCE */

