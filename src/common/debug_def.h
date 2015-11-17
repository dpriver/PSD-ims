/*******************************************************************************
 *	debug_def.h
 *
 *  Definition for debuging
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

#ifdef DEBUG

#define DEBUG_PRINT_FUNCTION() \
	printf("[DEBUG]: \"%s\" %d: (FUNCTION <%s>)\n",__FILE__, __LINE__, __func__)

#define DEBUG_PRINTF(literal_string, ...) \
		printf("[DEBUG]: \"%s\" %d: " literal_string "\n",__FILE__, __LINE__, ##__VA_ARGS__)

#else

#define DEBUG_PRINT_FUNCTION()

#define DEBUG_PRINT(string)

#endif
