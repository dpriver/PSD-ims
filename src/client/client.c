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

#include "soapH.h"
#include "psdims.nsmap"
#include "friends.h"
#include "leak_detector_c.h"
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv ) {

	struct soap soap;
	char *serverURL;
	int operation = 0;
	int result = 0;
	atexit(report_mem_leak);

	friend_node *friend_list;

	char name[20] = "pepito";
	char passwd[20] = "abcd1234";
	char dummyname[20] = "juanito";
	char description[100];

	if (argc < 3) {
		printf("Usage: %s http://server:port <operation>\n", argv[0]);
		exit(-1);
	}

	//friend_list = new_friend_list();
	//print_friend_list(friend_list);
	//free_friend_list(friend_list);
	
	//return 0;
	soap_init(&soap);

	serverURL = argv[1];
	operation = atoi(argv[2]);

	switch (operation) {
		case 0:
 			printf("Introduce tu nick =>");
            scanf("%s", name);
            printf("Introduce tu contraseña =>");
			scanf("%s", passwd);
            printf("Introduce una breve descripcion tuya =>");
			scanf("%s", description);
			soap_call_psdims__user_register(&soap, serverURL, "", name, passwd,description, &result);
			break;
		case 1:
            printf("Introduce tu nick =>");
            scanf("%s", name);
			soap_call_psdims__user_unregister(&soap, serverURL, "", name, &result);
			break;
		case 2:
			soap_call_psdims__friend_request(&soap, serverURL, "", name, passwd, dummyname, &result);
			break;
		case 3:
			//soap_call_psdims__get_request(&soap, serverURL, "", name, passwd, dummyname, /*TODO*/);
			break;
		case 4:
			soap_call_psdims__accept_request(&soap, serverURL, "", name, passwd, dummyname, &result);
			break;		
		case 5:
			soap_call_psdims__decline_request(&soap, serverURL, "", name, passwd, dummyname, &result);
			break;
	}

	if (soap.error) {
		soap_print_fault(&soap, stderr);
		exit(-1);
	}

	//printf("Result is = %d\n", result);

	soap_end(&soap);
	soap_done(&soap);

	return 0;
}
