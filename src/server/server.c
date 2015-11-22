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

#include "psd_ims_server.h"
//#include "persistence.h"
//#include "network.h"

#include "debug_def.h"


int main( int argc, char **argv) {

	int bind_port;

	if (argc < 4) {
		printf("Usage: %s <port> <bd_user> <bd_pass>\n", argv[0]);
		exit(-1);
	}	
;
	bind_port = atoi(argv[1]);
	DEBUG_INFO_PRINTF("Init server");
	init_server(bind_port, argv[2], argv[3]);

	while( 1 ) {
		DEBUG_INFO_PRINTF("Waiting for new connection");
		listen_connection();
	}

  return 0;
}
