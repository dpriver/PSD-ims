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
//#include "psdims.nsmap"
#include "persistence.h"
#include <pthread.h>
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>

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
	*ERRCODE = 10;

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
	*ERRCODE = 11;

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
int psdims__get_user(struct soap *soap, psdims__login_info *login, psdims__user_info *user_info) {
	if(user_exist(server.persistence,login->name)!=1)
		return SOAP_USER_ERROR;

 	if(strcmp(login->password,get_user_pass(server.persistence,login->name))!=0){
		return SOAP_USER_ERROR;
	}
	user_info->name = malloc(sizeof(char)*50);
	strcpy(user_info->name,login->name);
    
	// Buscar el usuario mediante persistence
		// [si se encuentra]
			//user_info->name = malloc(sizeof(char)*10);
			//user_info->information = malloc(sizeof(char)*50);
			//strcpy(user_info->name, <name>);
			//strcpy(user_info->information, <information>); 
			//return SOAP_OK
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
int psdims__get_friends(struct soap *soap,psdims__login_info *login, psdims__user_list *friends){
	// Si el usuario y el user no existen, salir
		// return SOAP_USER_ERROR
	// obtener el id del usuario
	// buscar todos los usuario que sean amigos de "id"

	return SOAP_OK;
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__get_chats(struct soap *soap,psdims__login_info *login, psdims__chat_list *chats){
	// Si el usuario y el user no existen, salir
		// return SOAP_USER_ERROR
	// obtener el id del usuario
	// buscar todos los chats en os que forme parte "id"

	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__get_chat_messages(struct soap *soap,psdims__login_info *login, int chat_id, psdims__message_list *messages){
	// Si el usuario y el user no existen, salir
		// return SOAP_USER_ERROR
	// obtener el id del usuario
	// buscar todos los mensajes del chat "chat_id"

	// Es necesario un mecanismo para obtener solo los mensajes pendientes del usuario
	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__get_pending_notifications(struct soap *soap,psdims__login_info *login, psdims__notification_list *notifications){
	// Si el usuario y el user no existen, salir
		// return SOAP_USER_ERROR
	// obtener el id del usuario
	// buscar todas las notificationes pendientes del usuario "id"
	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__send_message(struct soap *soap,psdims__login_info *login, int chat_id,  psdims__message_info *message, int *ERRCODE){
	// Si el usuario y el user no existen, salir
		// return SOAP_USER_ERROR
	// obtener el id del usuario
	// Agregar el mensaje al chat indicado (TODO Falta enviar chat_id)
	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__send_friend_request(struct soap *soap,psdims__login_info *login, char* request_name, int *ERRCODE){
	// Si el usuario y el user no existen, salir
		// return SOAP_USER_ERROR
	// obtener el id del usuario
	// Comprobar que el nombre coincida con algún usuario y estos no sean ya amigos
	// agregar la petición de amistad y la notificación para el otro usuario
    *ERRCODE=1;
	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__accept_request(struct soap *soap,psdims__login_info *login, char *request_name, int *ERRCODE){
	// Si el usuario y el user no existen, salir
		// return SOAP_USER_ERROR
	// obtener el id del usuario
	// Comprobar que el nombre coincida con algún usuario y haya una petición pendiente
	// en la que el usuario sea el receptor
	// borrar la peticion y agregarles como amigos
	*ERRCODE=1;
	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__decline_request(struct soap *soap,psdims__login_info *login, char *request_name, int *ERRCODE){
	// Si el usuario y el user no existen, salir
		// return SOAP_USER_ERROR
	// obtener el id del usuario
	// Comprobar que el nombre coincida con algún usuario y haya una petición pendiente
	// en la que el usuario sea el receptor
	// borrar la petición
	*ERRCODE=1;
	return SOAP_OK; 
}

