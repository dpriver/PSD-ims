/*******************************************************************************
 *	network.h
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

#ifndef __NETWORK
#define __NETWORK


#include "soapH.h"
#include "bool.h"
//#include "psdims.nsmap"

typedef struct network network;
struct network {
	boolean logged;
	psdims__login_info login_info;
	char *serverURL;
	struct soap soap;
};

/*
 *
 *
 */
network *net_new();

/*
 *
 *
 */
void net_free(network *network);

/*
 *
 *
 */
int net_bind_network(network *network, char *serverURL);

/*
 *
 *
 */
psdims__user_info *net_login(network *network, char *name, char *password);


/*
 *
 *
 */
psdims__user_info *net_recv_user_info(network *network, char *name);

/*
 *
 *
 */
void net_logout(network *network);

/*
 * 
 * 
 */
 psdims__client_data *net_recv_all_data(network *network);

/*
 *
 *
 */
psdims__notifications *net_recv_notifications(network *network, int timestamp, int chat_id[], int read_timestamp[], int n_chats);

/*
 *
 *
 */
psdims__message_list *net_recv_pending_messages(network *network, int chat_id, int timestamp);

/*
 *
 *
 */
psdims__file *net_get_attachment(network *network, int chat_id, int msg_timestamp);

/*
 *
 *
 */
psdims__chat_list *net_get_chat_list(network *network, int timestamp);

/*
 *
 *
 */
psdims__chat_info *net_recv_chat_info(network *network, int chat_id);

/*
 *
 *
 */
psdims__user_list *net_get_friend_list(network *network, int timestamp);


/*
 *
 *
 */
int net_user_register(network *network, char *name, char *password, char *information);

/*
 *
 *
 */
int net_user_unregister(network *network, char *name, char *password);

/*
 *
 *
 */
int net_create_chat(network *network, char *description, char *member, int *chat_id);

/*
 *
 *
 */
int net_add_user_to_chat(network *network, char *member, int chat_id);

/*
 *
 *
 */
int net_remove_user_from_chat(network *network, char *member, int chat_id);

/*
 *
 *
 */
int net_quit_from_chat(network *network, int chat_id);

/*
 *
 *
 */
int net_send_message(network *network, int chat_id, char *text, char *attach_name, int *timestamp);

/*
 *
 *
 */
int net_send_attachment(network *network, int chat_id, int msg_timestamp, unsigned char *ptr, int size);

/*
 *
 *
 */
int net_send_friend_request(network *network, char *user, int *timestamp);

/*
 *
 *
 */
int net_send_request_accept(network *network, char *user, int *timestamp);

/*
 *
 *
 */
int net_send_request_decline(network *network, char *user);


/*
 *
 *
 */
void net_free_user(psdims__user_info *user);

void net_free_user_list();

void net_free_notification();

void net_free_notification_list(psdims__notifications *notifications);

void net_free_message();

void net_free_message_list(psdims__message_list *messages);

void net_free_chat();

void net_free_chat_list(psdims__chat_list *chats);



#endif /* __NETWORK */
