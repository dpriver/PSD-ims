/*******************************************************************************
 *	client.c
 *
 *  client main file
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

#include "network.h"
#include "psd_ims_client.h"

#ifdef DEBUG
#include "leak_detector_c.h"
#endif

int main( int argc, char **argv ) {



	int operation = 0;
	int return_value;
	network *network;

	atexit(report_mem_leak);


	char dummy_name[] = "pepito";
	char dummy_password[] = "qwed";
	char dummy_description[] = "juanito";

	if (argc < 3) {
		printf("Usage: %s http://server:port <operation>\n", argv[0]);
		exit(-1);
	}
	operation = atoi(argv[2]);

	if( (network = init_network(argv[1])) == NULL) {
		printf("Coult not create network estructure\n");
		return 0;
	}
	
	switch (operation) {
		case 0:
			if( login(network, NULL, dummy_name, dummy_password) != 0 ) {
				printf("Error at login\n");
			}
			break;
		case 1:
			break;
		/*
			case 2:
			soap_call_psdims__friend_request(&soap, serverURL, "", name, passwd, dummyname, &result);
			break;
		case 3:
			//soap_call_psdims__get_request(&soap, serverURL, "", name, passwd, dummyname, NULL);
			break;
		case 4:
			soap_call_psdims__accept_request(&soap, serverURL, "", name, passwd, dummyname, &result);
			break;		
		case 5:
			soap_call_psdims__decline_request(&soap, serverURL, "", name, passwd, dummyname, &result);
			break;
        */
	}

	//printf("Result is = %d\n", result);
	
	free_network(network);	

	return 0;
}
