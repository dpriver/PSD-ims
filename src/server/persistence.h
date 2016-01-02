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
	int thread_safe;
	char *location;
	char *bd_name;
	char *user_name;
	char *user_pass;
};

#define persistence_thread_safe(persistence) \
		(persistence->thread_safe)

#define clone_persistence(persistence) \
		init_persistence(persistence->user_name, persistence->user_pass)

persistence * init_persistence(char user[],char pass[]);

void free_persistence(persistence *persistence);

int add_user(persistence* persistence, char* name, char* pass, char* information);

int del_user(persistence* persistence, char* name);

int user_exist(persistence* persistence, char name[]);

int get_user_pass(persistence* persistence, char name[], char *buff, int max_chars);

int get_user_id(persistence* persistence, char name[]);

int get_user_name(persistence* persistence, int user_id,char* buff, int max_chars);

int get_user_info(persistence* persistence, int user_id,char* buff, int max_chars);

int get_id_admin_chat(persistence* persistence,int id_chat);

int get_chat_info(persistence* persistence, int chat_id,char* buff, int max_chars);

int exist_user_in_chat(persistence* persistence,int user_id, int chat_id);

int exist_user_entry_in_chat(persistence* persistence,int user_id, int chat_id);

int chat_exist(persistence* persistence, int chat_id);

int get_list_friends(persistence* persistence,int user_id, int timestamp, struct soap *soap, psdims__user_list *friends);

int get_member_list_chats(persistence* persistence,int chat_id, int timestamp, struct soap *soap, psdims__member_list *members);

int get_list_messages(persistence* persistence, int chat_id, int user_id, int timestamp, struct soap *soap, psdims__message_list *messages);

int del_user_all_chats(persistence* persistence, int user_id, int timestamp);

int get_list_chats(persistence* persistence,int user_id, int timestamp, struct soap *soap, psdims__chat_list *chats);

int exist_timestamp_in_messages(persistence* persistence, int chat_id, int timestamp);

int send_messages(persistence* persistence,int chat_id, int user_id, int timestamp, psdims__message_info *message);

int decline_friend_request(persistence* persistence, int user_id1, int user_id2);

int accept_friend_request(persistence* persistence, int user_id1, int user_id2, int timestamp);

int send_request(persistence* persistence, int user_id1, int user_id2, int timestamp);

int exist_request(persistence* persistence,int user_id1, int user_id2);

int exist_friendly(persistence* persistence,int user_id1, int user_id2);

int del_friends(persistence* persistence, int user_id1, int user_id2);

int add_chat(persistence* persistence, int admin_id, char* description, int timestamp,int *chat_id);

int del_chat(persistence* persistence, int user_id);

int add_user_chat(persistence* persistence, int user_id, int chat_id, int read_timestamp, int timestamp);

int del_user_chat(persistence* persistence, int user_id, int chat_id, int timestamp);

int change_admin(persistence* persistence, int user_id, int chat_id, int timestamp);

int is_admin(persistence* persistence, int user_id, int chat_id);

int still_users_in_chat(persistence* persistence,int chat_id);

int get_first_users_in_chat(persistence* persistence,int chat_id);

int get_all_chat_info(persistence* persistence,int chat_id, struct soap *soap, psdims__chat_info *chat);

int set_file(persistence* persistence, int user_id, int chat_id,char* path, int timestamp);

int get_file(persistence* persistence, int user_id, int chat_id,char* path, int timestamp);

int message_have_attach(persistence *persistence, int user_id, int chat_id, int msg_timestamp);

int message_can_attach(persistence *persistence, int user_id, int chat_id, int msg_timestamp);

int update_sync(persistence *persistence, int user_id, int chat_id, int read_timestamp);

int get_notif_chats_with_messages(persistence *persistence, int user_id, int timestamp, struct soap *soap, psdims__notif_chat_list *chat_list);

int get_notif_chats_read_times(persistence *persistence, int user_id, struct soap *soap, psdims__notif_chat_list *chat_list);

int get_notif_friend_requests(persistence *persistence, int user_id, int timestamp, struct soap *soap, psdims__notif_friend_list *request_list);

int get_notif_chat_members(persistence *persistence, int user_id, int timestamp, struct soap *soap, psdims__notif_chat_member_list *member_list);

int get_notif_chat_rem_members(persistence *persistence, int user_id, int timestamp, struct soap *soap, psdims__notif_chat_member_list *member_list) ;

int get_notif_chat_admins(persistence *persistence, int user_id, int timestamp, struct soap *soap, psdims__notif_chat_member_list *member_list);

#endif /* __PERSISTENCE */

