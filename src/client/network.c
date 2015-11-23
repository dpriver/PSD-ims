/*******************************************************************************
 *	network.c
 *
 *  client network management
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
#include "network.h"
#include "psd_ims_client.h"


#include "debug_def.h"

#ifdef DEBUG
#include "leak_detector_c.h"
#endif


network *init_network(char *serverURL) {
	network *new_network;
	if( (new_network = malloc( sizeof(network) )) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate network estructure");
		return NULL;
	}
	if( (new_network->serverURL = malloc( sizeof(char)*(strlen(serverURL)+1) )) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate network estructure");
		free(new_network);
		return NULL;
	}
	strcpy(new_network->serverURL,serverURL);
	
	soap_init(&new_network->soap);
	return new_network;
}


void *free_network(network *network) {
	soap_end(&network->soap);
	soap_done(&network->soap);

	free(network->serverURL);
	free(network);
}


int recv_notifications(network *network, psd_ims_client *client) {
	// call the gsoap method
	// add the new notifications to the client struct
	return -1;
}


int recv_pending_messages(network *network, psd_ims_client *client, int chat_id) {
	// call the gsoap method
	// add the new messages to the client struct
	return -1;
}


int recv_new_chats(network *network, psd_ims_client *client) {
	// call the gsoap method
	// add the new chats to the client struct
	return -1;
}


int send_message(network *network, psd_ims_client *client, int chat_id, char *text, char *attach_path) {
	// Create the psdims__message_info struct
	// call the gsoap method
	return -1;
}


int send_friend_request(network *network, psd_ims_client *client, char *user) {
/*
	int errcode;
	int soap_response;
	psdims__login_info login_info;

	login_info.name = name;
	login_info.password = password;

	soap_response = soap_call_psdims__send_friend_request(&network->soap, network->serverURL, "", &login_info, user, &errcode);
	if( soap_response != SOAP_OK ) {
		DEBUG_FAILURE_PRINTF("Server request failed");
		return -1;
	}

	// TODO add friend_request to client struct
	return 0;
*/
	return -1;
}


int send_request_accept(network *network, psd_ims_client *client, char *user) {
/*
	int errcode;
	int soap_response;
	psdims__login_info login_info;

	login_info.name = name;
	login_info.password = password;

	soap_response = soap_call_psdims__accept_request(&network->soap, network->serverURL, "", &login_info, user, &errcode);
	if( soap_response != SOAP_OK ) {
		DEBUG_FAILURE_PRINTF("Server request failed");
		return -1;
	}

	// TODO delete friend_request from client struct
	return 0;
*/
	return -1;
}


int send_request_decline(network *network, psd_ims_client *client, char *user) {
/*
	int errcode;
	int soap_response;
	psdims__login_info login_info;

	login_info.name = name;
	login_info.password = password;

	soap_response = soap_call_psdims__decline_request(&network->soap, network->serverURL, "", &login_info, user, &errcode);
	if( soap_response != SOAP_OK ) {
		DEBUG_FAILURE_PRINTF("Server request failed");
		return -1;
	}

	// TODO delete friend_request from client struct
	return 0;
*/
	return -1;
}

int user_register(network *network,psdims__user_info *user_info){
	int soap_response = 0;
	psdims__register_info *register_info=malloc(sizeof(psdims__register_info));

	if( (user_info = malloc( sizeof(psdims__register_info) )) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate register_info estructure");
		return -1;
	}
	
	register_info->name=malloc(50);
	printf("Escribe tu nuevo nick=>");
 	scanf("%s",register_info->name);

	register_info->password=malloc(50);
 	printf("Escribe tu contraseña=>");
 	scanf("%s",register_info->password);

	register_info->information=malloc(200);
	printf("Describete un poco=>");
    scanf("%s",register_info->information);

	if( soap_call_psdims__user_register(&network->soap, network->serverURL, "", register_info, &soap_response) != SOAP_OK ) {
		DEBUG_FAILURE_PRINTF("Server request failed");
		return -1;
	}

    //FUNCIONA, PERO AL DEVOLVER LA LLAMADA DE SOAP LANZA VIOLACIÓN DE SEGMENTO,FALLO DE SOAP

	strcpy(user_info->name,register_info->name);
	strcpy(user_info->information,register_info->information);
	
	// Comprobar error del servidor
	return 0;
}


int login(network *network, psd_ims_client *client,psdims__user_info *user_info) {
	int soap_response = 0;
	psdims__login_info *login_info=malloc(sizeof(psdims__login_info));
	char *soap_error;
    char *name,*password;

	login_info->name=malloc(50);
	printf("Pon tu nick=>");
 	scanf("%s",login_info->name);
	
	login_info->password=malloc(50);
 	printf("Escribe tu contraseña=>");
 	scanf("%s",login_info->password);

	soap_response = soap_call_psdims__get_user(&network->soap, network->serverURL, "", login_info, user_info);
   
    //FUNCIONA, PERO AL DEVOLVER LA LLAMADA DE SOAP LANZA VIOLACIÓN DE SEGMENTO,FALLO DE SOAP

	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		return -1;
	}
	 
 	printf("%s\n",user_info->name);
	//printf("Name: %s\n", user_info.name);
	//printf("Information: %s\n", user_info.information);

	free(login_info);
	// TODO add user information to client struct

	return 0;
}



