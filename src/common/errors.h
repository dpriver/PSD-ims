/*******************************************************************************
 *  errors.h
 *
 *  Error definitions and macros
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

#define N_ERRORS (3)
#define MAX_ERROR_MSG_CHARS (50)

enum {
	ERR_SRV_LOGIN = 0,
	ERR_SRV_EMPTY_FILEDS,
	ERR_SRV_BAD_FIELDS
	} error_type;


char error_msgs[N_ERRORS][MAX_ERROR_MSG_CHARS] = {
	"Login incorrect",
	"There are empty mandatory fields",
	"Some files have incorrect values"
};


#define printerror(err_code) \
		printf(" %s", error_msgs[err_code])
