#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void* Handle_new(void* obj, void (*destructor)(void*), const char* name);
void Handle_close(void* h);
void* Handle_value(void* h);
void* Handle_ref(void* h);
void* Handle_refName(const char* name);
bool Handle_hasName(const char* name);
void Handle_closeName(const char* name);
void Handle_closeAll();

#ifdef __cplusplus
}
#endif
