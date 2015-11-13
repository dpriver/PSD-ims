/*******************************************************************************
 *	ims.h
 *
 *  gsoap declarations
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

//gsoap psdims service name: psdims
//gsoap psdims service style: rpc
//gsoap psdims service location: http://localhost:10000
//gsoap psdims service encoding: literal
//gsoap psdims service namespace: urn:psdims


typedef struct psdims__request_list{
	int __sizenelems;	
	char **name;
} psdims__request_list;


/********************************************************************
 * Basicas
 ********************************************************************/
// register user
int psdims__user_register(char *name, char *passwd, int *ERRCODE);

// borrar user
int psdims__user_unregister(char *name, char *passwd, int *ERRCODE);

// enviar solicitud de amistad a usuario
int psdims__friend_request(char *name, char *passwd, char* request_name, int *ERRCODE);

// recibir solicitudes de amistad pendientes
int psdims__get_requests(char *name, char *passwd, psdims__request_list *requests);

// aceptar solicitud de amistad
int psdims__accept_request(char *name, char *passwd, char *request_name, int *ERRCODE);

// rechazar solicitud de amistad
int psdims__decline_request(char *name, char *passwd, char *request_name, int *ERRCODE);


/********************************************************************
 * Mensajes
 ********************************************************************/
// enviar mensaje de texto a amigo
// recibir mensajes de texto pendientes
// recibir confirmación de entrega de mensajes enviados

/********************************************************************
 * Grupos
 ********************************************************************/
// crear grupo
// pasar a otro user el rol de admin
// añadir amigo a un grupo del que soy admin
// borrar amigo de un grupo del que soy admin
// recibir aviso de mi adicion o borrado de grupo
// enviar mensaje a grupo (igual que enviar mensaje a usuario)
