/*******************************************************************************
 *	psd_ims_server.h
 *
 *  Server network management
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

#ifndef __PSD_IMS_SERVER
#define __PSD_IMS_SERVER

#include "soapH.h"
#include "persistence.h"
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_FILE_CHARS (10485760)


int init_server(int bind_port, char persistence_user[], char persistence_pass[]);

void free_server();

int listen_connection();


#endif /* __PSD_IMS_SERVER */
