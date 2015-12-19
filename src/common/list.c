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
	node->prev->next = node->next;
	node->next->prev = node->prev;
	
	item_free(node->item);
	free(node);
}

int _node_add(list *list, list_node *node) {
	node->next = list->ghost_item;
	node->prev = list->ghost_item->prev;
	list->ghost_item->prev->next = node;
	list->ghost_item->prev = node;
	
	return 0;
}

list_node *_find_node(list *list, void *comp_val, int (*comp)(void *item, void *val)) {
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
	list *new_list;
	
	if ((info_free == NULL) || (item_free == NULL)) {
		return NULL;
	}
	
	new_list = malloc(sizeof(list));
	if (new_list == NULL) {
		return NULL;
	}
	
	new_list->info_free = info_free;
	new_list->item_free = item_free;
	
	new_list->max_elems = max_elems;
	new_list->n_elems = 0;
	new_list->list_info = list_info;
	
	return new_list;
}


void list_free(list *list) {
	list_node *node;
	
	node = list->ghost_item;
	while (node != node->next) {
		_node_delete(node->next, list->item_free);
	}
	
	free(node);
	list->info_free(list->list_info);
	free(list);
}


list_node *list_find_node(list *list, void *comp_val) {
	return _find_node(list, comp_val, list->item_comp);
}

	
void *list_find_item(list *list, void *comp_val) {
	list_node *node;
	node = _find_node(list, comp_val, list->item_comp);
	
	return (node != NULL)? node->item : NULL;
}


int list_add_item(list *list, void *item) {
	list_node *node;
	
	node = malloc(sizeof(list_node));
	if (node == NULL) {
		return -1;
	}
	
	node->item = item;
	_node_add(list, node);
	
	return 0;
}


void list_delete_node(list *list, list_node *node) {
	_node_delete(node, list->item_free);
}
