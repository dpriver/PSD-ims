/*******************************************************************************
 *	psd_ims_server.c
 *
 *  Server network management
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

#include "soapH.h"
#include "psdims.nsmap"
#include "persistence.h"
#include <pthread.h>
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "debug_def.h"

struct server {
	persistence *persistence;
	struct soap soap;
	int n_alive_threads;
} server;


/*
 *
 *
 */
void end_soap_connection(struct soap *soap) {
	soap_end((struct soap*)soap);
	soap_done((struct soap*)soap);
}


void *thread_serve_request(void *soap) {
	DEBUG_TRACE_PRINT();

	// INCREMENT server.n_alive_threads

	pthread_detach(pthread_self());
	soap_serve((struct soap*)soap);

	// end the connection and free the resources
	end_soap_connection((struct soap*)soap);
	free(soap);

	// DECREMENT server.n_alive_threads

	// Now it could report to the main thread via a pipe or so that it has finished, doing so
		// when the main thread is listenning to connections, ignores the pipe
		// when the main thread has to finish and is waiting for the threads, instead of
		// polling the number of threads, keeps listening the pipe, and anytime a thread finish, polls
		// the number of threads

	return NULL;
}



/*
 *
 * Returns 0 or -1 if fails
 */
int init_server(int bind_port, char persistence_user[], char persistence_pass[]) {
	DEBUG_TRACE_PRINT();

	SOAP_SOCKET m;
	
	server.persistence = init_persistence(persistence_user, persistence_pass);
	if (server.persistence == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not init persistence");
		return -1;
	}

	DEBUG_INFO_PRINTF("Init soap...");
	soap_init(&server.soap);
	
	server.soap.send_timeout = 60; 			// 60 secs
	server.soap.recv_timeout = 60;			// 60 secs
	server.soap.accept_timeout = 3600;	// after 3600 secs of inactivity the server stops
	server.soap.max_keep_alive = 100;		// max keep_alive sequence
	server.n_alive_threads = 0;

	m = soap_bind(&server.soap, NULL, bind_port, 100);

	if (!soap_valid_socket(m)) {
		soap_print_fault(&server.soap, stderr);
		return -1;
	}

	return 0;
}


/*
 *
 *
 */
void free_server() {
	DEBUG_TRACE_PRINT();
	// finish the "list" soap connection
	end_soap_connection(&server.soap);
	// wait until (n_alive_threads == 0)
	free_persistence(server.persistence);
}


/*
 *
 * Returns 0 or -1 if fails
 */
int listen_connection () {
	DEBUG_TRACE_PRINT();

	SOAP_SOCKET s;

	s = soap_accept(&server.soap);
	if (!soap_valid_socket(s)) {
		if(server.soap.errnum) {
			soap_print_fault(&server.soap, stderr); 
			return -1;
		}
		DEBUG_INFO_PRINTF("Server timed out");
		return -1;
	}
	
	// Execute invoked operation
	soap_serve(&server.soap);

	// Clean up!
	soap_end(&server.soap);

	return 0;
}


/*
 *
 * Returns 0 or -1 if fails
 */
int mthread_listen_connection () {
	DEBUG_TRACE_PRINT();

	SOAP_SOCKET s;
	pthread_t tid;
	struct soap *tsoap;

	s = soap_accept(&server.soap);
	if (!soap_valid_socket(s)) {
		if(server.soap.errnum) {
			soap_print_fault(&server.soap, stderr); 
			return -1;
		}
		DEBUG_INFO_PRINTF("Server timed out");
		return -1;
	}

	// The threads are not controlled, there may potencially be an infinite number of them
	// at the same time (resource problems... )
	tsoap = soap_copy(&server.soap);	//make a safe copy
	if (!tsoap) {
		DEBUG_FAILURE_PRINTF("Could not copy the soap struct");
		return -1;
	}
	
	pthread_create(&tid, NULL, (void*(*)(void*))thread_serve_request, (void*)tsoap);

	return 0;
}


/* =========================================================================
 *  Gsoap handlers
 * =========================================================================*/

/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__user_register(struct soap *soap,psdims__register_info *user_info, int *ERRCODE){
	DEBUG_TRACE_PRINT();
	*ERRCODE = 1;

	if ( (user_info->name == NULL) || (user_info->name == NULL) || (user_info->name == NULL) ) {
		DEBUG_FAILURE_PRINTF("Some fields are empty");
		return SOAP_USER_ERROR;
	}

	DEBUG_INFO_PRINTF("Registering: name:%s pass:%s", user_info->name, user_info->password);

	if( add_user(server.persistence, user_info->name, user_info->password, user_info->information) != 0 ) {
		DEBUG_FAILURE_PRINTF("Failed to add user");
		return SOAP_USER_ERROR;
	}

	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__user_unregister(struct soap *soap, psdims__login_info *login, int *ERRCODE){
	DEBUG_TRACE_PRINT();
	*ERRCODE = 1;

	if(user_exist(server.persistence,login->name)!=1)
		return SOAP_USER_ERROR;

 	if(strcmp(login->password,get_user_pass(server.persistence,login->name))!=0){
		return SOAP_USER_ERROR;
	}

	DEBUG_INFO_PRINTF("Unregistering: name:%s ", login->name);

	if( del_user(server.persistence, login->name) != 0 ) {
		DEBUG_FAILURE_PRINTF("Failed to delete user");
		return SOAP_USER_ERROR;
	}

	return SOAP_OK;
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__get_user(struct soap *soap, psdims__login_info *login, psdims__user_info *user) {
	DEBUG_TRACE_PRINT();
	if(user_exist(server.persistence,login->name)!=1)
		return SOAP_USER_ERROR;

 	if(strcmp(login->password,get_user_pass(server.persistence,login->name))!=0){
		return SOAP_USER_ERROR;
	}
	user->name = soap_malloc(soap, strlen(login->name) + sizeof(char));
	user->information = soap_malloc(soap, sizeof(char)*200);

	strcpy(user->name,login->name);  
	get_user_info(server.persistence,get_user_id(server.persistence,login->name),user->information);

	// Buscar el usuario mediante persistence
		//[SI no se encuentra]
			//soap->user = (char*)malloc(sizeof(char)*200);
			//strcpy((char *)soap->user, "Incorrect login credentials");
			//return SOAP_USER_ERROR
	return SOAP_OK;
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__get_friends(struct soap *soap,psdims__login_info *login, int timestamp, psdims__user_list *friends){
	DEBUG_TRACE_PRINT();
	int id;

	if(user_exist(server.persistence,login->name)!=1)
		return SOAP_USER_ERROR;

 	if(strcmp(login->password,get_user_pass(server.persistence,login->name))!=0){
		return SOAP_USER_ERROR;
	}

	id=get_user_id(server.persistence,login->name);
	

  	if(get_list_friends(server.persistence,id,timestamp, soap, friends)!=0){
		return SOAP_USER_ERROR;
	}

	return SOAP_OK;
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__get_chats(struct soap *soap,psdims__login_info *login, int timestamp, psdims__chat_list *chats){
	DEBUG_TRACE_PRINT();
	int id;

	if(user_exist(server.persistence,login->name)!=1)
		return SOAP_USER_ERROR;

 	if(strcmp(login->password,get_user_pass(server.persistence,login->name))!=0){
		return SOAP_USER_ERROR;
	}

	id=get_user_id(server.persistence,login->name);
	

	if(get_list_chats(server.persistence,id,timestamp,soap, chats)!=0){
		return SOAP_USER_ERROR;
	}

	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__get_chat_info(struct soap *soap, psdims__login_info *login, int chat_id, psdims__chat_info *chat) {
	DEBUG_TRACE_PRINT();
	int id_user;

	if(user_exist(server.persistence,login->name)!=1)
		return SOAP_USER_ERROR;

 	if(strcmp(login->password,get_user_pass(server.persistence,login->name))!=0){
		return SOAP_USER_ERROR;
	}

	if(chat_exist(server.persistence,chat_id)!=1)
		return SOAP_USER_ERROR;

	id_user=get_user_id(server.persistence,login->name);

	if(get_all_chat_info(server.persistence,chat_id,soap,chat)!=1)
		return SOAP_USER_ERROR;

	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__get_chat_messages(struct soap *soap,psdims__login_info *login, int chat_id, int timestamp, psdims__message_list *messages){
	DEBUG_TRACE_PRINT();
	int id_user;

	if(user_exist(server.persistence,login->name)!=1)
		return SOAP_USER_ERROR;

 	if(strcmp(login->password,get_user_pass(server.persistence,login->name))!=0){
		return SOAP_USER_ERROR;
	}

	if(chat_exist(server.persistence,chat_id)!=1)
		return SOAP_USER_ERROR;

	id_user=get_user_id(server.persistence,login->name);

	if(exist_user_in_chat(server.persistence,id_user,chat_id)!=1)
		return SOAP_USER_ERROR;

	if(get_list_messages(server.persistence,chat_id,timestamp, soap, messages)!=0)
		return SOAP_USER_ERROR;

	return SOAP_OK; 
}


// Get the file attached to msd_id
int psdims__get_attachment(struct soap *soap, psdims__login_info *login, int chat_id, int msg_timestamp, psdims__file *file) {
	DEBUG_TRACE_PRINT();
	DEBUG_FAILURE_PRINTF("Not implemented");
	return SOAP_USER_ERROR;
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__get_pending_notifications(struct soap *soap,psdims__login_info *login, int timestamp, psdims__notifications *notifications){
	DEBUG_TRACE_PRINT();
	DEBUG_FAILURE_PRINTF("Not implemented");
	// Si el usuario y el user no existen, salir
		// return SOAP_USER_ERROR
	// obtener el id del usuario
	// buscar todas las notificationes pendientes del usuario "id"
	return SOAP_USER_ERROR; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__create_chat(struct soap *soap, psdims__login_info *login, psdims__new_chat *new_chat, int *chat_id) {
	DEBUG_TRACE_PRINT();
	int timestamp;
	int id_user;
	timestamp = time(NULL);

	if(user_exist(server.persistence,login->name)!=1)
		return SOAP_USER_ERROR;

 	if(strcmp(login->password,get_user_pass(server.persistence,login->name))!=0){
		return SOAP_USER_ERROR;
	}

	id_user=get_user_id(server.persistence,login->name);

	if(add_chat(server.persistence, id_user, new_chat->description, timestamp, chat_id)!=0)
		return SOAP_USER_ERROR;

	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__add_member(struct soap *soap, psdims__login_info *login, char *name, int chat_id, int *ERRCODE) {
	DEBUG_TRACE_PRINT();
	
	int id_user;

	if(user_exist(server.persistence,login->name)!=1)
		return SOAP_USER_ERROR;

 	if(strcmp(login->password,get_user_pass(server.persistence,login->name))!=0){
		return SOAP_USER_ERROR;
	}

	if(chat_exist(server.persistence,chat_id)!=1)
		return SOAP_USER_ERROR;

	id_user=get_user_id(server.persistence,name);

    if(exist_user_in_chat(server.persistence,id_user,chat_id)==1){
		printf("Ya existe el usuario en el chat\n");
		return SOAP_USER_ERROR;
	}

	if(add_user_chat(server.persistence,id_user,chat_id,0)!=0)
		return SOAP_USER_ERROR;

	return SOAP_OK;  
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__quit_from_chat(struct soap *soap, psdims__login_info *login, int chat_id, int *ERRCODE) {
	DEBUG_TRACE_PRINT();
	
	int id_user,first_user;

	if(user_exist(server.persistence,login->name)!=1)
		return SOAP_USER_ERROR;

 	if(strcmp(login->password,get_user_pass(server.persistence,login->name))!=0){
		return SOAP_USER_ERROR;
	}

	if(chat_exist(server.persistence,chat_id)!=1)
		return SOAP_USER_ERROR;

	id_user=get_user_id(server.persistence,login->name);

	if(exist_user_in_chat(server.persistence,id_user,chat_id)!=1)
		return SOAP_USER_ERROR;

	if(del_user_chat(server.persistence,id_user,chat_id)!=0)	
		return SOAP_USER_ERROR;

	if(still_users_in_chat(server.persistence,chat_id)==1){
		if(is_admin(server.persistence,id_user,chat_id)==1){
			if((first_user=get_first_users_in_chat(server.persistence,chat_id))==1)
				return SOAP_USER_ERROR;
			if(change_admin(server.persistence,first_user,chat_id)==1)
				return SOAP_USER_ERROR;
		}
	}
	else{
		if(del_chat(server.persistence,chat_id)!=0)	
			return SOAP_USER_ERROR;
	}


	return SOAP_OK;  
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__send_message(struct soap *soap,psdims__login_info *login, int chat_id,  psdims__message_info *message, int *timestamp){
	DEBUG_TRACE_PRINT();
	int id_user;

	if(user_exist(server.persistence,login->name)!=1)
		return SOAP_USER_ERROR;

 	if(strcmp(login->password,get_user_pass(server.persistence,login->name))!=0){
		return SOAP_USER_ERROR;
	}

	id_user=get_user_id(server.persistence,login->name);
	*timestamp = time(NULL);

	if(exist_user_in_chat(server.persistence,id_user,chat_id)!=1)
		return SOAP_USER_ERROR;
	
	if( send_messages(server.persistence,chat_id, *timestamp, message)!=0)
		return SOAP_USER_ERROR;

	return SOAP_OK; 
}


// Send a file to attach msd_id
int psdims__send_attachment(struct soap *soap, psdims__login_info *login, int chat_id, int msg_timestamp, psdims__file *file, int *ERRCODE) {
	DEBUG_TRACE_PRINT();
	DEBUG_FAILURE_PRINTF("Not implemented");
	return SOAP_USER_ERROR;
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__send_friend_request(struct soap *soap,psdims__login_info *login, char* request_name, int *timestamp){
	DEBUG_TRACE_PRINT();
	int id_user,id_request_name;

	if(user_exist(server.persistence,login->name)!=1)
		return SOAP_USER_ERROR;

 	if(strcmp(login->password,get_user_pass(server.persistence,login->name))!=0){
		return SOAP_USER_ERROR;
	}

	id_user=get_user_id(server.persistence,login->name);
	id_request_name=get_user_id(server.persistence,request_name);
	*timestamp = time(NULL);

	if(exist_friendly(server.persistence,id_user,id_request_name)!=0){
		DEBUG_FAILURE_PRINTF("Ya son amigos\n");
		return SOAP_USER_ERROR;
	}

	if(exist_request(server.persistence,id_user,id_request_name)!=0){
		DEBUG_FAILURE_PRINTF("Ya existe una petición de amistad\n");
		return SOAP_USER_ERROR;
	}

	if(send_request(server.persistence,id_user, id_request_name, *timestamp)!=0)
		return SOAP_USER_ERROR;

	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__accept_request(struct soap *soap,psdims__login_info *login, char *request_name, int *timestamp){
	DEBUG_TRACE_PRINT();
	int id_user,id_request_name;

	if(user_exist(server.persistence,login->name)!=1)
		return SOAP_USER_ERROR;

 	if(strcmp(login->password,get_user_pass(server.persistence,login->name))!=0)
		return SOAP_USER_ERROR;

	id_user=get_user_id(server.persistence,login->name);
	id_request_name=get_user_id(server.persistence,request_name);
	*timestamp = time(NULL);

	if(exist_request(server.persistence,id_user,id_request_name)==0){
		DEBUG_FAILURE_PRINTF("No existe una petición de amistad\n");
		return SOAP_USER_ERROR;
	}
    
    if(accept_friend_request(server.persistence,id_user, id_request_name, *timestamp)!=0)
		return SOAP_USER_ERROR;

	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__decline_request(struct soap *soap, psdims__login_info *login, char *request_name, int *timestamp){
	DEBUG_TRACE_PRINT();
	int id_user,id_request_name;

	if(user_exist(server.persistence,login->name)!=1)
		return SOAP_USER_ERROR;

 	if(strcmp(login->password,get_user_pass(server.persistence,login->name))!=0)
		return SOAP_USER_ERROR;

	id_user=get_user_id(server.persistence,login->name);
	id_request_name=get_user_id(server.persistence,request_name);

	if(exist_request(server.persistence,id_user,id_request_name)==0){
		DEBUG_FAILURE_PRINTF("No existe una petición de amistad\n");
		return SOAP_USER_ERROR;
	}

	if(decline_friend_request(server.persistence,id_user,id_request_name)!=0)
		return SOAP_USER_ERROR;

	*timestamp = time(NULL);
	return SOAP_OK; 
}

