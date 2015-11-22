/*******************************************************************************
 *	server.c
 *
 *  server main file
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
#include <mysql.h>
#include "persistence.h"
#include <stdio.h>
#include <stdlib.h>

MYSQL* bd;

int main( int argc, char **argv) {

	int m, s;
	struct soap soap;

	if (argc < 4) {
		printf("Usage: %s <port> <bd_user> <bd_pass>\n", argv[0]);
		exit(-1);
	}	

	// Init environment
	soap_init(&soap);
  bd = init_bd(argv[2], argv[3] ,"PSD");

	if( bd == NULL ) {
		printf("Faied to initialize DataBase\n");
		return -1;
	}

	// Bind to the specified port	
	m = soap_bind(&soap, NULL, atoi(argv[1]), 100);

	// Check result of binding		
	if (m < 0) {
		soap_print_fault(&soap, stderr); exit(-1); 
	}

	// Listen to next connection
	while (1) { 

		// accept
		s = soap_accept(&soap);    

		if (s < 0) {
			soap_print_fault(&soap, stderr); exit(-1);
		}

		// Execute invoked operation
		soap_serve(&soap);

		// Clean up!
		soap_end(&soap);
	}

  return 0;
}

//(struct soap *soap, int a, int b, int *res)
int psdims__user_register(struct soap *soap,psdims__register_info *user_info, int *ERRCODE){
	*ERRCODE = 10;
	if ( (user_info->name == NULL) || (user_info->name == NULL) || (user_info->name == NULL) ) {
		printf("Some fields are empty\n");
		return -1;
	}
	printf("Agregando usuario a la base de datos:\n");
	printf("NAME: %s\n", user_info->name);
	printf("PASS: %s\n", user_info->password);
	printf("INFO: %s\n", user_info->information);

	if( add_user(bd, user_info->name, user_info->password, user_info->information) != 0 ) {
		printf("Failed to add_user\n");
	}
	return SOAP_OK; 
}

// borrar user
int psdims__user_unregister(struct soap *soap, psdims__login_info *login, int *ERRCODE){
	*ERRCODE = 11;
	del_user(bd,login->name);
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

// get chat list
int psdims__get_chats(struct soap *soap,psdims__login_info *login, psdims__chat_list *chats){
	return SOAP_OK; 
}

// get messages from chat
int psdims__get_chat_messages(struct soap *soap,psdims__login_info *login, int chat_id, psdims__message_list *messages){
	return SOAP_OK; 
}

// get pending notifications
int psdims__get_pending_notifications(struct soap *soap,psdims__login_info *login, psdims__notification_list *notifications){
	return SOAP_OK; 
}

// Send message
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
