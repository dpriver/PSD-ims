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


#define OFF					0
#define BOLD				1
#define UNDERSCORE	2
#define BLINK				3
#define REVERSE			4
#define CONCEALED		5

#define FBLACK		30
#define FRED			31
#define FGREEN		32
#define FYELLOW	33
#define FBLUE		34
#define FMAGENTA	35
#define FCYAN		36
#define FWHITE		37

#define BBLACK		40
#define BRED			41
#define BGREEN		42
#define BYELLOW	43
#define BBLUE		44
#define BMAGENTA	45
#define BCYAN		46
#define BWHITE		47


#define D_ATTR			BOLD
#define I_ATTR			BOLD
#define T_ATTR			BOLD
#define F_ATTR			BOLD

#define D_FCOLOR			FBLUE
#define I_FCOLOR			FBLUE
#define T_FCOLOR			FYELLOW
#define F_FCOLOR			FRED



#define TEXT_RESET 0x1b[0m



#ifdef DEBUG

	#define DEBUG_PRINTF(literal_string, ...) \
		printf("[%c[%d;%dmDEBUG%c[%dm]: \"%s\" %d: " literal_string "\n", \
			0x1b, D_ATTR, D_FCOLOR, 0x1b, 0, __FILE__, __LINE__, ##__VA_ARGS__)

	#ifdef DEBUG_TRACE
		#define DEBUG_TRACE_PRINT() \
			printf("[%c[%d;%dmTRACE%c[%dm]: \"%s\" %d: (FUNCTION %c[%d;%dm<%s>%c[%dm)\n", \
				0x1b, T_ATTR, T_FCOLOR, 0x1b, 0, __FILE__, __LINE__, 0x1b, OFF, T_FCOLOR, __func__, 0x1b, 0)	
	#else
		#define DEBUG_TRACE_PRINT()
	#endif

	#ifdef DEBUG_INFO
		#define DEBUG_INFO_PRINTF(literal_string, ...) \
			printf("[%c[%d;%dmINFO%c[%dm]: \"%s\" %d: " literal_string "\n", \
				0x1b, I_ATTR, I_FCOLOR,0x1b, 0, __FILE__, __LINE__, ##__VA_ARGS__)
	#else
		#define DEBUG_INFO_PRINTF(literal_string, ...)
	#endif

	#ifdef DEBUG_FAILURE
		#define DEBUG_FAILURE_PRINTF(literal_string, ...) \
			printf("[%c[%d;%dmFAILURE%c[%dm]: \"%s\" %d: " literal_string "\n", \
				0x1b, F_ATTR, F_FCOLOR, 0x1b, 0, __FILE__, __LINE__, ##__VA_ARGS__)
	#else
		#define DEBUG_FAILURE_PRINTF(literal_string, ...)
	#endif

#else

#define DEBUG_PRINTF(literal_string, ...)
#define DEBUG_TRACE_PRINT()
#define DEBUG_INFO_PRINTF(literal_string, ...)
#define DEBUG_FAILURE_PRINTF(literal_string, ...)

#endif
