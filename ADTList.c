#include <stdlib.h>
#include "ADTList.h"

struct list{
	ListNode dummy, last;				
	int size;					
	DestroyFunc destroy_value;	
};

struct list_node{
	ListNode next;		
	Pointer value;		
};


List list_create(DestroyFunc destroy_value){
	List list = malloc(sizeof(*list));

	list->dummy = malloc(sizeof(*list->dummy));
	list->dummy->next = NULL;
	list->last = list->dummy;
	list->size = 0, list->destroy_value = destroy_value;

	return list;
}

int list_size(List list){
	return list->size;
}

void list_insert_next(List list, ListNode node, Pointer value){
	if(node == NULL)
		node = list->dummy;

	ListNode nd = malloc(sizeof(*nd));

	nd->value = value;
	nd->next = node->next;
	node->next = nd;
	list->size++;

	if(list->last == node)
		list->last = nd;
}

void list_remove_next(List list, ListNode node){
	if(node == NULL)
		node = list->dummy;

	ListNode removed = node->next;
	if(list->destroy_value != NULL)
		list->destroy_value(removed->value);

	node->next = removed->next;		
	free(removed);
	list->size--;

	if(list->last == removed)
		list->last = node;
}

void list_remove(List list, ListNode node){
	ListNode prv = list->dummy;
	for(ListNode locnode = list->dummy->next; locnode != NULL; locnode = locnode->next){
		if(node == locnode){
			prv->next = node->next;
			if(list->destroy_value != NULL) list->destroy_value(node);
			list->size--;
			break;
		}
		prv = locnode;
	}
}

Pointer list_find(List list, Pointer value, CompareFunc compare){
	ListNode node = list_find_node(list, value, compare);
	if(node == NULL)
		return NULL;
	return node->value;
}

DestroyFunc list_set_destroy_value(List list, DestroyFunc destroy_value){
	DestroyFunc old = list->destroy_value;
	list->destroy_value = destroy_value;
	return old;
}

void list_destroy(List list){
	ListNode node = list->dummy;

	while(node != NULL){				
		ListNode next = node->next;		
		if(node != list->dummy && list->destroy_value != NULL)
			list->destroy_value(node->value);

		free(node);
		node = next;
	}
	free(list);
}



ListNode list_first(List list){
	return list->dummy->next;
}

ListNode list_last(List list) {
	if (list->last == list->dummy)
		return LIST_EOF;	
		
	return list->last;
}

ListNode list_next(List list, ListNode node){
	return node->next;
}

Pointer list_node_value(List list, ListNode node){
	return node->value;
}

ListNode list_find_node(List list, Pointer value, CompareFunc compare){
	for(ListNode node = list->dummy->next; node != NULL; node = node->next)
		if(compare(value, node->value) == 0)
			return node;	

	return NULL;
}