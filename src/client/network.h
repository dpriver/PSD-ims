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
//#include "psdims.nsmap"

typedef struct network network;
struct network {
	char *serverURL;
	struct soap soap;
};


network *net_new(char *serverURL);

void net_free(network *network);

int net_recv_notifications(network *network);

int net_recv_pending_messages(network *network, int chat_id);

int net_recv_new_chats(network *network);

int net_send_message(network *network, int chat_id, char *text, char *attach_path);

int net_send_friend_request(network *network, char *user);

int net_send_request_accept(network *network, char *user);

int net_send_request_decline(network *network, char *user);

int net_user_register(network *network, char *name, char *password, char *information);

int net_login(network *network, char *name, char *password);


#endif /* __NETWORK */
