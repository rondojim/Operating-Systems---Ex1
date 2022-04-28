#include "common_types.h"
#include <stdbool.h>

typedef struct map* Map;

Map map_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value);

int map_size(Map map);

void map_insert(Map map, Pointer key, Pointer value);

bool map_remove(Map map, Pointer key);

Pointer map_find(Map map, Pointer key);

DestroyFunc map_set_destroy_key  (Map map, DestroyFunc destroy_key  );
DestroyFunc map_set_destroy_value(Map map, DestroyFunc destroy_value);

void map_destroy(Map map);


#define MAP_EOF (MapNode)0

typedef struct map_node* MapNode;

MapNode map_first(Map map);

MapNode map_next(Map map, MapNode node);

Pointer map_node_key(Map map, MapNode node);

Pointer map_node_value(Map map, MapNode node);

MapNode map_find_node(Map map, Pointer key);

typedef uint (*HashFunc)(Pointer);

uint hash_int(Pointer value);

void map_set_hash_function(Map map, HashFunc hash_func);