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
#include "psd_ims_client.h"

typedef struct network network;
struct network {
	char *serverURL;
	struct soap soap;
};


network *init_network(char *serverURL);


void *free_network(network *network);


int recv_notifications(network *network, psd_ims_client *client);


int recv_pending_messages(network *network, psd_ims_client *client);


int recv_new_chats(network *network, psd_ims_client *client);


int send_message(network *network, psd_ims_client *client);


int send_friend_request(network *network, psd_ims_client *client);


int send_request_accept(network *network, psd_ims_client *client);


int send_request_decline(network *network, psd_ims_client *client);


int alta(network *network, char *name, char *password, char *information);


int login(network *network, psd_ims_client *client, char *name, char *password);


#endif /* __NETWORK */
