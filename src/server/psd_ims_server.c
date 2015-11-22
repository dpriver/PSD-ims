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
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>

#include "debug_def.h"

struct server {
	persistence *persistence;
	struct soap soap;
} server;


int init_server(int bind_port, char persistence_user[], char persistence_pass[]) {
	DEBUG_TRACE_PRINT();

	server.persistence = init_persistence(persistence_user, persistence_pass);

	if (server.persistence == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not init persistence");
		return -1;
	}

	DEBUG_INFO_PRINTF("Init soap...");
	soap_init(&server.soap);

	if (soap_bind(&server.soap, NULL, bind_port, 100) < 0) {
		soap_print_fault(&server.soap, stderr);
		return -1;
	}

	return 0;
}


void free_server() {
	soap_end(&server.soap);
	soap_done(&server.soap);
	free_persistence(server.persistence);
}


int listen_connection () {
	DEBUG_TRACE_PRINT();
	if (soap_accept(&server.soap) < 0) {
		soap_print_fault(&server.soap, stderr); 
		return -1;
	}

	// Execute invoked operation
	soap_serve(&server.soap);

	// Clean up!
	soap_end(&server.soap);
}


int psdims__user_register(struct soap *soap,psdims__register_info *user_info, int *ERRCODE){
	*ERRCODE = 10;

	if ( (user_info->name == NULL) || (user_info->name == NULL) || (user_info->name == NULL) ) {
		DEBUG_FAILURE_PRINTF("Some fields are empty");
		return SOAP_USER_ERROR;
	}

	DEBUG_INFO_PRINTF("Agregando usuario a la base de datos:");
	DEBUG_INFO_PRINTF("NAME: %s", user_info->name);
	DEBUG_INFO_PRINTF("PASS: %s", user_info->password);
	DEBUG_INFO_PRINTF("INFO: %s", user_info->information);

	if( add_user(server.persistence, user_info->name, user_info->password, user_info->information) != 0 ) {
		DEBUG_FAILURE_PRINTF("Failed to add user");
		return SOAP_USER_ERROR;
	}

	return SOAP_OK; 
}


int psdims__user_unregister(struct soap *soap, psdims__login_info *login, int *ERRCODE){
	*ERRCODE = 11;

	if( del_user(server.persistence, login->name) != 0 ) {
		DEBUG_FAILURE_PRINTF("Failed to delete user");
		return SOAP_USER_ERROR;
	}

	return SOAP_OK;
}


int psdims__get_user(struct soap *soap, psdims__login_info *login, psdims__user_info *user_info) {
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


int psdims__get_friends(struct soap *soap,psdims__login_info *login, psdims__user_list *friends){
	return SOAP_OK; 
}


int psdims__get_chats(struct soap *soap,psdims__login_info *login, psdims__chat_list *chats){
	return SOAP_OK; 
}


int psdims__get_chat_messages(struct soap *soap,psdims__login_info *login, int chat_id, psdims__message_list *messages){
	return SOAP_OK; 
}


int psdims__get_pending_notifications(struct soap *soap,psdims__login_info *login, psdims__notification_list *notifications){
	return SOAP_OK; 
}


int psdims__send_message(struct soap *soap,psdims__login_info *login, psdims__message_info *message){
	return SOAP_OK; 
}

// enviar solicitud de amistad a usuario
int psdims__send_friend_request(struct soap *soap,psdims__login_info *login, char* request_name, int *ERRCODE){
    *ERRCODE=1;
	return SOAP_OK; 
}

// aceptar solicitud de amistad
int psdims__accept_request(struct soap *soap,psdims__login_info *login, char *request_name, int *ERRCODE){
	*ERRCODE=1;
	return SOAP_OK; 
}

// rechazar solicitud de amistad
int psdims__decline_request(struct soap *soap,psdims__login_info *login, char *request_name, int *ERRCODE){
	*ERRCODE=1;
	return SOAP_OK; 
}

