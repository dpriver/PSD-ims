/*******************************************************************************
 *  list.h
 *
 *  A generic list implementation
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

#ifndef __GENERIC_LIST
#define __GENERIC_LIST

typedef struct list_node list_node;
struct list_node {
	void *item;
	struct list_node *next;
	struct list_node *prev;
};

typedef struct linked_list list;
struct linked_list {
	list_node *ghost_item;
	void *list_info;
	int n_elems;
	int max_elems;
	void (*info_free)(void *info);		// function to free the list info
	void (*item_free)(void *item);		// function to free the list items
	int (*item_comp)(void *item1, void *item2);	// function to set an order
	int (*item_value_comp)(void *item, void *value);	// function to comp an item with a value
};


/* =========================================================================
 *  List access macros
 * =========================================================================*/

#define list_num_elems(list_ptr) 		(list->n_elems)
#define list_max_elems(list_ptr) 		(list->max_elems)
#define list_info(list_ptr) 			(list->list_info)



/* =========================================================================
 *  List functions
 * =========================================================================*/
 
list *list_new(void *list_info, int max_elems, void (*info_free)(void *info), void (*item_free)(void *item));

void list_free(list *list);

list_node *list_find_node(list *list, void *comp_val);
	
void *list_find_item(list *list, void *comp_val);

int list_add_item(list *list, void *item);

void list_delete_node(list *list, list_node *node);




#endif /* __GENERIC_LIST */
