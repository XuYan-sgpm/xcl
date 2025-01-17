#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SYMBOL_NAME 64

void* DlModule_new();
void DlModule_free(void* obj);
bool DlModule_open(void* obj, const char* name, ...);
void* DlModule_symbol(void* obj, const char* symbol);
void DlModule_close(void* obj);
const char* DlModule_name(void* obj);
bool DlModule_isOpened(void* obj, const char* name);

#ifdef __cplusplus
}
#endif
