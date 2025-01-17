//
// Created by 14049 on 2024/6/13.
//

#pragma once

#include <stdbool.h>
#include <xcl/util/comparator.h>
#include <xcl/util/tree_iter.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Map Map;
Map* Map_new(unsigned key_size, unsigned value_size, Comparator* key_comp);
void Map_free(Map* map);
Map* Map_copy(Map* map);
bool Map_move(Map* map, Map* obj);
unsigned Map_size(Map* map);
bool Map_empty(Map* map);
TreeIter Map_add(Map* map, bool unique, const void* key, const void* value);
bool Map_remove(Map* map, const void* key);
void Map_removeIter(Map* map, TreeIter iter);
void* Map_get(Map* map, const void* key);
TreeIter Map_begin(Map* map);
TreeIter Map_end(Map* map);
TreeIter Map_find(Map* map, const void* key);
void Map_range(Map* map, const void* key, TreeIter out[2]);
void Map_clear(Map* map);
void Map_swap(Map* map1, Map* map2);
void Map_verify(Map* map);
bool Map_equal(Map* map1, Map* map2);

#ifdef __cplusplus
}
#endif
