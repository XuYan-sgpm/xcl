//
// Created by 14049 on 2023/10/18.
//

#include <Windows.h>
#include <internal/lang/local_storage.h>
#include <xcl/util/log.h>

static void __stdcall Fls_callback(void* args) { LocalStorage_free(args); }
unsigned long allocLocalKey() { return FlsAlloc(Fls_callback); }
void destroyLocalKey(unsigned long key) { FlsFree(key); }
void* osLocalGet(unsigned long key) { return FlsGetValue(key); }
void osLocalSet(unsigned long key, void* obj) { FlsSetValue(key, obj); }
