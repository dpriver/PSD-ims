/*******************************************************************************
 *  list.c
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


#include <stdlib.h>
#include "list.h"

#include "debug_def.h"

#ifdef DEBUG
#include "leak_detector_c.h"
#endif

void _node_delete(list_node *node, void (*item_free)(void *item)) {
	DEBUG_TRACE_PRINT();
	node->prev->next = node->next;
	node->next->prev = node->prev;
	
	item_free(node->item);
	free(node);
}

int _node_add(list *list, list_node *node) {
	DEBUG_TRACE_PRINT();
	node->next = list->ghost_item;
	node->prev = list->ghost_item->prev;
	list->ghost_item->prev->next = node;
	list->ghost_item->prev = node;
	
	return 0;
}

list_node *_find_node(list *list, const void *comp_val, int (*comp)(const void *item, const void *val)) {
	DEBUG_TRACE_PRINT();
	list_node *node;
	
	node = list->ghost_item->next;
	while (node != list->ghost_item) {
		if (comp(node->item, comp_val) == 0) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}


/* =========================================================================
 *  List functions
 * =========================================================================*/
 
list *list_new(void *list_info, int max_elems, void (*info_free)(void *info), void (*item_free)(void *item)) {
	DEBUG_TRACE_PRINT();
	list *new_list;
	
	if ((info_free == NULL) || (item_free == NULL)) {
		DEBUG_FAILURE_PRINTF("Could not create list, NULL free functions");
		return NULL;
	}
	
	new_list = malloc(sizeof(list));
	if (new_list == NULL) {
		return NULL;
	}
	
	new_list->ghost_item = malloc(sizeof(list_node));
	if (new_list->ghost_item == NULL) {
		free(new_list);
		return NULL;
	}
	new_list->ghost_item->item = NULL;
	new_list->ghost_item->next = new_list->ghost_item;
	new_list->ghost_item->prev = new_list->ghost_item;
	
	new_list->info_free = info_free;
	new_list->item_free = item_free;
	
	new_list->max_elems = max_elems;
	new_list->n_elems = 0;
	new_list->list_info = list_info;
	
	return new_list;
}


void list_free(list *list) {
	DEBUG_TRACE_PRINT();
	list_node *node;
	
	if ((list->item_free == NULL) || (list->info_free == NULL) ) {
		DEBUG_FAILURE_PRINTF("Could not free the list, free functions not defined");
		return;
	}
	
	node = list->ghost_item;
	while (node != node->next) {
		_node_delete(node->next, list->item_free);
	}
	
	free(node);
	list->info_free(list->list_info);
	free(list);
}


list_node *list_find_node(list *list, const void *comp_val) {
	DEBUG_TRACE_PRINT();
	if (list->item_value_comp == NULL) {
		DEBUG_FAILURE_PRINTF("Can not search, item_value_comp not defined");
		return NULL;
	}
	return _find_node(list, comp_val, list->item_value_comp);
}

	
void *list_find_item(list *list, const void *comp_val) {
	DEBUG_TRACE_PRINT();
	list_node *node;
	if (list->item_value_comp == NULL) {
		DEBUG_FAILURE_PRINTF("Can not search, item_value_comp not defined");
		return NULL;
	}
	node = _find_node(list, comp_val, list->item_value_comp);
	
	return (node != NULL)? node->item : NULL;
}


int list_add_item(list *list, void *item) {
	DEBUG_TRACE_PRINT();
	list_node *node;
	
	if (item == NULL) {
		DEBUG_FAILURE_PRINTF("The added item cannot be null");
		return -1;
	}

	if (list->item_comp == NULL) {
		DEBUG_FAILURE_PRINTF("Can not search, item_value_comp not defined");
		return -1;
	}
	
	if (list->n_elems >= list->max_elems) {
		DEBUG_FAILURE_PRINTF("The list is full");
		return -1;
	}
	
	if (_find_node(list, item, list->item_comp) != NULL) {
		DEBUG_FAILURE_PRINTF("The item does exist in the list");
		return -1;
	}
	
	node = malloc(sizeof(list_node));
	if (node == NULL) {
		return -1;
	}
	
	node->item = item;
	_node_add(list, node);
	list->n_elems++;
	
	return 0;
}


void list_delete_node(list *list, list_node *node) {
	DEBUG_TRACE_PRINT();
	_node_delete(node, list->item_free);
	list->n_elems--;
}


void list_delete_first(list *list, int num_elems) {
	DEBUG_TRACE_PRINT();
	for( num_elems ; num_elems > 0 ; num_elems-- ) {
		if(list->ghost_item->next == list->ghost_item) {
			return;	// if the list is empty
		}
		_node_delete(list->ghost_item->next, list->item_free);
		list->n_elems--;
	}
}


void list_delete_last(list *list, int num_elems) {
	DEBUG_TRACE_PRINT();
	for( num_elems ; num_elems > 0 ; num_elems-- ) {
		if(list->ghost_item->prev == list->ghost_item) {
			return;	// if the list is empty
		}
		_node_delete(list->ghost_item->prev, list->item_free);
		list->n_elems--;
	}
}
