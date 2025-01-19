#include <internal/util/handle_table.h>
#include <internal/xcl.h>

void* Handle_new(void* obj, void (*destructor)(void*), const char* name) {
  return HandleTable_map(__xcl_handle_table, obj, destructor, name);
}

void Handle_close(void* h) { HandleTable_close(__xcl_handle_table, h); }

void* Handle_value(void* h) { return HandleTable_get(__xcl_handle_table, h); }

void* Handle_ref(void* h) { return HandleTable_clone(__xcl_handle_table, h); }

void* Handle_refName(const char* name) { return HandleTable_cloneByName(__xcl_handle_table, name); }

bool Handle_hasName(const char* name) { return HandleTable_contains(__xcl_handle_table, name); }

void Handle_closeName(const char* name) { HandleTable_closeByName(__xcl_handle_table, name); }

void Handle_closeAll() { HandleTable_clear(__xcl_handle_table); }
