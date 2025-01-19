//
// Created by 14049 on 2024/6/13.
//

#include <internal/util/rb.h>
#include <internal/util/tree_impl.h>
#include <string.h>
#include <xcl/lang/alloc.h>
#include <xcl/util/log.h>
#include <xcl/util/map.h>

struct Map {
  struct TreeImpl impl;
};

Map* Map_new(unsigned key_size, unsigned value_size, Comparator* key_comp) {
  Map* map = palloc(sizeof(Map));
  if (map)
    TreeImpl_init(&map->impl, key_size, key_size + value_size, key_comp, NULL);
  return map;
}
void Map_free(Map* map) {
  if (map) {
    Map_clear(map);
    pfree(map);
  }
}
Map* Map_copy(Map* map) {
  Map* copy =
      Map_new(map->impl.key_size, map->impl.item_size - map->impl.key_size, map->impl.key_comp);
  if (copy && TreeImpl_copy(&copy->impl, &map->impl))
    return copy;
  Map_free(copy);
  return NULL;
}
bool Map_move(Map* map, Map* obj) {
  if (!Map_empty(map))
    return false;
  TreeImpl_move(&map->impl, &obj->impl);
  return true;
}
unsigned Map_size(Map* map) { return map->impl.size; }
bool Map_empty(Map* map) { return map->impl.size == 0; }
TreeIter Map_add(Map* map, bool unique, const void* key, const void* value) {
  struct RbNode* node = TreeImpl_reserve(&map->impl, unique, key);
  if (node) {
    char* item = RbNode_value(node);
    memcpy(item, key, map->impl.key_size);
    memcpy(&item[map->impl.key_size], value, map->impl.item_size - map->impl.key_size);
  }
  return TreeIter_make(node ? node : &map->impl.header, map);
}
bool Map_remove(Map* map, const void* key) {
  struct RbNode* node = TreeImpl_find(&map->impl, key);
  if (node)
    TreeImpl_remove(&map->impl, node);
  return node;
}
void Map_removeIter(Map* map, TreeIter iter) {
  if (iter.impl == map)
    TreeImpl_remove(&map->impl, iter.cur);
}
void* Map_get(Map* map, const void* key) {
  struct RbNode* node = TreeImpl_find(&map->impl, key);
  return node ? (char*)RbNode_value(node) + map->impl.key_size : NULL;
}
TreeIter Map_begin(Map* map) { return TreeImpl_begin(&map->impl); }
TreeIter Map_end(Map* map) { return TreeImpl_end(&map->impl); }
TreeIter Map_find(Map* map, const void* key) {
  struct RbNode* node = TreeImpl_find(&map->impl, key);
  return TreeIter_make(node ? node : &map->impl.header, map);
}
void Map_range(Map* map, const void* key, TreeIter out[2]) {
  struct RbNode *x = TreeImpl_lower(&map->impl, key), *y = TreeImpl_upper(&map->impl, key);
  out[0] = TreeIter_make(x, map);
  out[1] = TreeIter_make(y, map);
}
void Map_clear(Map* map) { TreeImpl_clear(&map->impl); }
void Map_swap(Map* map1, Map* map2) { TreeImpl_swap(&map1->impl, &map2->impl); }
void Map_verify(Map* map) { TreeImpl_verify(&map->impl); }
bool Map_equal(Map* map1, Map* map2) { return TreeImpl_equal(&map1->impl, &map2->impl); }
