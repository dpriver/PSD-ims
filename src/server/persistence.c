/*******************************************************************************
 *	persistence.c
 *
 *  functionality to allow the server-side persistance of data
 *
 *
 *  This file is part of PSD-IMS
 * 
 *  Copyright (C) 2015  Daniel Pinto Rivero, Javier BermÃºdez Blanco
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <time.h>
#include "persistence.h"
#include "bool.h"

#include "debug_def.h"

#define MAX_QUERY_CHARS (500)

persistence * init_persistence(char user[],char pass[]){
	DEBUG_TRACE_PRINT();
	persistence *new_persistence;

	if ( (new_persistence = malloc(sizeof(persistence)) ) == NULL ) {
		DEBUG_FAILURE_PRINTF("Failed to initialize the persistence struct");
		return NULL;
	}

	if( mysql_thread_safe() ) {
		DEBUG_INFO_PRINTF("Mysql is thread safe\n");
	}
	else{
		DEBUG_FAILURE_PRINTF("Mysql is not thread safe\n");
	}
	new_persistence->thread_safe = mysql_thread_safe();


	new_persistence->mysql = mysql_init(NULL);
	if (new_persistence->mysql == NULL ) {
		DEBUG_FAILURE_PRINTF("Failed to initialize the database struct");
		return NULL;
	}

  if(!mysql_real_connect(new_persistence->mysql, "localhost", user, pass, "PSD", 0, NULL, 0)){
		DEBUG_FAILURE_PRINTF("Failed to conect to the database");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(new_persistence->mysql)); 
    return NULL;
  }

	new_persistence->location = malloc(sizeof(char)*20);
	new_persistence->bd_name = malloc(sizeof(char)*4);
	new_persistence->user_name = malloc(strlen(user)+sizeof(char));
	new_persistence->user_pass = malloc(strlen(pass)+sizeof(char));

	strcpy(new_persistence->location, "localhost");
	strcpy(new_persistence->bd_name, "PSD");
	strcpy(new_persistence->user_name, user);
	strcpy(new_persistence->user_pass, pass);

	return new_persistence;
}


int reconnect_persistence(persistence *persistence) {
	mysql_close(persistence->mysql);
	if(!mysql_real_connect(persistence->mysql, "localhost", persistence->user_name, persistence->user_pass, "PSD", 0, NULL, 0)){
		DEBUG_FAILURE_PRINTF("Failed to reconect to the database");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}
	return 0;
}

int persistence_err(persistence *persistence) {
	return mysql_errno(persistence->mysql);
}


void free_persistence(persistence *persistence) {
	free(persistence->location);
	free(persistence->bd_name);
	free(persistence->user_name);
	free(persistence->user_pass);
	mysql_close(persistence->mysql);
	free(persistence);
}


int add_user(persistence* persistence, char* name, char* pass, char* information){
	char consulta[MAX_QUERY_CHARS]="INSERT INTO users(NAME,PASS,INFORMATION, VALID) VALUES('";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}
	if( (strlen(name) + strlen(pass) + strlen(information) + sizeof(char)*58) >= (sizeof(char)*MAX_QUERY_CHARS) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta,name);
	strcat(consulta,"','");
	strcat(consulta,pass);
	strcat(consulta,"','");
	strcat(consulta,information);
	strcat(consulta,"',1);");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	return 0;
}


int del_user(persistence* persistence, char* name){
	char consulta[MAX_QUERY_CHARS]="UPDATE users SET VALID = 0 where NAME = '";

	if(persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}
	if( (strlen(name) + sizeof(char)*34) >= (sizeof(char)*MAX_QUERY_CHARS) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta,name);
	strcat(consulta,"';");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	return 0;
}

int user_exist(persistence* persistence, char name[]){
	DEBUG_TRACE_PRINT();

	int resultado;

	char consulta[MAX_QUERY_CHARS]="SELECT * FROM users where ((NAME = '";

	if(persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}
	if( (strlen(name) + sizeof(char)*36 ) >= (sizeof(char)*MAX_QUERY_CHARS) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta,name);
	strcat(consulta,"') and (VALID = 1));");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	if( mysql_num_rows(mysql_store_result(persistence->mysql)) >= 1 ){
		return 1;
	}

	return 0;
}


int user_entry_exist(persistence* persistence, char name[]){
	DEBUG_TRACE_PRINT();

	int resultado;

	char consulta[MAX_QUERY_CHARS]="SELECT * FROM users where ((NAME = '";

	if(persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}
	if( (strlen(name) + sizeof(char)*36 ) >= (sizeof(char)*MAX_QUERY_CHARS) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta,name);
	strcat(consulta,"'));");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	if( mysql_num_rows(mysql_store_result(persistence->mysql)) >= 1 ){
		return 1;
	}

	return 0;
}


int get_user_pass(persistence* persistence, char name[], char *buff, int max_chars){
	DEBUG_TRACE_PRINT();
	int resultado;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char consulta[MAX_QUERY_CHARS]="SELECT PASS FROM users where ((NAME = '";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}
	if( (strlen(name) + strlen(consulta) + sizeof(char)*2) >= (sizeof(char)*MAX_QUERY_CHARS) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta,name);
	strcat(consulta,"') and (VALID = 1));");


	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	res = mysql_store_result(persistence->mysql);	
	if (mysql_num_rows(res) < 1 ) {
		DEBUG_FAILURE_PRINTF("Could not get user password");
		return -1;
	}
	
	row = mysql_fetch_row(res);
	if ( strlen(row[0]) >= max_chars ) {
		DEBUG_FAILURE_PRINTF("password is to long");
		return -1;		
	}
	
	strcpy(buff, row[0]);

	return 0;
}

int get_user_id(persistence* persistence, char name[]){
	DEBUG_TRACE_PRINT();
	int resultado;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char consulta[MAX_QUERY_CHARS]="SELECT ID FROM users where ((NAME = '";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}
	if( (strlen(name) + strlen(consulta) + sizeof(char)*2) >= (sizeof(char)*MAX_QUERY_CHARS) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta,name);
	strcat(consulta,"') and (VALID = 1));");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	// TODO check for errors here
	res = mysql_store_result(persistence->mysql);
	if (mysql_num_rows(res) < 1 ) {
		DEBUG_FAILURE_PRINTF("Could not get user id");
		return -1;
	}
	
	row = mysql_fetch_row(res);

	return atoi(row[0]);
}

int get_user_name(persistence* persistence, int user_id, char* buff, int max_chars){
	DEBUG_TRACE_PRINT();

	int resultado;
	char str_id[20];
	MYSQL_RES *res;
	MYSQL_ROW row;

	sprintf(str_id, "%d", user_id);

	char consulta[MAX_QUERY_CHARS]="SELECT NAME FROM users where ID = '";

	if(persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}
	if((strlen(str_id) + strlen(consulta) + sizeof(char)*2) >= (sizeof(char)*MAX_QUERY_CHARS) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta,str_id);
	strcat(consulta,"';");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	res = mysql_store_result(persistence->mysql);
	if (mysql_num_rows(res) < 1 ) {
		DEBUG_FAILURE_PRINTF("Could not get user name");
		return -1;
	}
	row = mysql_fetch_row(res);

	if (strlen(row[0]) >= max_chars) {
		DEBUG_FAILURE_PRINTF("name is too long");
	}
	strcpy(buff,row[0]);

	return 0;
}

int get_user_info(persistence* persistence, int user_id,char* buff, int max_chars){
	DEBUG_TRACE_PRINT();

	int resultado;
	char str_id[20];
	MYSQL_RES *res;
	MYSQL_ROW row;
	char consulta[MAX_QUERY_CHARS]="SELECT INFORMATION FROM users where ID = '";

	sprintf(str_id, "%d", user_id);

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}
	if( (strlen(str_id) + strlen(consulta) + sizeof(char)*2) >= (sizeof(char)*MAX_QUERY_CHARS) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta,str_id);
	strcat(consulta,"';");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	res = mysql_store_result(persistence->mysql);
	if (mysql_num_rows(res) < 1 ) {
		DEBUG_FAILURE_PRINTF("Could not get user name");
		return -1;
	}
	row = mysql_fetch_row(res);

	if (strlen(row[0]) >= max_chars) {
		DEBUG_FAILURE_PRINTF("name is too long");
	}
	strcpy(buff,row[0]);


	return 0;
}

int get_id_admin_chat(persistence* persistence,int id_chat){
	DEBUG_TRACE_PRINT();

	int resultado;
	char str_id[20];
	MYSQL_RES *res;
	MYSQL_ROW row;
	char consulta[MAX_QUERY_CHARS]="SELECT ID_ADMIN FROM chats where ID = '";

	sprintf(str_id, "%d", id_chat);

	if(persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}
	if((strlen(str_id) + strlen(consulta) + sizeof(char)*2) >= (sizeof(char)*MAX_QUERY_CHARS) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta,str_id);
	strcat(consulta,"';");

	if(mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	// TODO check for errors here
	res = mysql_store_result(persistence->mysql);
	if (mysql_num_rows(res) < 1 ) {
		DEBUG_FAILURE_PRINTF("Could not get admin id");
		return -1;
	}
	
	row = mysql_fetch_row(res);

	return atoi(row[0]);
}

int get_chat_info(persistence* persistence, int chat_id,char* buff, int max_chars){
	DEBUG_TRACE_PRINT();

	int resultado;
	char str_id[20];
	MYSQL_RES *res;
	MYSQL_ROW row;

	sprintf(str_id, "%d", chat_id);

	char consulta[200]="SELECT DESCRIPTION FROM chats where ID = '";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}
	if( (strlen(str_id) + strlen(consulta)) >= (sizeof(char)*190) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta,str_id);
	strcat(consulta,"';");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	// TODO check for errors here
	res = mysql_store_result(persistence->mysql);
	if (mysql_num_rows(res) < 1 ) {
		DEBUG_FAILURE_PRINTF("Could not get user name");
		return -1;
	}
	row = mysql_fetch_row(res);

	if( strlen(row[0]) >= max_chars ) {
		DEBUG_FAILURE_PRINTF("chat info too long");
		return -1;
	}
	strcpy(buff,row[0]);	

	return 0;
}

int exist_user_in_chat(persistence* persistence,int user_id, int chat_id){
	DEBUG_TRACE_PRINT();

	int resultado;
	char str_id[20],str_chat[20];
	MYSQL_RES *res;
	MYSQL_ROW row;

	sprintf(str_id, "%d", user_id);
	sprintf(str_chat, "%d", chat_id);

	char consulta[200]="SELECT * FROM users_chats where ((ID_USERS = ";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	if( (strlen(str_id) + strlen(consulta)) >= (sizeof(char)*190) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta,str_id);
	strcat(consulta,") AND (ID_CHAT = ");
	strcat(consulta,str_chat);
	strcat(consulta,") AND (REM_TIME = 0));");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	res = mysql_store_result(persistence->mysql);

	return (mysql_num_rows(res) > 0)? 1 : 0;
}

int exist_user_entry_in_chat(persistence* persistence,int user_id, int chat_id){
	DEBUG_TRACE_PRINT();

	int resultado;
	char str_id[20],str_chat[20];
	MYSQL_RES *res;
	MYSQL_ROW row;

	sprintf(str_id, "%d", user_id);
	sprintf(str_chat, "%d", chat_id);

	char consulta[200]="SELECT * FROM users_chats where ((ID_USERS = ";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	if( (strlen(str_id) + strlen(consulta)) >= (sizeof(char)*190) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta,str_id);
	strcat(consulta,") AND (ID_CHAT = ");
	strcat(consulta,str_chat);
	strcat(consulta,"));");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	res = mysql_store_result(persistence->mysql);

	return (mysql_num_rows(res) > 0)? 1 : 0;
}

int chat_exist(persistence* persistence, int chat_id){
	DEBUG_TRACE_PRINT();

	int resultado;
	char str_chat[20];

	sprintf(str_chat, "%d", chat_id);

	char consulta[200]="SELECT * FROM chats where ((ID = '";

	if(persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}
	if((strlen(str_chat) + strlen(consulta)) >= (sizeof(char)*190) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta,str_chat);
	strcat(consulta,"') and (VALID = 1));");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	return (mysql_num_rows(mysql_store_result(persistence->mysql)) > 0)? 1 : 0;
}


int get_list_friends(persistence* persistence,int user_id, int timestamp, struct soap *soap, psdims__user_list *friends){
	DEBUG_TRACE_PRINT();

	char str_id[20];
	char str_time[20];
	int resultado;
	int i;
	int j;
	int k;
	int totalrows;
	int numfields;
	char name[50];
	char info[500];
  	MYSQL_RES *res;
  	MYSQL_ROW row;
	
	sprintf(str_id, "%d", user_id);
	sprintf(str_time, "%d", timestamp);

  	char consulta[500]="select DISTINCT users.NAME, users.INFORMATION from users " \
		"INNER JOIN friends on ( (friends.ID1 = users.ID ) OR (friends.ID2 = " \
		"users.ID )) where ( ((friends.ID1 = ";
	
	if( (strlen(str_id)*2 + strlen(consulta)) >= (sizeof(char)*450) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}
 
  	strcat(consulta,str_id);
  	strcat(consulta,") or (friends.ID2 = ");
    strcat(consulta,str_id);
  	strcat(consulta,")) and (users.ID != ");
  	strcat(consulta, str_id);
  	strcat(consulta, ") and (friends.CREATION_TIME >= ");
  	strcat(consulta, str_time); 
  	strcat(consulta, ") and (users.VALID = 1));");	
 
  	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
    	return -1;
	}

    res = mysql_store_result(persistence->mysql);
    totalrows = mysql_num_rows(res);
    numfields = mysql_num_fields(res);

	

	friends->user = soap_malloc(soap, sizeof(psdims__user_info)*totalrows);
	friends->__sizenelems = totalrows;
	
	for( k = 0 ; k < totalrows ; k++ ){
		row = mysql_fetch_row(res);
		friends->user[k].name = soap_malloc(soap, strlen(row[0])+sizeof(char));
		friends->user[k].information = soap_malloc(soap, strlen(row[1])+sizeof(char));
		strcpy(friends->user[k].name, row[0]);
		strcpy(friends->user[k].information, row[1]);
	}

	return 0;
}

int get_member_list_chats(persistence* persistence, int chat_id, int timestamp, struct soap *soap, psdims__member_list *members){
	DEBUG_TRACE_PRINT();

	char str_id[20];
	char str_time[20];
	int resultado;
	int i;
	int id_admin;
	int totalrows;
	int numfields;
	char name[50];
	char info[500];
	MYSQL_RES *res;
	MYSQL_ROW row;

	sprintf(str_id, "%d", chat_id);	
	sprintf(str_time, "%d", timestamp);


	char consulta[500]="select NAME from users INNER JOIN users_chats on " \
			"(users_chats.ID_USERS = users.ID) where ((users_chats.ID_CHAT = "; 	

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	strcat(consulta, str_id);
	strcat(consulta,") and (users_chats.CREATION_TIME >= ");
	strcat(consulta, str_time);
	strcat(consulta, ") and (users_chats.REM_TIME = 0));");

	if( (strlen(str_id) + strlen(str_time) + strlen(consulta)) >= (sizeof(char)*450) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	res = mysql_store_result(persistence->mysql);
	totalrows = mysql_num_rows(res);

	members->name = soap_malloc(soap, sizeof(psdims__string)*totalrows);
	members->__sizenelems = totalrows;

	for( i = 0 ; i < totalrows ; i++ ){
		row = mysql_fetch_row(res);
		members->name[i].string = soap_malloc(soap, strlen(row[0])+sizeof(char));
		strcpy(members->name[i].string, row[0]);
	}
	
	return 0;
}

int get_list_messages(persistence* persistence,int chat_id, int user_id, int timestamp, struct soap *soap, psdims__message_list *messages){
	DEBUG_TRACE_PRINT();

	char str_chat_id[20], str_id[20], str_time[20];
	int resultado;
	int k;
	int totalrows;
	int numfields;
	char name[50];
  	MYSQL_RES *res;
  	MYSQL_ROW row;
	
	sprintf(str_chat_id, "%d", chat_id);
	sprintf(str_id, "%d", user_id);	
	sprintf(str_time, "%d", timestamp);

  	char consulta[500]="SELECT DISTINCT users.NAME, messages.TEXT, messages.CREATION_TIME, messages.FILE_ FROM messages " \
			"LEFT JOIN users_chats on ( messages.ID_CHAT = users_chats.ID_CHAT) "
			"LEFT JOIN users on (messages.ID_SENDER = users.ID) where (messages.CREATION_TIME > " \
			"users_chats.CREATION_TIME AND messages.ID_CHAT = "; 	
	
  	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
    	return -1;
  	}
	if( (strlen(str_id) + strlen(str_time) + strlen(consulta)) >= (sizeof(char)*450) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}
 
  	strcat(consulta, str_chat_id);
  	strcat(consulta, " AND messages.CREATION_TIME >= ");
    strcat(consulta, str_time);
    strcat(consulta, " AND users_chats.ID_USERS = ");
    strcat(consulta, str_id);
  	strcat(consulta, ");");	
 
  	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
    	return -1;
	}

	// TODO check for errors here
    res = mysql_store_result(persistence->mysql);
    totalrows = mysql_num_rows(res);
    numfields = mysql_num_fields(res);

	messages->last_timestamp = timestamp;
	messages->messages = soap_malloc(soap, sizeof(psdims__message_info)*totalrows);
	messages->__sizenelems=totalrows;
	
	for(k=0;k<totalrows;k++){
		row = mysql_fetch_row(res);

		messages->messages[k].user = soap_malloc(soap, strlen(row[0]) + sizeof(char));
		messages->messages[k].text = soap_malloc(soap, strlen(row[1]) + sizeof(char));
		
		// FILE_ field is NULL if the message has no attached file
		if (row[3]) {
			messages->messages[k].file_name = soap_malloc(soap, strlen(row[3]) + sizeof(char));
			strcpy(messages->messages[k].file_name, row[3]);
		}
		else {
			messages->messages[k].file_name = NULL;
		}
		
		strcpy(messages->messages[k].user, row[0]);
		strcpy(messages->messages[k].text, row[1]);
		messages->messages[k].send_date = atoi(row[2]);
		
		if (messages->messages[k].send_date > messages->last_timestamp) {
			messages->last_timestamp = messages->messages[k].send_date;
		}
	}

	messages->last_timestamp++;

	return 0;
}


int del_user_all_chats(persistence* persistence, int user_id, int timestamp){
	DEBUG_TRACE_PRINT();
	char str_user_id[10], str_time[10]; 
	char consulta[200]="UPDATE users_chats set REM_TIME = ";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	sprintf(str_user_id, "%d", user_id);
	sprintf(str_time, "%d", timestamp);

	if( (strlen(str_user_id) + strlen(consulta)) >= (sizeof(char)*180) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_time);
	strcat(consulta, " where (ID_USERS = ");
	strcat(consulta, str_user_id);
	strcat(consulta, ");");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	return 0;
}


int get_list_chats(persistence* persistence,int user_id, int timestamp, struct soap *soap, psdims__chat_list *chats){
	DEBUG_TRACE_PRINT();

	char str_id[20];
	char str_time[20];
	int resultado;
	int i;
	int id_admin;
	int totalrows;
	int numfields;
	char name[50];
	char info[500];
  	MYSQL_RES *res;
  	MYSQL_ROW row;
	
	sprintf(str_id, "%d", user_id);	
	sprintf(str_time, "%d", timestamp);

  	char consulta[500]="select chats.ID, chats.DESCRIPTION, users.NAME, users_chats.READ_MSG_TIME, chats.CREATION_TIME, chats.READ_TIME from chats " \
		"INNER JOIN users_chats on (users_chats.ID_CHAT = chats.ID) INNER JOIN " \
		"users on (users.ID = chats.ID_ADMIN ) where ((users_chats.ID_USERS = "; 	
	
  	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
    	return -1;
  	}
	if( (strlen(str_id) + strlen(consulta)) >= (sizeof(char)*450) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}
 
  	strcat(consulta,str_id);
  	strcat(consulta, ") and (chats.CREATION_TIME >= ");
  	strcat(consulta, str_time);
  	strcat(consulta,") and (users_chats.REM_TIME = 0));");

  	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
    	return -1;
	}

    res = mysql_store_result(persistence->mysql);
    totalrows = mysql_num_rows(res);

	chats->chat_info = soap_malloc(soap, sizeof(psdims__chat_info)*totalrows);
	chats->__sizenelems = totalrows;
	chats->last_timestamp = 0;
	
	for( i = 0 ; i < totalrows ; i++ ){
		row = mysql_fetch_row(res);
		chats->chat_info[i].description = soap_malloc(soap, strlen(row[1])+sizeof(char));
		chats->chat_info[i].admin = soap_malloc(soap, strlen(row[2])+sizeof(char));
		
		chats->chat_info[i].chat_id = atoi(row[0]);
		strcpy(chats->chat_info[i].description, row[1]);
		strcpy(chats->chat_info[i].admin, row[2]);
		chats->chat_info[i].read_timestamp = atoi(row[3]);
		if (atoi(row[4]) > chats->last_timestamp) {
			chats->last_timestamp = atoi(row[4]);
		}
		chats->chat_info[i].all_read_timestamp = atoi(row[5]);

		get_member_list_chats(persistence, atoi(row[0]), timestamp, soap, &(chats->chat_info[i].members));
	}

	return 0;
}

int exist_timestamp_in_messages(persistence* persistence, int chat_id, int timestamp) {
	DEBUG_TRACE_PRINT();

	char str_id[20],str_chat[20],str_time[20];
	int resultado;
  	MYSQL_RES *res;
  	MYSQL_ROW row;
	
	sprintf(str_chat, "%d", chat_id);
	sprintf(str_time, "%d", timestamp);
	
  	char consulta[200]="SELECT CREATION_TIME from messages where ((ID_CHAT = ";
  	
  	strcat(consulta, str_chat);
  	strcat(consulta, ") and (CREATION_TIME = ");
  	strcat(consulta, str_time);
  	strcat(consulta, "));");

  	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
    	return -1;
	}  	
  	
  	res = mysql_store_result(persistence->mysql);
    return mysql_num_rows(res);
}

int send_messages(persistence* persistence, int chat_id, int user_id, int timestamp, psdims__message_info *message){
	DEBUG_TRACE_PRINT();

	char str_id[20],str_chat[20],str_time[20];
	int resultado;
  	MYSQL_RES *res;
  	MYSQL_ROW row;
	
	sprintf(str_id, "%d", user_id);
	sprintf(str_chat, "%d", chat_id);
	sprintf(str_time, "%d", timestamp);
	
  	char consulta[200]="INSERT INTO messages(ID_SENDER, ID_CHAT, FILE_, TEXT, CREATION_TIME) VALUES("; 	

  	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
    	return -1;
  	}
	if( ( strlen(consulta) + strlen(str_id) + strlen(str_chat) + strlen(str_time) + strlen(message->text) ) >= (sizeof(char)*190) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}
 
  	strcat(consulta, str_id);
  	strcat(consulta,",");
	strcat(consulta, str_chat);
	if (message->file_name != NULL) {
		strcat(consulta,",'");
		strcat(consulta, message->file_name);
		strcat(consulta,"','");	
	}
	else {
		strcat(consulta,", NULL ,'");	
	}

	strcat(consulta, message->text);
  	strcat(consulta,"',");
	strcat(consulta, str_time);
	strcat(consulta,");");
 
  	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
    	return -1;
	}

	return 0;
}

int decline_friend_request(persistence* persistence, int user_id1, int user_id2){
	char str_id1[10], str_id2[10]; 
	char consulta[200]="DELETE FROM friends_request where (ID1 =";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	sprintf(str_id1, "%d", user_id1);
	sprintf(str_id2, "%d", user_id2);

	if( (strlen(str_id1) + strlen(str_id2) + strlen(consulta)) >= (sizeof(char)*170) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_id1);
	strcat(consulta, " AND ID2_request = ");
	strcat(consulta, str_id2);
	strcat(consulta, ");");


	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	return 0;
}


int accept_friend_request(persistence* persistence, int user_id1, int user_id2, int timestamp){
	char str_id1[10], str_id2[10], str_time[10]; 
	char consulta[200]="INSERT INTO friends(ID1, ID2, CREATION_TIME) VALUES(";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	sprintf(str_id1, "%d", user_id1);
	sprintf(str_id2, "%d", user_id2);
	sprintf(str_time, "%d", timestamp);

	if( (strlen(str_id1) + strlen(str_id2) + strlen(consulta)) >= (sizeof(char)*190) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_id1);
	strcat(consulta, ",");
	strcat(consulta, str_id2);
	strcat(consulta, ",");
	strcat(consulta, str_time);
	strcat(consulta, ");");


	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	// TODO This is weird... FIX
	decline_friend_request(persistence, user_id1, user_id2);

	return 0;
}


int send_request(persistence* persistence, int user_id1, int user_id2, int timestamp){
	char str_id1[10], str_id2[10],str_time[10]; 

	char consulta[200]="INSERT INTO friends_request(ID1,ID2_request,CREATION_TIME) VALUES(";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	sprintf(str_id1, "%d", user_id1);
	sprintf(str_id2, "%d", user_id2);
	sprintf(str_time, "%d", timestamp);

	if( (strlen(str_id1) + strlen(str_id2) + strlen(str_time) + strlen(consulta)) >= (sizeof(char)*190) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_id1);
	strcat(consulta, ",");
	strcat(consulta, str_id2);
	strcat(consulta, ",");
	strcat(consulta, str_time);
	strcat(consulta, ");");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	return 0;
}

int exist_request(persistence* persistence,int user_id1, int user_id2){
	DEBUG_TRACE_PRINT();

	int resultado;
	int totalrows;
	char str_id1[20],str_id2[20];
	MYSQL_RES *res;
	MYSQL_ROW row;
	char consulta[200]="SELECT * FROM friends_request where (ID1 = ";

	sprintf(str_id1, "%d", user_id1);
	sprintf(str_id2, "%d", user_id2);

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	if( (strlen(consulta) + strlen(str_id1) + strlen(str_id2)) >= (sizeof(char)*150) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_id1);
	strcat(consulta," AND ID2_request = ");
	strcat(consulta, str_id2);
	strcat(consulta,");");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	res = mysql_store_result(persistence->mysql);

	return (mysql_num_rows(res) > 0)? 1 : 0;
}

int exist_friendly(persistence* persistence,int user_id1, int user_id2){
	DEBUG_TRACE_PRINT();

	int resultado;
	int totalrows;
	char str_id1[20],str_id2[20];
	MYSQL_RES *res;
	MYSQL_ROW row;

	sprintf(str_id1, "%d", user_id1);
	sprintf(str_id2, "%d", user_id2);

	char consulta[200]="SELECT * FROM friends where ((ID1 = ";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	if( (strlen(consulta) + strlen(str_id1)*2 + strlen(str_id2)*2 ) >= (sizeof(char)*150) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta,str_id1);
	strcat(consulta," AND ID2 = ");
	strcat(consulta,str_id2);
	strcat(consulta,") OR ");

	strcat(consulta,"( ID1 = ");
	strcat(consulta,str_id2);
	strcat(consulta," AND ID2 = ");
	strcat(consulta,str_id1);
	strcat(consulta,"));");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	res = mysql_store_result(persistence->mysql);

	return (mysql_num_rows(res) > 0)? 1 : 0;
}

int del_friends(persistence* persistence, int user_id1, int user_id2){
	DEBUG_TRACE_PRINT();

	char str_id1[10], str_id2[10]; 
	char consulta[200]="DELETE FROM friends where (ID1 =";
	
	sprintf(str_id1, "%d", user_id1);
	sprintf(str_id2, "%d", user_id2);

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}
	if( (strlen(str_id1) + strlen(str_id2) + strlen(consulta)) >= (sizeof(char)*180) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_id1);
	strcat(consulta, " AND ID2 = ");
	strcat(consulta, str_id2);
	strcat(consulta, ");");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	return 0;
}

int add_user_chat(persistence* persistence, int user_id, int chat_id, int read_timestamp, int timestamp){
	char str_user_id[10], str_chat_id[10],str_time[10], str_read_time[10];
	char consulta[200]="INSERT INTO users_chats(ID_USERS,ID_CHAT,CREATION_TIME, READ_MSG_TIME, REM_TIME) VALUES(";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	sprintf(str_user_id, "%d", user_id);
	sprintf(str_chat_id, "%d", chat_id);
	sprintf(str_time, "%d", timestamp);
	sprintf(str_read_time, "%d", read_timestamp);

	if( (strlen(str_user_id) + strlen(str_chat_id) + strlen(str_time) + strlen(consulta)) >= (sizeof(char)*180) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_user_id);
	strcat(consulta, ",");
	strcat(consulta, str_chat_id);
	strcat(consulta, ",");
	strcat(consulta, str_time);
	strcat(consulta, ",");
	strcat(consulta, str_read_time);
	strcat(consulta, ", 0);");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	return 0;
}

int add_chat(persistence* persistence, int admin_id, char* description, int timestamp, int *chat_id){
	DEBUG_TRACE_PRINT();
	char str_id[10],str_time[10];
	MYSQL_RES *res;
	MYSQL_ROW row;  
	char consulta[200]="INSERT INTO chats(ID_ADMIN,DESCRIPTION,CREATION_TIME, ADMIN_TIME, VALID, READ_TIME) VALUES(";

	sprintf(str_id, "%d", admin_id);
	sprintf(str_time, "%d", timestamp);

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}
	if((strlen(str_id) + strlen(consulta)) >= (sizeof(char)*180) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_id);
	strcat(consulta, ",'");
	strcat(consulta, description);
	strcat(consulta, "',");
	strcat(consulta, str_time);
	strcat(consulta, ",");
	strcat(consulta, str_time);
	strcat(consulta, ", 1, 0);");

	if(mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	*chat_id = mysql_insert_id(persistence->mysql);

	return 0;
}

int del_chat(persistence* persistence, int id_chat){
	DEBUG_TRACE_PRINT();
	char str_id[10];
	char consulta[200]="UPDATE chats set VALID = 0 where ID = ";

	sprintf(str_id, "%d", id_chat);

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}
	if( (strlen(str_id) + strlen(consulta)) >= (sizeof(char)*180) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_id);
	strcat(consulta, ";");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	return 0;
}


int recover_user_chat(persistence* persistence, int user_id, int chat_id, int timestamp) {
	DEBUG_TRACE_PRINT();
	char str_user_id[10], str_chat_id[10], str_time[10]; 
	char consulta[200]="UPDATE users_chats set REM_TIME = 0, CREATION_TIME = ";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	sprintf(str_user_id, "%d", user_id);
	sprintf(str_chat_id, "%d", chat_id);
	sprintf(str_time, "%d", timestamp);

	if( (strlen(str_user_id) + strlen(str_chat_id) + strlen(consulta)) >= (sizeof(char)*180) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_time);
	strcat(consulta, " where ((ID_USERS = ");
	strcat(consulta, str_user_id);
	strcat(consulta, ") AND (ID_CHAT =");
	strcat(consulta, str_chat_id);
	strcat(consulta, "));");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	return 0;
}


int del_user_chat(persistence* persistence, int user_id, int chat_id, int timestamp){
	DEBUG_TRACE_PRINT();
	char str_user_id[10], str_chat_id[10], str_time[10]; 
	char consulta[200]="UPDATE users_chats set REM_TIME = ";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	sprintf(str_user_id, "%d", user_id);
	sprintf(str_chat_id, "%d", chat_id);
	sprintf(str_time, "%d", timestamp);

	if( (strlen(str_user_id) + strlen(str_chat_id) + strlen(consulta)) >= (sizeof(char)*180) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_time);
	strcat(consulta, " where (ID_USERS = ");
	strcat(consulta, str_user_id);
	strcat(consulta, " AND ID_CHAT =");
	strcat(consulta, str_chat_id);
	strcat(consulta, ");");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	return 0;
}

int change_admin(persistence* persistence, int user_id, int chat_id, int timestamp){
	DEBUG_TRACE_PRINT();
	char str_user_id[10], str_chat_id[10], str_time[10]; 
	char consulta[200]="UPDATE chats SET ID_ADMIN=";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	sprintf(str_user_id, "%d", user_id);
	sprintf(str_chat_id, "%d", chat_id);
	sprintf(str_time, "%d", timestamp);

	if( (strlen(str_user_id) + strlen(str_chat_id) + strlen(consulta)) >= (sizeof(char)*180) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_user_id);
	strcat(consulta, ", ADMIN_TIME = ");
	strcat(consulta, str_time);
	strcat(consulta, " where ID = ");
	strcat(consulta, str_chat_id);
	strcat(consulta, ";");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	return 0;
}

int is_admin(persistence* persistence, int user_id, int chat_id){
	DEBUG_TRACE_PRINT();
	char str_user_id[10], str_chat_id[10];
	MYSQL_RES *res;
	MYSQL_ROW row; 
	int totalrows;

	char consulta[200]="SELECT * FROM chats where(ID_ADMIN=";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	sprintf(str_user_id, "%d", user_id);
	sprintf(str_chat_id, "%d", chat_id);

	if( (strlen(str_user_id) + strlen(str_chat_id) + strlen(consulta)) >= (sizeof(char)*180) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_user_id);
	strcat(consulta, " AND ID =");
	strcat(consulta, str_chat_id);
	strcat(consulta, ");");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	res = mysql_store_result(persistence->mysql);
	
	return (mysql_num_rows(res) > 0)? 1 : 0;
}

int still_users_in_chat(persistence* persistence,int chat_id){
	DEBUG_TRACE_PRINT();
	char str_chat_id[10]; 
	MYSQL_RES *res;
	MYSQL_ROW row;
	int totalrows;

	char consulta[200]="SELECT * FROM users_chats where ((ID_CHAT=";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	sprintf(str_chat_id, "%d", chat_id);

	if( (strlen(str_chat_id) + strlen(consulta)) >= (sizeof(char)*190) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_chat_id);
	strcat(consulta, ") and ( REM_TIME = 0));");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	res = mysql_store_result(persistence->mysql);
	
	return (mysql_num_rows(res) > 0)? 1 : 0;
}


int get_first_users_in_chat(persistence* persistence,int chat_id){
	DEBUG_TRACE_PRINT();

	char str_chat_id[10]; 
	MYSQL_RES *res;
	MYSQL_ROW row;
	int totalrows;

	char consulta[200]="SELECT * FROM users_chats where ((ID_CHAT=";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	sprintf(str_chat_id, "%d", chat_id);

	if((strlen(str_chat_id) + strlen(consulta)) >= (sizeof(char)*180) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_chat_id);
	strcat(consulta, ") and (REM_TIME = 0));");

	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	res = mysql_store_result(persistence->mysql);
	if (mysql_num_rows(res) < 1) {
		DEBUG_FAILURE_PRINTF("The chat have not users");
	}
	
	row = mysql_fetch_row(res);

	return atoi(row[0]);
}


int get_all_chat_info(persistence* persistence,int chat_id, struct soap *soap, psdims__chat_info *chat){
	DEBUG_TRACE_PRINT();

	char str_id[20];
	char str_time[20];
	int resultado;
	int i;
	int j;
	int k;
	int totalrows;
	int numfields;
	char name[50];
	char info[500];
  	MYSQL_RES *res;
  	MYSQL_ROW row;
	
	sprintf(str_id, "%d", chat_id);	
	sprintf(str_time, "%d", 0);

  	char consulta[200]="SELECT users.NAME, chats.DESCRIPTION, chats.READ_TIME " \
			"FROM chats INNER JOIN users on (users.ID = chats.ID_ADMIN) " \
			"where ((chats.ID = "; 	
	
  	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
    	return -1;
  	}
	if( (strlen(str_id) + strlen(consulta)) >= (sizeof(char)*190) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}
 
  	strcat(consulta,str_id);
  	strcat(consulta," ) and (CREATION_TIME >= ");
  	strcat(consulta, str_time);
  	strcat(consulta, "));"); 
 
  	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
    	return -1;
	}

    res = mysql_store_result(persistence->mysql);

	if( mysql_num_rows(res) < 1) {
		DEBUG_FAILURE_PRINTF("The chat id does not exist");
		return -1;
	}

	chat->admin = soap_malloc(soap, strlen(row[0]) + sizeof(char) );
	chat->description = soap_malloc(soap, strlen(row[1]) + sizeof(char));

	chat->chat_id = chat_id;
	strcpy(chat->admin, row[0]);
	strcpy(chat->description, row[1]);
	chat->all_read_timestamp = atoi(row[2]);
	
	get_member_list_chats(persistence, chat_id, 0, soap, &(chat->members));
	
	return 0;
}


int get_file(persistence* persistence, int user_id, int chat_id,char* path, int timestamp){
	DEBUG_TRACE_PRINT();
	char str_id[10],str_time[10],str_chat_id[10];
	MYSQL_RES *res;
	MYSQL_ROW row;  
	// TODO add CREATION_TIME
	char consulta[200]="SELECT FILE_ FROM messages WHERE (";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	sprintf(str_id, "%d", user_id);
	sprintf(str_time, "%d", timestamp);
	sprintf(str_chat_id, "%d", chat_id);

	if((strlen(str_id) + strlen(str_chat_id) + strlen(str_time) + strlen(consulta)) >= (sizeof(char)*150) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, " ID_SENDER ='");
	strcat(consulta, str_id);
	strcat(consulta, " and ID_CHAT = '");
	strcat(consulta, str_chat_id);
	strcat(consulta, " and  CREATION_TIME = ");
	strcat(consulta, str_time);
	strcat(consulta, ");");

	if(mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	res = mysql_store_result(persistence->mysql);
	if (mysql_num_rows(res) < 1) {
		DEBUG_FAILURE_PRINTF("The file does not exist");
		return -1;
	}
	row = mysql_fetch_row(res);

	strcpy(path,row[0]);

	return 0;
}


int message_can_attach(persistence *persistence, int user_id, int chat_id, int msg_timestamp) {
	DEBUG_TRACE_PRINT();
	char str_id[10],str_time[10],str_chat_id[10];
	MYSQL_RES *res;
	MYSQL_ROW row;  

	char consulta[200]="SELECT FILE_ FROM messages WHERE (";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	sprintf(str_id, "%d", user_id);
	sprintf(str_time, "%d", msg_timestamp);
	sprintf(str_chat_id, "%d", chat_id);

	if((strlen(str_id) + strlen(str_chat_id) + strlen(str_time) + strlen(consulta)) >= (sizeof(char)*150) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, " ID_SENDER = ");
	strcat(consulta, str_id);
	strcat(consulta, " and ID_CHAT = ");
	strcat(consulta, str_chat_id);
	strcat(consulta, " and CREATION_TIME = ");
	strcat(consulta, str_time);
	strcat(consulta, ");");

	if(mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	res = mysql_store_result(persistence->mysql);

	return (mysql_num_rows(res) == 1)? 1 : 0;
}


int message_have_attach(persistence *persistence, int user_id, int chat_id, int msg_timestamp) {
	DEBUG_TRACE_PRINT();
	char str_id[10],str_time[10],str_chat_id[10];
	MYSQL_RES *res;
	MYSQL_ROW row;  

	char consulta[500]="SELECT messages.FILE_ FROM users_chats LEFT JOIN messages " \
			"on (users_chats.ID_CHAT = messages.ID_CHAT and messages.CREATION_TIME >= users_chats.CREATION_TIME) WHERE (";

	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
		return -1;
	}

	sprintf(str_id, "%d", user_id);
	sprintf(str_time, "%d", msg_timestamp);
	sprintf(str_chat_id, "%d", chat_id);

	if((strlen(str_id) + strlen(str_chat_id) + strlen(str_time) + strlen(consulta)) >= (sizeof(char)*450) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, " users_chats.ID_USERS = ");
	strcat(consulta, str_id);
	strcat(consulta, " and messages.ID_CHAT = ");
	strcat(consulta, str_chat_id);
	strcat(consulta, " and messages.CREATION_TIME = ");
	strcat(consulta, str_time);
	strcat(consulta, ");");

	if(mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
		return -1;
	}

	res = mysql_store_result(persistence->mysql);

	return (mysql_num_rows(res) == 1)? 1 : 0;
}


int update_sync(persistence *persistence, int user_id, int chat_id, int read_timestamp) {
	DEBUG_TRACE_PRINT();
	
	int i;
	char str_timestamp[10];
	char str_id[10];
	char str_id_chat[10];
	
	char consulta[200] = "UPDATE users_chats set READ_MSG_TIME = ";
	char consulta2[200] = "UPDATE chats set READ_TIME = ";
	
  	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
    	return -1;
  	}	

	sprintf(str_timestamp, "%d", read_timestamp);
	sprintf(str_id, "%d", user_id);
	sprintf(str_id_chat, "%d", chat_id);
	
	if( (strlen(str_id) + strlen(str_timestamp) + strlen(str_id_chat) + strlen(consulta)) >= (sizeof(char)*150) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}
	if( (strlen(str_id_chat) + strlen(str_timestamp)*2 + strlen(consulta2)) >= (sizeof(char)*150) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_timestamp);
	strcat(consulta, " where ((ID_USERS = ");
	strcat(consulta, str_id);
	strcat(consulta, ") AND (ID_CHAT =");
	strcat(consulta, str_id_chat);
	strcat(consulta, ") AND (READ_MSG_TIME < ");
	strcat(consulta, str_timestamp);
	strcat(consulta, "));");	
	
	strcat(consulta2, str_timestamp);
	strcat(consulta2, " where ID = ");
	strcat(consulta2, str_id_chat);
	strcat(consulta2, " and ID NOT IN (select ID_CHAT from users_chats where READ_MSG_TIME < ");
	strcat(consulta2, str_timestamp);
	strcat(consulta2, " AND REM_TIME = 0);");
	
  	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
    	return -1;
	}
  	if( mysql_query(persistence->mysql, consulta2) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
    	return -1;
	}	
	
	return 0;
}


int get_notif_chats_with_messages(persistence *persistence, int user_id, int timestamp, struct soap *soap, psdims__notif_chat_list *chat_list) {

	char str_id[20];
	char str_time[20];
	MYSQL_RES *res;
	MYSQL_ROW row;  
	int totalrows;
	int i;

	char consulta[500]="select DISTINCT users_chats.ID_CHAT from users_chats " \
			"INNER JOIN messages on (users_chats.ID_CHAT = messages.ID_CHAT) " \
			"where (users_chats.REM_TIME = 0) and (messages.CREATION_TIME > " \
			"users_chats.CREATION_TIME) and (users_chats.ID_USERS = "; 		
	
  	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
    	return -1;
  	}
	
	sprintf(str_id, "%d", user_id);	
	sprintf(str_time, "%d",timestamp);	
	
	if( (strlen(str_id) + strlen(str_time) + strlen(consulta)) >= (sizeof(char)*450) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_id);
	strcat(consulta, ") and (messages.CREATION_TIME >= ");
	strcat(consulta, str_time);
	strcat(consulta, ");");
 
  	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
    	return -1;
	}

    res = mysql_store_result(persistence->mysql);
    totalrows = mysql_num_rows(res);

	chat_list->chat = soap_malloc(soap ,sizeof(psdims__notif_chat_info)*totalrows);
	chat_list->__sizenelems = totalrows;

	for( i = 0 ; i < totalrows ; i++ ){
		row = mysql_fetch_row(res);
		chat_list->chat[i].chat_id = atoi(row[0]);
	}
	
	return 0;
}


int get_notif_chats_read_times(persistence *persistence, int user_id, struct soap *soap, psdims__notif_chat_list *chat_list) {

	char str_id[20];
	MYSQL_RES *res;
	MYSQL_ROW row;  
	int totalrows;
	int i;

	char consulta[500]="select chats.ID, chats.READ_TIME from users_chats LEFT JOIN chats " \
			"on (users_chats.ID_CHAT = chats.ID) where (users_chats.ID_USERS = ";
	
  	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
    	return -1;
  	}
	
	sprintf(str_id, "%d", user_id);
	
	if( (strlen(str_id) + strlen(consulta)) >= (sizeof(char)*450) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_id);
	strcat(consulta, ");");
 
  	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
    	return -1;
	}

    res = mysql_store_result(persistence->mysql);
    totalrows = mysql_num_rows(res);

	chat_list->chat = soap_malloc(soap ,sizeof(psdims__notif_chat_info)*totalrows);
	chat_list->__sizenelems = totalrows;

	for( i = 0 ; i < totalrows ; i++ ){
		row = mysql_fetch_row(res);
		chat_list->chat[i].chat_id = atoi(row[0]);
		chat_list->chat[i].timestamp = atoi(row[1]);
	}
	
	return 0;
}


int get_notif_friend_requests(persistence *persistence, int user_id, int timestamp, struct soap *soap, psdims__notif_friend_list *request_list) {
	DEBUG_TRACE_PRINT();

	char str_id[20];
	char str_time[20];
	MYSQL_RES *res;
	MYSQL_ROW row;  
	int totalrows;
	int i;

  	char consulta[500]="select users.NAME, friends_request.CREATION_TIME from users INNER JOIN " \
		"friends_request on (users.ID = friends_request.ID1) where " \
		"(friends_request.ID2_request = '"; 
	
	
	sprintf(str_id, "%d", user_id);	
	sprintf(str_time, "%d",timestamp);	
	
  	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
    	return -1;
  	}
	if( (strlen(str_id) + strlen(str_time) + strlen(consulta)) >= (sizeof(char)*450) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

    strcat(consulta,str_id);
  	strcat(consulta,"') AND (CREATION_TIME >= ");	
	strcat(consulta,str_time);
	strcat(consulta,");");
 
  	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
    	return -1;
	}

    res = mysql_store_result(persistence->mysql);
    totalrows = mysql_num_rows(res);

	request_list->user = soap_malloc(soap,sizeof(psdims__notif_friend_info)*totalrows);
	request_list->__sizenelems = totalrows;

	for( i = 0 ; i < totalrows ; i++ ){
		row = mysql_fetch_row(res);
		
		request_list->user[i].name.string = soap_malloc(soap, strlen(row[0]) + sizeof(char) );
		
		strcpy(request_list->user[i].name.string, row[0]);
		request_list->user[i].send_date = atoi(row[1]);
	}
	
	return 0;
}


int get_notif_chat_members(persistence *persistence, int user_id, int timestamp, struct soap *soap, psdims__notif_chat_member_list *member_list) {

	char str_id[20];
	char str_time[20];
	MYSQL_RES *res;
	MYSQL_ROW row;  
	int totalrows;
	int i;

	char consulta[500]="select users.NAME, member.ID_CHAT, member.CREATION_TIME from users_chats as member  " \
			"INNER JOIN users_chats as user on (member.ID_CHAT = user.ID_CHAT) " \
			"INNER JOIN users on (member.ID_USERS = users.ID)" \
			"where (user.ID_USERS =  ";

	
  	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
    	return -1;
  	}
	
	sprintf(str_id, "%d", user_id);	
	sprintf(str_time, "%d",timestamp);	
	
	if( (strlen(str_id) + strlen(str_time) + strlen(consulta)) >= (sizeof(char)*450) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_id);
	strcat(consulta, ") and (member.CREATION_TIME >= ");
	strcat(consulta, str_time);
	strcat(consulta, ") and (member.REM_TIME = 0);");
 
  	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
    	return -1;
	}

    res = mysql_store_result(persistence->mysql);
    totalrows = mysql_num_rows(res);

	member_list->member = soap_malloc(soap, sizeof(psdims__notif_member_info)*totalrows);
	member_list->__sizenelems = totalrows;

	for( i = 0 ; i < totalrows ; i++ ){
		row = mysql_fetch_row(res);
		member_list->member[i].name.string = soap_malloc(soap, strlen(row[0]) + sizeof(char));
		
		strcpy(member_list->member[i].name.string, row[0]);
		member_list->member[i].chat_id = atoi(row[1]);
		member_list->member[i].timestamp = atoi(row[2]);
	}
	
	return 0;
}


int get_notif_chat_rem_members(persistence *persistence, int user_id, int timestamp, struct soap *soap, psdims__notif_chat_member_list *member_list) {
	char str_id[20];
	char str_time[20];
	MYSQL_RES *res;
	MYSQL_ROW row;  
	int totalrows;
	int i;

	char consulta[500]="select users.NAME, member.ID_CHAT, member.CREATION_TIME from users_chats as member  " \
			"INNER JOIN users_chats as user on (member.ID_CHAT = user.ID_CHAT) " \
			"INNER JOIN users on (member.ID_USERS = users.ID)" \
			"where (user.ID_USERS =  ";

	
  	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
    	return -1;
  	}
	
	sprintf(str_id, "%d", user_id);	
	sprintf(str_time, "%d",timestamp);	
	
	if( (strlen(str_id) + strlen(str_time) + strlen(consulta)) >= (sizeof(char)*450) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_id);
	strcat(consulta, ") and (member.REM_TIME >= ");
	strcat(consulta, str_time);
	strcat(consulta, ");");
 
  	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
    	return -1;
	}

    res = mysql_store_result(persistence->mysql);
    totalrows = mysql_num_rows(res);

	member_list->member = soap_malloc(soap, sizeof(psdims__notif_member_info)*totalrows);
	member_list->__sizenelems = totalrows;

	for( i = 0 ; i < totalrows ; i++ ){
		row = mysql_fetch_row(res);
		member_list->member[i].name.string = soap_malloc(soap, strlen(row[0]) + sizeof(char));
		
		strcpy(member_list->member[i].name.string, row[0]);
		member_list->member[i].chat_id = atoi(row[1]);
		member_list->member[i].timestamp = atoi(row[2]);
	}
	
	return 0;
}

int get_notif_chat_admins(persistence *persistence, int user_id, int timestamp, struct soap *soap, psdims__notif_chat_member_list *member_list) {
	char str_id[20];
	char str_time[20];
	MYSQL_RES *res;
	MYSQL_ROW row;  
	int totalrows;
	int i;

	char consulta[500]="select users.NAME, chat_admin.ID, chat_admin.ADMIN_TIME from chats as chat_admin  " \
			"INNER JOIN users_chats as user on (chat_admin.ID = user.ID_CHAT) " \
			"INNER JOIN users on (chat_admin.ID_ADMIN = users.ID)" \
			"where (user.ID_USERS =  ";

	
  	if (persistence->mysql == NULL) {	
		DEBUG_FAILURE_PRINTF("DataBase is not initialized");
    	return -1;
  	}
	
	sprintf(str_id, "%d", user_id);	
	sprintf(str_time, "%d",timestamp);	
	
	if( (strlen(str_id) + strlen(str_time) + strlen(consulta)) >= (sizeof(char)*450) ) {
		DEBUG_FAILURE_PRINTF("Query is too long to fit");
		return -1;
	}

	strcat(consulta, str_id);
	strcat(consulta, ") and (chat_admin.ADMIN_TIME >= ");
	strcat(consulta, str_time);
	strcat(consulta, ");");
 
  	if( mysql_query(persistence->mysql, consulta) ) {
		DEBUG_FAILURE_PRINTF("Query error");
		DEBUG_FAILURE_PRINTF("MYSQL_ERROR: %s", mysql_error(persistence->mysql)); 
    	return -1;
	}

    res = mysql_store_result(persistence->mysql);
    totalrows = mysql_num_rows(res);

	member_list->member = soap_malloc(soap, sizeof(psdims__notif_member_info)*totalrows);
	member_list->__sizenelems = totalrows;

	for( i = 0 ; i < totalrows ; i++ ){
		row = mysql_fetch_row(res);
		member_list->member[i].name.string = soap_malloc(soap, strlen(row[0]) + sizeof(char));
		
		strcpy(member_list->member[i].name.string, row[0]);
		member_list->member[i].chat_id = atoi(row[1]);
		member_list->member[i].timestamp = atoi(row[2]);
	}
	
	return 0;
}
