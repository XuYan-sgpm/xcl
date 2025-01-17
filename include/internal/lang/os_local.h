#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define INVALID_LOCAL_KEY (~0ul)
void* osLocalGet(unsigned long key);
void osLocalSet(unsigned long key, void* obj);
unsigned long allocLocalKey();
void destroyLocalKey(unsigned long key);

#ifdef __cplusplus
}
#endif