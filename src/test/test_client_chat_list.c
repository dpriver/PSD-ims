/*******************************************************************************
 *	friends.c
 *
 *  Program to test the client friend list
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

#include <string.h>
#include <stdio.h>

#include "friends.h"
#include "leak_detector_c.h"


void print_chats(chat_list *list);

int main (int argc, char **argv) {
	
	chat_list *chats;
	chat_info *aux_chat_info;


	atexit(report_mem_leak);

	chats = cha_lst_new();

	printf("= Printing empty list =====\n");
	print_chats(chats)
	

	


	cha_lst_free(chats);
	
}

void print_chats(chat_list *list) {
	printf("-------------------------------"\n);
	cha_lst_print(list);
	printf("-------------------------------"\n),
}
