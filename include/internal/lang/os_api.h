#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

char* __Env_osGet(const char* name);
bool __Env_osSet(const char* name, const char* value);
char __Env_seperator();

#ifdef __cplusplus
}
#endif
