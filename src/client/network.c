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
#include "bool.h"

#include <stdlib.h>

#include "debug_def.h"

#ifdef DEBUG
#include "leak_detector_c.h"
#endif


void _net_unlink_file(struct soap *soap, psdims__file *file) {
	soap_unlink(soap, file->__ptr);
}

/*
 *
 *
 */
void _net_unlink_user(struct soap *soap, psdims__user_info *user) {
	soap_unlink(soap, user->name);
	soap_unlink(soap, user->information);
}


void _net_unlink_user_list(struct soap *soap, psdims__user_list *user) {
	int i;
	for( i = 0 ; i < user->__sizenelems ; i++ ) {
		_net_unlink_user(soap, &user->user[i]);
	}
	soap_unlink(soap, user->user);
}


void _net_unlink_notification() {

}


void _net_unlink_notification_list(struct soap *soap, psdims__notifications *notifications) {
	int i;
	for( i = 0 ; i < notifications->friend_request.__sizenelems ; i++) {
		soap_unlink(soap, notifications->friend_request.user[i].name.string);
	}
	soap_unlink(soap, notifications->friend_request.user);
/*
	for( i = 0 ; i < notifications->deleted_friends.__sizenelems ; i++) {
		soap_unlink(soap, notifications->deleted_friends.user[i].name.string);
	}
	soap_unlink(soap, notifications->deleted_friends.user);
	soap_unlink(soap, notifications->new_chats.chat);
	soap_unlink(soap, notifications->deleted_chats.chat);
*/
	soap_unlink(soap, notifications->chats_with_messages.chat);
}


void _net_unlink_message() {

}


void _net_unlink_message_list(struct soap *soap, psdims__message_list *messages) {
	int i;
	for( i = 0 ; i < messages->__sizenelems ; i++ ) {
		soap_unlink(soap, messages->messages[i].user);
		soap_unlink(soap, messages->messages[i].text);
	}
	soap_unlink(soap, messages->messages);
}


void _net_unlink_chat() {

}


void _net_unlink_chat_list(struct soap *soap, psdims__chat_list *chats) {
	int i, j;
	for( i = 0 ; i < chats->__sizenelems ; i++ ) {
		for( j = 0 ; j < chats->chat_info[i].members.__sizenelems ; j++ ) {
			soap_unlink(soap, chats->chat_info[i].members.name[j].string);
		}
		soap_unlink(soap, chats->chat_info[i].members.name);
		//soap_unlink(soap, &(chats->chat_info[i].members));
		soap_unlink(soap, chats->chat_info[i].description);
		soap_unlink(soap, chats->chat_info[i].admin);
	}
	soap_unlink(soap, chats->chat_info);
}


/*
 *
 *
 */
network *net_new() {
	DEBUG_TRACE_PRINT();
	network *new_network;
	if( (new_network = malloc( sizeof(network) )) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate network estructure");
		return NULL;
	}

	new_network->login_info.name = NULL;
	new_network->login_info.password = NULL;	
	new_network->serverURL = NULL;
	new_network->logged = FALSE;

	new_network->soap.send_timeout = 60; 			// 60 secs
	new_network->soap.recv_timeout = 60;			// 60 secs

	soap_init(&new_network->soap);
	return new_network;
}


/*
 *
 *
 */
void net_free(network *network) {
	DEBUG_TRACE_PRINT();
	soap_end(&network->soap);
	soap_done(&network->soap);

	free(network->serverURL);
	free(network->login_info.name);
	free(network->login_info.password);
	network->serverURL = NULL;
	network->login_info.name = NULL;
	network->login_info.password = NULL;	
	free(network);
}


/*
 * Binds the network to the psd_ims server url
 *
 */
int net_bind_network(network *network, char *serverURL) {
	DEBUG_TRACE_PRINT();

	if( (network->serverURL = malloc( sizeof(char)*(strlen(serverURL)+1) )) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate network estructure");
		return -1;
	}

	strcpy(network->serverURL,serverURL);
	return 0;
}


/*
 *
 *
 */
psdims__user_info *net_login(network *network, char *name, char *password) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	psdims__login_info login_info;
	psdims__user_info *user_info;
	char *soap_error;

	login_info.name = name;
	login_info.password = password;

	if ( (user_info = malloc(sizeof(psdims__user_info)) ) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory for user info");
		return NULL;
	}

	soap_response = soap_call_psdims__get_user(&network->soap, network->serverURL, "", &login_info, user_info);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		free(user_info);
		return NULL;
	}

	if ( (network->login_info.name = malloc(strlen(name) + sizeof(char)) ) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory for network user name");
		return NULL;
	}
	if ( (network->login_info.password = malloc(strlen(password) + sizeof(char)) ) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory for network pass name");
		free(network->login_info.name);
		network->login_info.name = NULL;
		return NULL;
	}

	strcpy(network->login_info.name, name);
	strcpy(network->login_info.password, password);
	network->logged = TRUE;

	_net_unlink_user(&network->soap, user_info);

	return user_info;
}


void net_logout(network *network) {
	network->logged = FALSE;
	free(network->login_info.name);
	free(network->login_info.password);
}


psdims__user_info *net_recv_user_info(network *network, char *name) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	psdims__user_info *user_info;
	char *soap_error;


	if ( (user_info = malloc(sizeof(psdims__user_info)) ) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory for user info");
		return NULL;
	}

	soap_response = soap_call_psdims__get_friend_info(&network->soap, network->serverURL, "", &network->login_info, name, user_info);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		free(user_info);
		return NULL;
	}

	_net_unlink_user(&network->soap, user_info);

	return user_info;
}


/*
 * 
 * 
 */
psdims__client_data *net_recv_all_data(network *network) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	psdims__client_data *client_data;
	char *soap_error;	
	
	if ( (client_data = malloc(sizeof(psdims__client_data)) ) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory for client data");
		return NULL;
	}
	
	soap_response = soap_call_psdims__get_all_data(&network->soap, network->serverURL, "", &network->login_info, client_data);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		free(client_data);
		return NULL;
	}
	
	//_net_unlink_client_data(&network->soap, client_data);
	
	return client_data;
}


/*
 *
 *
 */
psdims__notifications *net_recv_notifications(network *network, int timestamp, int chat_id[], int read_timestamp[], int n_chats) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	psdims__notifications *notification_list;
	psdims__sync sync;
	char *soap_error;
	int i;

	if( !network->logged ) {
		DEBUG_FAILURE_PRINTF("Not logged");
		return NULL;
	}

	if ( (notification_list = malloc(sizeof(psdims__notifications)) ) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory for notification list");
		return NULL;
	}
	
	// Create sync struct
	if ( (sync.chat_read_timestamps.chat = malloc(sizeof(psdims__notif_chat_info)*n_chats) ) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory for sync");
		return NULL;
	}	
	sync.chat_read_timestamps.__sizenelems = n_chats;
	for ( i = 0 ; i < n_chats ; i++) {
		sync.chat_read_timestamps.chat[i].chat_id = chat_id[i];
		sync.chat_read_timestamps.chat[i].timestamp = read_timestamp[i];
	}

	soap_response = soap_call_psdims__get_pending_notifications(&network->soap, network->serverURL, "", &network->login_info, timestamp, &sync, notification_list);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		free(notification_list);
		return NULL;
	}

	free(sync.chat_read_timestamps.chat);
	_net_unlink_notification_list(&network->soap, notification_list);

	return notification_list;
}


/*
 *
 *
 */
psdims__message_list *net_recv_pending_messages(network *network, int chat_id, int timestamp) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	psdims__message_list *message_list;
	char *soap_error;

	if( !network->logged ) {
		DEBUG_FAILURE_PRINTF("Not logged");
		return NULL;
	}

	if ( (message_list = malloc(sizeof(psdims__message_list)) ) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory for message list");
		return NULL;
	}

	soap_response = soap_call_psdims__get_chat_messages(&network->soap, network->serverURL, "", &network->login_info, chat_id, timestamp, message_list);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		free(message_list);
		return NULL;
	}

	_net_unlink_message_list(&network->soap, message_list);

	return message_list;
}


/*
 *
 *
 */
psdims__file *net_get_attachment(network *network, int chat_id, int msg_timestamp) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	psdims__file *file;
	char *soap_error;

	if( !network->logged ) {
		DEBUG_FAILURE_PRINTF("Not logged");
		return NULL;
	}

	if ( (file = malloc(sizeof(psdims__file)) ) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory for the file");
		return NULL;
	}

	soap_response = soap_call_psdims__get_attachment(&network->soap, network->serverURL, "", &network->login_info, chat_id, msg_timestamp, file);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		free(file);
		return NULL;
	}

	_net_unlink_file(&network->soap, file);

	return file;
}


/*
 *
 *
 */
psdims__chat_list *net_get_chat_list(network *network, int timestamp) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	psdims__chat_list *chat_list;
	char *soap_error;

	if( !network->logged ) {
		DEBUG_FAILURE_PRINTF("Not logged");
		return NULL;
	}

	if ( (chat_list = malloc(sizeof(psdims__chat_list)) ) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory for chat list");
		return NULL;
	}

	soap_response = soap_call_psdims__get_chats(&network->soap, network->serverURL, "", &network->login_info, timestamp,  chat_list);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		free(chat_list);
		return NULL;
	}

	_net_unlink_chat_list(&network->soap, chat_list);

	return chat_list;
}


/*
 *
 *
 */
psdims__user_list *net_get_friend_list(network *network, int timestamp) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	psdims__user_list *user_list;
	char *soap_error;

	if( !network->logged ) {
		DEBUG_FAILURE_PRINTF("Not logged");
		return NULL;
	}

	if ( (user_list = malloc(sizeof(psdims__user_list)) ) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not allocate memory for chat list");
		return NULL;
	}

	soap_response = soap_call_psdims__get_friends(&network->soap, network->serverURL, "", &network->login_info, timestamp,  user_list);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		free(user_list);
		return NULL;
	}

	_net_unlink_user_list(&network->soap, user_list);

	return user_list;
}


/*
 *
 *
 */
int net_user_register(network *network, char *name, char *password, char *information){
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	int errcode = 0;
	char *soap_error;
	psdims__register_info user_info;

	user_info.name = name;
	user_info.password = password;
	user_info.information = information;

	soap_response = soap_call_psdims__user_register(&network->soap, network->serverURL, "", &user_info, &errcode);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		return -1;
	}

	// Comprobar error del servidor
	return 0;
}


/*
 *
 *
 */
int net_user_unregister(network *network, char *name, char *password){
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	int errcode = 0;
	char *soap_error;
	psdims__login_info user_info;

	user_info.name = name;
	user_info.password = password;

	soap_response = soap_call_psdims__user_unregister(&network->soap, network->serverURL, "", &user_info,  &errcode);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		return -1;
	}

	// Comprobar error del servidor
	return 0;
}


/*
 *
 *
 */
int net_create_chat(network *network, char *description, char *member, int *chat_id) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	int errcode = 0;
	char *soap_error;
	psdims__new_chat new_chat;

	new_chat.description = description;
	new_chat.member = member;

	soap_response = soap_call_psdims__create_chat(&network->soap, network->serverURL, "", &network->login_info, &new_chat, chat_id);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		return -1;
	}

	// Comprobar error del servidor
	return 0;
}


/*
 *
 *
 */
int net_add_user_to_chat(network *network, char *member, int chat_id) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	int errcode = 0;
	char *soap_error;

	soap_response = soap_call_psdims__add_member(&network->soap, network->serverURL, "", &network->login_info, member, chat_id, &errcode);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		return -1;
	}

	// Comprobar error del servidor
	return 0;
}


/*
 *
 *
 */
int net_remove_user_from_chat(network *network, char *member, int chat_id) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	int errcode = 0;
	char *soap_error;

	soap_response = soap_call_psdims__remove_member(&network->soap, network->serverURL, "", &network->login_info, member, chat_id, &errcode);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		return -1;
	}

	// Comprobar error del servidor
	return 0;	
}


/*
 *
 *
 */
int net_quit_from_chat(network *network, int chat_id) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	int errcode = 0;
	char *soap_error;

	soap_response = soap_call_psdims__quit_from_chat(&network->soap, network->serverURL, "", &network->login_info, chat_id, &errcode);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		return -1;
	}

	// Comprobar error del servidor
	return 0;
}


/*
 *
 *
 */
int net_send_message(network *network, int chat_id, char *text, char *attach_name, int *timestamp) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	char *soap_error;
	psdims__message_info message_info;

	if( !network->logged ) {
		DEBUG_FAILURE_PRINTF("Not logged");
		return -1;
	}

	message_info.user = network->login_info.name;
	message_info.text = text;
	message_info.file_name = attach_name;

	soap_response = soap_call_psdims__send_message(&network->soap, network->serverURL, "", &network->login_info, chat_id, &message_info, timestamp);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		return -1;
	}

	// Comprobar error del servidor
	return 0;
}


/*
 *
 *
 */
int net_send_attachment(network *network, int chat_id, int msg_timestamp, unsigned char *ptr, int size) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	psdims__file file;
	int errcode = 0;
	char *soap_error;

	if( !network->logged ) {
		DEBUG_FAILURE_PRINTF("Not logged");
		return -1;
	}

	file.__ptr = ptr;
	file.__size = size;


	soap_response = soap_call_psdims__send_attachment(&network->soap, network->serverURL, "", &network->login_info, chat_id, msg_timestamp, &file, &errcode);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		return -1;
	}

	return 0;
}


/*
 *
 *
 */
int net_send_friend_request(network *network, char *user, int *timestamp) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	char *soap_error;

	if( !network->logged ) {
		DEBUG_FAILURE_PRINTF("Not logged");
		return -1;
	}

	soap_response = soap_call_psdims__send_friend_request(&network->soap, network->serverURL, "", &network->login_info, user, timestamp);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		return -1;
	}

	// Comprobar error del servidor
	return 0;
}


/*
 *
 *
 */
int net_send_request_accept(network *network, char *user, int *timestamp) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	char *soap_error;

	if( !network->logged ) {
		DEBUG_FAILURE_PRINTF("Not logged");
		return -1;
	}

	soap_response = soap_call_psdims__accept_request(&network->soap, network->serverURL, "", &network->login_info, user, timestamp);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		return -1;
	}

	// Comprobar error del servidor
	return 0;
}


/*
 *
 *
 */
int net_send_request_decline(network *network, char *user) {
	DEBUG_TRACE_PRINT();
	int soap_response = 0;
	int timestamp;
	char *soap_error;
	
	if( !network->logged ) {
		DEBUG_FAILURE_PRINTF("Not logged");
		return -1;
	}

	soap_response = soap_call_psdims__decline_request(&network->soap, network->serverURL, "", &network->login_info, user, &timestamp);
	if( soap_response != SOAP_OK ) {
		soap_error = malloc(sizeof(char)*200);
		soap_sprint_fault(&network->soap, soap_error, sizeof(char)*200);
		DEBUG_FAILURE_PRINTF("Server request failed: %s", soap_error);
		free(soap_error);
		return -1;
	}

	// Comprobar error del servidor
	return 0;
}


void net_free_file(psdims__file *file) {
	DEBUG_TRACE_PRINT();
	free(file->__ptr);
	free(file);
}


/*
 *
 *
 */
void net_free_user(psdims__user_info *user) {
	DEBUG_TRACE_PRINT();
	free(user->name);
	free(user->information);
	free(user);
}


void net_free_user_list() {
	DEBUG_TRACE_PRINT();

}


void net_free_notification() {
	DEBUG_TRACE_PRINT();

}


void net_free_notification_list(psdims__notifications *notifications) {
	DEBUG_TRACE_PRINT();
	int i;
	for( i = 0 ; i < notifications->friend_request.__sizenelems ; i++) {
		free(notifications->friend_request.user[i].name.string);
	}
	free(notifications->friend_request.user);
/*
	for( i = 0 ; i < notifications->deleted_friends.__sizenelems ; i++) {
		free(notifications->deleted_friends.user[i].name.string);
	}
	free(notifications->deleted_friends.user);
	free(notifications->new_chats.chat);
	free(notifications->deleted_chats.chat);
*/
	free(notifications->chats_with_messages.chat);

	free(notifications);
}


void net_free_message() {
	DEBUG_TRACE_PRINT();

}


void net_free_message_list(psdims__message_list *messages) {
	int i;
	for( i = 0 ; i < messages->__sizenelems ; i++ ) {
		free(messages->messages[i].user);
		free(messages->messages[i].text);
	}
	free(messages->messages);
}


void net_free_chat() {
	DEBUG_TRACE_PRINT();

}


void net_free_chat_list(psdims__chat_list *chats) {
	DEBUG_TRACE_PRINT();
	int i, j;
	for( i = 0 ; i < chats->__sizenelems ; i++ ) {
		for( j = 0 ; j < chats->chat_info[i].members.__sizenelems ; j++ ) {
			free(chats->chat_info[i].members.name[j].string);
		}
		free(chats->chat_info[i].members.name);
		//free(chats->chat_info[i].members);
		free(chats->chat_info[i].description);
		free(chats->chat_info[i].admin);
	}
	free(chats->chat_info);
	free(chats);
}


