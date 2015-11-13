/*******************************************************************************
 *	persistance.h
 *
 *  functionality to allow the server-side persistance of data
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
#include <mysql>
//#include <list.h>

/*Add new user,return 1 if error or 0 if no*/
int add_user(int id,char* name,char* information);

/*Del new user,return 1 if error or 0 if no*/
int del_user(int id);

/*Request of id1 to be friend of id2,return 1 if error or 0 if no*/
int send_request(int id1,int id2);

/*Return the list of request pendind of id*/
list<int> get_reguest_pending(int id);

/*Accept id1 the request of friendly id2,return 1 if error or 0 if no*/
int accept_friend(int id1,int id2);

/*Refuse id1 the request of friendly id2,return 1 if error or 0 if no*/
int refuse_friend(int id1,int id2);




