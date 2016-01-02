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
#include "bool.h"
#include "psd_ims_server.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pwd.h>


#include "debug_def.h"

#define MAX_ALIVE_THREADS (200)
#define ATTACH_FILES_DIR "server_files"

#define create_file_path(buff, chat_id, timestamp) \
		sprintf(buff, "%s/_%d%d", ATTACH_FILES_DIR, chat_id, timestamp)

struct server {
	persistence *persistence;
	struct soap soap;
	int n_alive_threads;
	pthread_mutex_t n_threads_mutex;
	pthread_cond_t zero_alive_threads;
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

	DEBUG_INFO_PRINTF("Serving slave connection");

	pthread_detach(pthread_self());
	soap_serve((struct soap*)soap);

	// end the connection and free the resources
	end_soap_connection((struct soap*)soap);
	free(soap);

	DEBUG_INFO_PRINTF("Closing slave connection");
	// decrement the number of alive threads
	pthread_mutex_lock(&server.n_threads_mutex);
	server.n_alive_threads--;
	if (server.n_alive_threads == 0) {
		pthread_cond_signal(&server.zero_alive_threads);
	}
	pthread_mutex_unlock(&server.n_threads_mutex);

	return NULL;
}



/*
 *
 * Returns 0 or -1 if fails
 */
int init_server(int bind_port, char persistence_user[], char persistence_pass[]) {
	DEBUG_TRACE_PRINT();

	SOAP_SOCKET m;
	server.n_alive_threads = 0;
	pthread_mutex_init(&server.n_threads_mutex, NULL);
	pthread_cond_init(&server.zero_alive_threads, NULL);

	server.persistence = init_persistence(persistence_user, persistence_pass);
	if (server.persistence == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not init persistence");
		return -1;
	}
	
	DEBUG_INFO_PRINTF("Init soap");
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
	
	pthread_mutex_lock(&server.n_threads_mutex);
	while (server.n_alive_threads > 0) {
		pthread_cond_wait(&server.zero_alive_threads, &server.n_threads_mutex);
	}
	pthread_mutex_unlock(&server.n_threads_mutex);
	
	free_persistence(server.persistence);
}


/*
 *
 * Returns 0 or -1 if fails
 */
int listen_connection () {
	DEBUG_TRACE_PRINT();
	int ret_value;

	if ( persistence_thread_safe(server.persistence) )
		ret_value = mthread_listen_connection();
	else
		ret_value = sthread_listen_connection();
		
	return ret_value;
}


/*
 *
 * Returns 0 or -1 if fails
 */
int sthread_listen_connection () {
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
	
	if (persistence_err(server.persistence)) {
		DEBUG_FAILURE_PRINTF("Persistence is disconnected, atempting to reconnect...");
		reconnect_persistence(server.persistence);
		if (persistence_err(server.persistence)) {
			DEBUG_FAILURE_PRINTF("Could not reconnect.");
			return -1;
		}
	}
	
	// Execute invoked operation
	if (soap_serve(&(server.soap)) != SOAP_OK) {
		soap_print_fault(&(server.soap), stderr);
	}

	// Clean up!
	soap_destroy(&(server.soap));
	soap_end(&(server.soap));
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

	DEBUG_INFO_PRINTF("Master connection ready");

	s = soap_accept(&server.soap);
	if (!soap_valid_socket(s)) {
		if(server.soap.errnum) {
			soap_print_fault(&server.soap, stderr); 
			return -1;
		}
		DEBUG_INFO_PRINTF("Server timed out");
		return -1;
	}

	if (persistence_err(server.persistence)) {
		DEBUG_FAILURE_PRINTF("Persistence is disconnected, atempting to reconnect...");
		reconnect_persistence(server.persistence);
		if (persistence_err(server.persistence)) {
			DEBUG_FAILURE_PRINTF("Could not reconnect.");
			return -1;
		}
	}

	DEBUG_INFO_PRINTF("Creating slave handler");
	// The threads are not controlled, there may potencially be an infinite number of them
	// at the same time (resource problems... )
	tsoap = soap_copy(&server.soap);	//make a safe copy
	if (!tsoap) {
		DEBUG_FAILURE_PRINTF("Could not copy the soap struct");
		return -1;
	}

	// Increment number of alive threads
	// it will be decremented inside the thread
	pthread_mutex_lock(&server.n_threads_mutex);
	if (server.n_alive_threads >= MAX_ALIVE_THREADS) {
		pthread_mutex_unlock(&server.n_threads_mutex);
		return -1;
	}
	server.n_alive_threads++;
	pthread_mutex_unlock(&server.n_threads_mutex);
	
	// Launch the new thread
	pthread_create(&tid, NULL, (void*(*)(void*))thread_serve_request, (void*)tsoap);

	return 0;
}


int check_login(persistence *persistence, psdims__login_info *login) {
	char pass[50];
	int user_id;
	
	if ( (login == NULL) || (login->name == NULL) || (login->password == NULL) ) {
		DEBUG_FAILURE_PRINTF("Login failed");
		return -1;
	}
	user_id = get_user_id(persistence, login->name);
	if( user_id == -1) {
		DEBUG_FAILURE_PRINTF("Login failed: the user does not exist\n");
		return -1;
	}
	get_user_pass(persistence,login->name, pass, sizeof(char)*50);
 	if(strcmp(login->password,pass)!=0) {
		DEBUG_FAILURE_PRINTF("Login failed: the password is not correct\n");
		return -1;
	}
	
	return user_id;
}


/* =========================================================================
 *  Gsoap handlers
 * =========================================================================*/

/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__user_register(struct soap *soap, psdims__register_info *user_info, int *ERRCODE){
	DEBUG_TRACE_PRINT();
	*ERRCODE = 1;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if ( (user_info == NULL) || (user_info->name == NULL) || (user_info->password == NULL) || (user_info->information == NULL) ) {
		DEBUG_FAILURE_PRINTF("Some fields are empty");
		return SOAP_USER_ERROR;
	}

	if (user_entry_exist(persistence, user_info->name)) {
		DEBUG_FAILURE_PRINTF("Failed to add user: The name is already in use");
		return SOAP_USER_ERROR;
	}

	DEBUG_INFO_PRINTF("Registering: name:%s pass:%s", user_info->name, user_info->password);

	if( add_user(persistence, user_info->name, user_info->password, user_info->information) != 0 ) {
		DEBUG_FAILURE_PRINTF("Failed to add user");
		return SOAP_USER_ERROR;
	}

	free_persistence(persistence);

	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__user_unregister(struct soap *soap, psdims__login_info *login, int *ERRCODE){
	DEBUG_TRACE_PRINT();
	*ERRCODE = 1;
	int user_id, timestamp;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if ( (login == NULL) || (login->name == NULL) || (login->password == NULL) ) {
		DEBUG_FAILURE_PRINTF("Some fields are empty");
		return SOAP_USER_ERROR;
	}


	user_id = check_login(persistence, login);
	if ( user_id < 0 ) {
		return SOAP_USER_ERROR;
	}

	DEBUG_INFO_PRINTF("Unregistering: name:%s ", login->name);

	timestamp = time(NULL);
	if( del_user(persistence, login->name) != 0 ) {
		DEBUG_FAILURE_PRINTF("Failed to delete user");
		return SOAP_USER_ERROR;
	}

	if( del_user_all_chats(persistence, user_id, timestamp) != 0 ) {
		DEBUG_FAILURE_PRINTF("Failed to delete user");
		return SOAP_USER_ERROR;
	}

	free_persistence(persistence);

	return SOAP_OK;
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__get_user(struct soap *soap, psdims__login_info *login, psdims__user_info *user) {
	DEBUG_TRACE_PRINT();
	int user_id;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}
	
	if ( user == NULL) {
		return SOAP_USER_ERROR;
	}
	
	user_id = check_login(persistence, login);
	if ( user_id < 0 ) {
		return SOAP_USER_ERROR;
	}
	
	user->name = soap_malloc(soap, strlen(login->name) + sizeof(char));
	user->information = soap_malloc(soap, sizeof(char)*200);

	strcpy(user->name,login->name);  
	get_user_info(persistence, user_id, user->information, 200);

	free_persistence(persistence);

	return SOAP_OK;
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__get_friends(struct soap *soap,psdims__login_info *login, int timestamp, psdims__user_list *friends){
	DEBUG_TRACE_PRINT();
	int id;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if (friends == NULL) {
		return SOAP_USER_ERROR;
	}
	
	id = check_login(persistence, login);
	if ( id < 0 ) {
		return SOAP_USER_ERROR;
	}

  	if(get_list_friends(persistence, id, timestamp, soap, friends) != 0){
		return SOAP_USER_ERROR;
	}

	free_persistence(persistence);

	return SOAP_OK;
}


// get friend info
int psdims__get_friend_info(struct soap *soap, psdims__login_info *login, char *name, psdims__user_info *friend_info) {
	DEBUG_TRACE_PRINT();
	int id, friend_id;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if ((name == NULL) || (friend_info == NULL)) {
		return SOAP_USER_ERROR;
	}
	
	id = check_login(persistence, login);
	if ( id < 0 ) {
		return SOAP_USER_ERROR;
	}
	
	friend_id = get_user_id(persistence, name);
	if( friend_id == -1) {
		DEBUG_FAILURE_PRINTF("Login failed: the user does not exist\n");
		return -1;
	}
	
	friend_info->name = soap_malloc(soap, strlen(name) + sizeof(char));
	friend_info->information = soap_malloc(soap, sizeof(char)*200);

	strcpy(friend_info->name, name);  
	get_user_info(persistence, friend_id, friend_info->information, 200);	

	free_persistence(persistence);
	
	return SOAP_OK;
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__get_chats(struct soap *soap,psdims__login_info *login, int timestamp, psdims__chat_list *chats){
	DEBUG_TRACE_PRINT();
	int id;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if (chats == NULL) {
		return SOAP_USER_ERROR;
	}

	id = check_login(persistence, login);
	if ( id < 0 ) {
		return SOAP_USER_ERROR;
	}


	if(get_list_chats(persistence, id, timestamp, soap, chats) != 0){
		return SOAP_USER_ERROR;
	}

	free_persistence(persistence);
	
	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__get_chat_info(struct soap *soap, psdims__login_info *login, int chat_id, psdims__chat_info *chat) {
	DEBUG_TRACE_PRINT();
	int id_user;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if (chat == NULL) {
		return SOAP_USER_ERROR;
	}

	id_user = check_login(persistence, login);
	if ( id_user < 0 ) {
		return SOAP_USER_ERROR;
	}

	if(chat_exist(persistence, chat_id) != 1)
		return SOAP_USER_ERROR;

	if( get_all_chat_info(persistence, chat_id, soap, chat) != 1)
		return SOAP_USER_ERROR;

	free_persistence(persistence);

	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__get_chat_messages(struct soap *soap,psdims__login_info *login, int chat_id, int timestamp, psdims__message_list *messages){
	DEBUG_TRACE_PRINT();
	int id_user;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if (messages == NULL) {
		return SOAP_USER_ERROR;
	}

	id_user = check_login(persistence, login);
	if ( id_user < 0 ) {
		return SOAP_USER_ERROR;
	}

	if(chat_exist(persistence, chat_id) != 1)
		return SOAP_USER_ERROR;

	if(exist_user_in_chat(persistence, id_user, chat_id) != 1)
		return SOAP_USER_ERROR;

	if(get_list_messages(persistence, chat_id, id_user, timestamp, soap, messages) != 0)
		return SOAP_USER_ERROR;

	free_persistence(persistence);

	return SOAP_OK; 
}


// Get the file attached to msd_id
int psdims__get_attachment(struct soap *soap, psdims__login_info *login, int chat_id, int msg_timestamp, psdims__file *file) {
	DEBUG_TRACE_PRINT();
	int id_user;
	int i = 0;
	char *file_path;
	struct stat st;
	FILE *fd;
	unsigned char * file_buffer;
	unsigned char *file_buff_aux;
	int total_blocks, readed_blocks;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if ( file == NULL) {
		return SOAP_USER_ERROR;
	}

	id_user = check_login(persistence, login);
	if ( id_user < 0 ) {
		return SOAP_USER_ERROR;
	}
	
	if( message_have_attach(persistence, id_user, chat_id, msg_timestamp) == 0) {
		DEBUG_FAILURE_PRINTF("The message does not have attachment");
		return SOAP_USER_ERROR;
	}
	
	file_path = soap_malloc(soap, sizeof(char)*20 );
	create_file_path(file_path, chat_id, msg_timestamp);

	if( (fd = fopen(file_path, "r")) == NULL) {
		DEBUG_FAILURE_PRINTF("The file does not exist yet");
		fclose(fd);
		return SOAP_USER_ERROR;
	}

	file_buffer = soap_malloc(soap, MAX_FILE_CHARS);
	total_blocks = 0;

	file_buff_aux = file_buffer;
	do {
		readed_blocks = fread(file_buff_aux++, 1, 1, fd);
		total_blocks += readed_blocks;
	} while( (readed_blocks == 1) && (total_blocks < MAX_FILE_CHARS) );

	fclose(fd);

	file->__ptr = file_buffer;
	file->__size = total_blocks * sizeof(char);

	free_persistence(persistence);

	return SOAP_OK;
}


// Send a file to attach msd_id
int psdims__send_attachment(struct soap *soap, psdims__login_info *login, int chat_id, int msg_timestamp, psdims__file *file, int *ERRCODE) {
	DEBUG_TRACE_PRINT();
	*ERRCODE = 0;

	int id_user;
	int i = 0;
	char *file_path;
	struct stat st;
	FILE *fd_write;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}
	

	if ( file == NULL) {
		return SOAP_USER_ERROR;
	}

	id_user = check_login(persistence, login);
	if ( id_user < 0 ) {
		return SOAP_USER_ERROR;
	}

	
	if( message_can_attach(persistence, id_user, chat_id, msg_timestamp) == 0) {
		DEBUG_FAILURE_PRINTF("The message does not have attachment");
		return SOAP_USER_ERROR;
	}

	if( stat(ATTACH_FILES_DIR, &st) == -1 ) {
		mkdir(ATTACH_FILES_DIR, 0700);
	}
	
	file_path = soap_malloc(soap, sizeof(char)*20 );
	create_file_path(file_path, chat_id, msg_timestamp);
	
	// check if the file exist
	if( (fd_write = fopen(file_path, "r")) != NULL) {
		DEBUG_FAILURE_PRINTF("The file does exist yet");
		fclose(fd_write);
		return SOAP_USER_ERROR;
	}

	// create the new file
	if( (fd_write = fopen(file_path, "w")) == NULL ) {
		DEBUG_FAILURE_PRINTF("Could not create the file");
		return SOAP_USER_ERROR;
	}

	if( fwrite(file->__ptr, file->__size, 1, fd_write) != 1 ) {
		DEBUG_FAILURE_PRINTF("Could not save the received file");
		fclose(fd_write);
		return SOAP_USER_ERROR;
	}


	fclose(fd_write);

	free_persistence(persistence);

	return SOAP_OK;
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__get_all_data(struct soap *soap, psdims__login_info *login, psdims__client_data *client_data){
	DEBUG_TRACE_PRINT();
	int user_id;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if (client_data == NULL) {
		return SOAP_USER_ERROR;
	}

	user_id = check_login(persistence, login);
	if ( user_id < 0 ) {
		return SOAP_USER_ERROR;
	}	
	
	client_data->timestamp = time(NULL);
	
	if (get_notif_friend_requests(persistence, user_id, 0, soap, &(client_data->friend_requests))) {
		return SOAP_USER_ERROR;
	}
	if (get_list_chats(persistence, user_id, 0, soap, &(client_data->chats))) {
		return SOAP_USER_ERROR;
	}
	
	if (get_list_friends(persistence, user_id, 0, soap, &(client_data->friends))) {
		return SOAP_USER_ERROR;
	}

	free_persistence(persistence);
	
	return SOAP_OK;
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__get_pending_notifications(struct soap *soap,psdims__login_info *login, int timestamp, psdims__sync *sync,  psdims__notifications *notifications){
	DEBUG_TRACE_PRINT();
	int i;
	int id_user;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if ((notifications == NULL) || (sync == NULL)) {
		return SOAP_USER_ERROR;
	}

	id_user = check_login(persistence, login);
	if ( id_user < 0 ) {
		return SOAP_USER_ERROR;
	}

	// use sync to update chats' read_timestamp
	for(i = 0 ; i < sync->chat_read_timestamps.__sizenelems ; i++) {
		update_sync(persistence, id_user, sync->chat_read_timestamps.chat[i].chat_id, sync->chat_read_timestamps.chat[i].timestamp);
	}

	// There may be problems with this concerns...
	notifications->last_timestamp = time(NULL);

	if(get_notif_chats_with_messages(persistence, id_user, timestamp, soap, &(notifications->chats_with_messages)) < 0){
		return SOAP_USER_ERROR;
	}
	if(get_notif_chats_read_times(persistence, id_user, soap, &(notifications->chats_read_times)) < 0){
		return SOAP_USER_ERROR;
	}	
	if(get_notif_friend_requests(persistence, id_user, timestamp, soap, &(notifications->friend_request)) < 0){
		return SOAP_USER_ERROR;
	}
	if(get_notif_chat_members(persistence, id_user, timestamp, soap, &(notifications->chat_members)) < 0){
		return SOAP_USER_ERROR;
	}
	if(get_notif_chat_rem_members(persistence, id_user, timestamp, soap, &(notifications->rem_chat_members)) < 0){
		return SOAP_USER_ERROR;
	}
	if(get_notif_chat_admins(persistence, id_user, timestamp, soap, &(notifications->chat_admins)) < 0) {
		return SOAP_USER_ERROR;
	}
	if(get_list_friends(persistence, id_user, timestamp, soap, &(notifications->new_friends)) < 0){
		return SOAP_USER_ERROR;
	}

	free_persistence(persistence);
	
	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__create_chat(struct soap *soap, psdims__login_info *login, psdims__new_chat *new_chat, int *chat_id) {
	DEBUG_TRACE_PRINT();
	int timestamp;
	int id_user;
	int id_member;
	int aux_chat_id;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if ( (new_chat == NULL) || (chat_id == NULL) ) {
		return SOAP_USER_ERROR;
	}
	
	id_user = check_login(persistence, login);
	if ( id_user < 0 ) {
		return SOAP_USER_ERROR;
	}
	
	timestamp = time(NULL);

	id_member = get_user_id(persistence, new_chat->member);
	if (id_user == -1) {
		printf("User does not exist\n");
		return SOAP_USER_ERROR;
	}

	if(add_chat(persistence, id_user, new_chat->description, timestamp, &aux_chat_id) !=0 ) {
		return SOAP_USER_ERROR;
	}
	if(add_user_chat(persistence, id_user, aux_chat_id, timestamp, timestamp) != 0) {
		del_chat(persistence, aux_chat_id);
		return SOAP_USER_ERROR;
	}
	if(add_user_chat(persistence, id_member, aux_chat_id, timestamp, timestamp) != 0) {
		return SOAP_USER_ERROR;
	}

	*chat_id = aux_chat_id;

	free_persistence(persistence);

	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__add_member(struct soap *soap, psdims__login_info *login, char *name, int chat_id, int *ERRCODE) {
	DEBUG_TRACE_PRINT();
	
	int id_user;
	int id_login;
	int timestamp;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if ( name == NULL ) {
		return SOAP_USER_ERROR;
	}

	id_login = check_login(persistence, login);
	if ( id_login < 0 ) {
		return SOAP_USER_ERROR;
	}

	timestamp = time(NULL);

	if(chat_exist(persistence, chat_id) != 1) {
		printf("Chat does not exist\n");
		return SOAP_USER_ERROR;
	}

	if(!is_admin(persistence, id_login, chat_id)) {
		printf("User is not the chat admin");
		return SOAP_USER_ERROR;
	}

	id_user = get_user_id(persistence, name);
	if (id_user == -1) {
		printf("User does not exist\n");
		return SOAP_USER_ERROR;
	}

	if (id_user == id_login) {
		printf("An user tried to add himself to a chat");
		return SOAP_USER_ERROR;
	}

	if( exist_friendly(persistence, id_user, id_login) != 1) {
		printf("An user tried to add a non-friend to a chat");
		return SOAP_USER_ERROR;;
	}

    if(exist_user_in_chat(persistence, id_user, chat_id) == 1){
		printf("User is already in the chat\n");
		return SOAP_USER_ERROR;
	}

	if (exist_user_entry_in_chat(persistence, id_user, chat_id) == 1 ) {
		 if( recover_user_chat(persistence, id_user, chat_id, timestamp) != 0) {
			return SOAP_USER_ERROR;
		}		
	}
	else {
		// msg read is equal to current time because we don't want a new chat user to read previous messages
		if(add_user_chat(persistence, id_user, chat_id, timestamp, timestamp) != 0){
			return SOAP_USER_ERROR;
		}	
	}

	free_persistence(persistence);

	return SOAP_OK;  
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__remove_member(struct soap *soap, psdims__login_info *login, char *name, int chat_id, int *ERRCODE) {
	DEBUG_TRACE_PRINT();
	
	int id_user;
	int id_login;
	int timestamp;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if ( name == NULL ) {
		return SOAP_USER_ERROR;
	}

	id_login = check_login(persistence, login);
	if ( id_login < 0 ) {
		return SOAP_USER_ERROR;
	}
	
	if(chat_exist(persistence, chat_id) != 1) {
		printf("Chat does not exist\n");
		return SOAP_USER_ERROR;
	}

	if(!is_admin(persistence, id_login, chat_id)) {
		printf("User is not the chat admin");
		return SOAP_USER_ERROR;
	}

	id_user = get_user_id(persistence, name);
	if (id_user == -1) {
		printf("User does not exist\n");
		return SOAP_USER_ERROR;
	}

	if (id_user == id_login) {
		printf("User can not remove himself from chat");
		return SOAP_USER_ERROR;
	}

    if(!exist_user_in_chat(persistence, id_user, chat_id) == 1){
		printf("User does not exist in the chat\n");
		return SOAP_USER_ERROR;
	}
	
	timestamp = time(NULL);
	if(del_user_chat(persistence, id_user, chat_id, timestamp) != 0)
		return SOAP_USER_ERROR;

	free_persistence(persistence);

	return SOAP_OK;  
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__quit_from_chat(struct soap *soap, psdims__login_info *login, int chat_id, int *ERRCODE) {
	DEBUG_TRACE_PRINT();
	
	int id_user,first_user, timestamp;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	id_user = check_login(persistence, login);
	if ( id_user < 0 ) {
		return SOAP_USER_ERROR;
	}

	if(chat_exist(persistence, chat_id) != 1)
		return SOAP_USER_ERROR;

	if(exist_user_in_chat(persistence, id_user, chat_id) != 1)
		return SOAP_USER_ERROR;

	timestamp = time(NULL);

	if(del_user_chat(persistence, id_user, chat_id, timestamp) != 0)	
		return SOAP_USER_ERROR;

	if(still_users_in_chat(persistence, chat_id) == 1){
		if(is_admin(persistence, id_user, chat_id) == 1){
			if((first_user = get_first_users_in_chat(persistence, chat_id)) == 1)
				return SOAP_USER_ERROR;
			if(change_admin(persistence, first_user, chat_id, timestamp) == 1)
				return SOAP_USER_ERROR;
		}
	}
	else{
		if(del_chat(persistence, chat_id) != 0)	
			return SOAP_USER_ERROR;
	}

	free_persistence(persistence);

	return SOAP_OK;  
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__send_message(struct soap *soap,psdims__login_info *login, int chat_id,  psdims__message_info *message, int *timestamp){
	DEBUG_TRACE_PRINT();
	int id_user;
	int local_time;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if ( (message == NULL) || (timestamp == NULL) ) {
		return SOAP_USER_ERROR;
	}

	id_user = check_login(persistence, login);
	if ( id_user < 0 ) {
		return SOAP_USER_ERROR;
	}
	

	if(exist_user_in_chat(persistence, id_user, chat_id)!=1)
		return SOAP_USER_ERROR;

	
	// As timestamp are in seconds resolution, and the messages do not have id, 
	// they need diferent timestamps...
	// Sure this can be done better.
	local_time = time(NULL);
	while( exist_timestamp_in_messages(persistence, chat_id, local_time) ) {
		sleep(1);
		local_time = time(NULL);
	}
	
	if( send_messages(persistence, chat_id, id_user, local_time, message) != 0)
		return SOAP_USER_ERROR;

	*timestamp = local_time;

	free_persistence(persistence);

	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__send_friend_request(struct soap *soap,psdims__login_info *login, char* request_name, int *timestamp){
	DEBUG_TRACE_PRINT();
	int id_user,id_request_name;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if ( (request_name == NULL) || (timestamp == NULL) ) {
		return SOAP_USER_ERROR;
	}

	id_user = check_login(persistence, login);
	if ( id_user < 0 ) {
		return SOAP_USER_ERROR;
	}

	id_request_name = get_user_id(persistence, request_name);
	if (id_request_name == id_user) {
		DEBUG_FAILURE_PRINTF("An user tried to add himshelf.. what a jerk..");
		return SOAP_USER_ERROR;
	}
	*timestamp = time(NULL);

	if(exist_friendly(persistence,id_user,id_request_name) != 0){
		DEBUG_FAILURE_PRINTF("The users are already friends\n");
		return SOAP_USER_ERROR;
	}

	if(exist_request(persistence, id_user, id_request_name) != 0){
		DEBUG_FAILURE_PRINTF("There is a previous a friend request\n");
		return SOAP_USER_ERROR;
	}

	if(send_request(persistence,id_user, id_request_name, *timestamp) != 0)
		return SOAP_USER_ERROR;

	free_persistence(persistence);

	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__accept_request(struct soap *soap,psdims__login_info *login, char *request_name, int *timestamp){
	DEBUG_TRACE_PRINT();
	int id_user, id_request_name;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if ( (request_name == NULL) || (timestamp == NULL) ) {
		return SOAP_USER_ERROR;
	}

	id_user = check_login(persistence, login);
	if ( id_user < 0 ) {
		return SOAP_USER_ERROR;
	}

	id_request_name = get_user_id(persistence,request_name);
	*timestamp = time(NULL);

	DEBUG_INFO_PRINTF("accepting req %s -> %s", request_name, login->name);
	if(exist_request(persistence, id_request_name, id_user) == 0){
		DEBUG_FAILURE_PRINTF("The friend request does not exist\n");
		return SOAP_USER_ERROR;
	}
    
    if(accept_friend_request(persistence, id_request_name, id_user, *timestamp) != 0)
		return SOAP_USER_ERROR;

	free_persistence(persistence);

	return SOAP_OK; 
}


/*
 *
 * Returns SOAP_OK or SOAP_USER_ERROR if fails
 */
int psdims__decline_request(struct soap *soap, psdims__login_info *login, char *request_name, int *timestamp){
	DEBUG_TRACE_PRINT();
	int id_user,id_request_name;
	persistence *persistence;
	
	persistence = clone_persistence(server.persistence);
	if (persistence == NULL) {
		DEBUG_FAILURE_PRINTF("Could not create the persistence struct");
		return SOAP_USER_ERROR;
	}

	if ( (request_name == NULL) || (timestamp == NULL) ) {
		return SOAP_USER_ERROR;
	}

	id_user = check_login(persistence, login);
	if ( id_user < 0 ) {
		return SOAP_USER_ERROR;
	}

	id_request_name = get_user_id(persistence, request_name);
	*timestamp = time(NULL);

	if(exist_request(persistence, id_request_name, id_user) == 0){
		DEBUG_FAILURE_PRINTF("The friends request does not exist\n");
		return SOAP_USER_ERROR;
	}

	if(decline_friend_request(persistence, id_request_name, id_user) != 0)
		return SOAP_USER_ERROR;

	free_persistence(persistence);

	return SOAP_OK; 
}

