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


#include "soapH.h"
#include "psdims.nsmap"
#include <mysql.h>
#include "persistence.h"
#include <stdio.h>
#include <stdlib.h>

MYSQL* bd;

int main( int argc, char **argv) {

	int m, s;
	struct soap soap;


	if (argc < 2) {
		printf("Usage: %s <port>\n", argv[0]);
		exit(-1);
	}	

	// Init environment
	soap_init(&soap);
    bd=init_bd("root","calasancio3","PSD");
	// Bind to the specified port	
	m = soap_bind(&soap, NULL, atoi(argv[1]), 100);

	// Check result of binding		
	if (m < 0) {
		soap_print_fault(&soap, stderr); exit(-1); 
	}

	// Listen to next connection
	while (1) { 

		// accept
		s = soap_accept(&soap);    

		if (s < 0) {
			soap_print_fault(&soap, stderr); exit(-1);
		}

		// Execute invoked operation
		soap_serve(&soap);

		// Clean up!
		soap_end(&soap);
	}

  return 0;
}

//(struct soap *soap, int a, int b, int *res)
int psdims__user_register(struct soap *soap, char *name, char *passwd,char *description, int *ERRCODE){
	*ERRCODE = 10;
	add_user(bd,get_cont(bd),name,passwd,description);
    sum_cont(bd);
	return SOAP_OK; 
}

// borrar user
int psdims__user_unregister(struct soap *soap, char *name, int *ERRCODE){
	*ERRCODE = 11;
	del_user(bd,name);
	return SOAP_OK; 
}

// enviar solicitud de amistad a usuario
int psdims__friend_request(struct soap *soap, char *name, char *passwd, char* request_name, int *ERRCODE){
	*ERRCODE = 12;

	return SOAP_OK; 
}

// recibir solicitudes de amistad pendientes
int psdims__get_requests(struct soap *soap, char *name, char *passwd, psdims__request_list *requests){

	return SOAP_OK; 
}

// aceptar solicitud de amistad
int psdims__accept_request(struct soap *soap, char *name, char *passwd, char *request_name, int *ERRCODE){
	*ERRCODE = 14;
	return SOAP_OK; 
}

// rechazar solicitud de amistad
int psdims__decline_request(struct soap *soap, char *name, char *passwd, char *request_name, int *ERRCODE){
	*ERRCODE = 15;
	return SOAP_OK; 
}
