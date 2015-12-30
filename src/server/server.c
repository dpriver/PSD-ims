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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "psd_ims_server.h"

#include "debug_def.h"

// TODO Must catch CTRL-C signal to free the resources and end the listen loop

volatile int continue_listening;


void stop_server(int sig);


int main( int argc, char **argv) {

	int listenner_ret_value = 0;
	int bind_port;
	sigset_t sig_blocked_mask;
	sigset_t old_sig_mask;

	if (argc < 4) {
		printf("Usage: %s <port> <bd_user> <bd_pass>\n", argv[0]);
		exit(-1);
	}	

	if (signal(SIGINT, stop_server) == SIG_ERR) {
		DEBUG_FAILURE_PRINTF("Could not attach SIGINT handler");
		return -1;
	}
	if (signal(SIGTERM, stop_server) == SIG_ERR) {
		DEBUG_FAILURE_PRINTF("Could not attach SIGTERM handler");
		return -1;
	}
	if (signal(SIGABRT, stop_server) == SIG_ERR) {
		DEBUG_FAILURE_PRINTF("Could not attach SIGABRT handler");
		return -1;
	}

	// Initialize variables
	sigemptyset(&sig_blocked_mask);
	sigaddset(&sig_blocked_mask, SIGINT);	//sig mask to block sigint
	sigaddset(&sig_blocked_mask, SIGTERM);	//sig mask to block sigint
	sigaddset(&sig_blocked_mask, SIGABRT);	//sig mask to block sigint
	continue_listening = 1;
	bind_port = atoi(argv[1]);
	if( bind_port <= 1024 ) {
		DEBUG_FAILURE_PRINTF("Invalid PORT");
		return 0;
	}

	// init server structure
	DEBUG_INFO_PRINTF("Init server");
	if (init_server(bind_port, argv[2], argv[3]) != 0 ) {
		DEBUG_FAILURE_PRINTF("Could not init server");
		return 0;
	}

	DEBUG_INFO_PRINTF("Init master connection");
	while( listenner_ret_value == 0 ) {
		listenner_ret_value = listen_connection();
		
		sigprocmask(SIG_BLOCK, &sig_blocked_mask, &old_sig_mask);
		if( !continue_listening ) 
			break;
		sigprocmask(SIG_SETMASK, &old_sig_mask, NULL);
	}

  return 0;
}


void stop_server(int sig) {
	DEBUG_INFO_PRINTF("Freeing server resources");
	free_server();
	continue_listening = 0;
}


