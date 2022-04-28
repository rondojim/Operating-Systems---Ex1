#include <stdlib.h>

#include "ADTMap.h"
#include "ADTList.h"

#include <stdio.h>
#include <assert.h>

int prime_sizes[] = {53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241,
	786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741};

#define MAX_LOAD_FACTOR 0.9

struct map_node{
	Pointer key;		
	Pointer value;  	
};

struct map {
	List* array;				
	int capacity;				
	int size;					
	CompareFunc compare;		
	HashFunc hash_function;		
	DestroyFunc destroy_key;	
	DestroyFunc destroy_value;
};


Map map_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value) {
	Map map = malloc(sizeof(*map));
	map->capacity = prime_sizes[0];
	map->array = malloc(map->capacity * sizeof(List));


	for(uint i=0; i<map->capacity; ++i) map->array[i] = list_create(NULL);

	map->size = 0;
	map->compare = compare;
	map->destroy_key = destroy_key;
	map->destroy_value = destroy_value;

	return map;
}

int map_size(Map map) {
	return map->size;
}



static void rehash(Map map) {
	int old_capacity = map->capacity;
	List* old_array = map->array;

	map->capacity *= 2;								

	map->array = malloc(map->capacity * sizeof(List));
	for (uint i = 0; i < map->capacity; i++) map->array[i] = list_create(NULL);

	map->size = 0;
	for (uint i = 0; i < old_capacity; i++){
		for(ListNode nd = list_first(old_array[i]); nd != LIST_EOF; nd = list_next(old_array[i], nd)){
			MapNode mnd = (MapNode)list_node_value(old_array[i], nd);
			map_insert(map, mnd->key, mnd->value);
		}
	}
	free(old_array);
}

void map_insert(Map map, Pointer key, Pointer value) {
	uint pos = map->hash_function(key) % map->capacity;

	ListNode nd;
	MapNode mnd;
	bool already_in_map = false;
	for(nd = list_first(map->array[pos]); nd != LIST_EOF; nd = list_next(map->array[pos], nd)){
		mnd = (MapNode)list_node_value(map->array[pos], nd);
		if (map->compare(mnd->key, key) == 0){
			already_in_map = true;
			break;
		}
	}

	if (already_in_map) {
		if (mnd->key != key && map->destroy_key != NULL)
			map->destroy_key(mnd->key);

		if (mnd->value != value && map->destroy_value != NULL)
			map->destroy_value(mnd->value);
			
		mnd->key = key;
		mnd->value = value;
	}
	else {
		mnd = malloc(sizeof(mnd));
		mnd->key = key;
		mnd->value = value;
		list_insert_next(map->array[pos], list_last(map->array[pos]), mnd);
		map->size++;
	}

	float load_factor = (float)map->size / map->capacity;
	if (load_factor > MAX_LOAD_FACTOR)
		rehash(map);
}


bool map_remove(Map map, Pointer key) {
	uint pos = map->hash_function(key) % map->capacity;
	ListNode prev = LIST_BOF;
	ListNode nd = list_first(map->array[pos]);
	MapNode mnd;
	bool found = false;
	for(; nd != LIST_EOF; nd = list_next(map->array[pos], nd)){
		mnd = (MapNode)list_node_value(map->array[pos], nd);
		if(map->compare(mnd->key, key) == 0){
			found = true;
			break;
		}
		prev = nd;
	}
	if(found == false) return false;

	if (map->destroy_key != NULL)
		map->destroy_key(mnd->key);
	if (map->destroy_value != NULL)
		map->destroy_value(mnd->value);

	list_remove_next(map->array[pos], prev);

	return true;
}

Pointer map_find(Map map, Pointer key) {
	MapNode node = map_find_node(map, key);
	if (node != MAP_EOF)
		return node->value;
	else
		return NULL;
}


DestroyFunc map_set_destroy_key(Map map, DestroyFunc destroy_key) {
	DestroyFunc old = map->destroy_key;
	map->destroy_key = destroy_key;
	return old;
}

DestroyFunc map_set_destroy_value(Map map, DestroyFunc destroy_value) {
	DestroyFunc old = map->destroy_value;
	map->destroy_value = destroy_value;
	return old;
}

void map_destroy(Map map) {
	for (uint i = 0; i < map->capacity; i++) {
		for(ListNode nd = list_first(map->array[i]); nd != LIST_EOF; nd = list_next(map->array[i], nd)){
			MapNode mnd = (MapNode)list_node_value(map->array[i], nd);
			if (map->destroy_key != NULL)
				map->destroy_key(mnd->key);
			if (map->destroy_value != NULL)
				map->destroy_value(mnd->value);
		}
		list_destroy(map->array[i]);
	}
	free(map->array);
	free(map);
}

MapNode map_first(Map map) {
	for (uint i = 0; i < map->capacity; i++)
		for(ListNode nd = list_first(map->array[i]); nd != LIST_EOF; nd = list_next(map->array[i], nd))
			return (MapNode)list_node_value(map->array[i], nd);

	return MAP_EOF;
}

MapNode map_next(Map map, MapNode node) {
	uint pos = map->hash_function(node->key) % map->capacity;
	for(ListNode nd = list_first(map->array[pos]); nd != LIST_EOF; nd = list_next(map->array[pos], nd)){
		MapNode mnd = (MapNode)list_node_value(map->array[pos], nd);
		if(map->compare(mnd->key, node->key) == 0){
			ListNode nd2 = list_next(map->array[pos], nd);
			if(nd2 != LIST_EOF){
				mnd = *(MapNode*)list_node_value(map->array[pos], nd2);
				return mnd;
			}
			else break;
		}
	}
	for(uint i = pos + 1; i<map->capacity; ++i){
		if(list_size(map->array[i]) != 0){
			MapNode mnd = (MapNode)list_node_value(map->array[i], list_first(map->array[i]));
			return mnd;
		}
	}
	return MAP_EOF;
}

Pointer map_node_key(Map map, MapNode node) {
	return node->key;
}

Pointer map_node_value(Map map, MapNode node) {
	return node->value;
}

MapNode map_find_node(Map map, Pointer key) {
	uint pos = map->hash_function(key) % map->capacity;
	for(ListNode nd = list_first(map->array[pos]); nd != LIST_EOF; nd = list_next(map->array[pos], nd)){
		MapNode mnd = (MapNode)list_node_value(map->array[pos], nd);
		if(map->compare(mnd->key, key) == 0)
			return mnd;
	}
	return MAP_EOF;
}

void map_set_hash_function(Map map, HashFunc func) {
	map->hash_function = func;
}

uint hash_int(Pointer value) {
	return *(int*)value;
}