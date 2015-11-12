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

int main( int argc, char **argv ) {

	struct soap soap;
	char *serverURL;
	int operation = 0;
	int result = 0;

	char name[20] = "pepito";
	char passwd[20] = "abcd1234";
	char dummyname[20] = "juanito";

	if (argc < 3) {
		printf("Usage: %s http://server:port <operation>\n", argv[0]);
		exit(-1);
	}

	soap_init(&soap);

	serverURL = argv[1];
	operation = atoi(argv[2]);

	switch (operation) {
		case 0:
			soap_call_psdims__user_register(&soap, serverURL, "", name, passwd, &result);
			break;
		case 1:
			soap_call_psdims__user_unregister(&soap, serverURL, "", name, passwd, &result);
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

	printf("Result is = %d\n", result);

	soap_end(&soap);
	soap_done(&soap);

	return 0;
}
