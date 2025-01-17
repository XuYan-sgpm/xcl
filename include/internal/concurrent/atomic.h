#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

char _load8(volatile char* obj);
void _store8(volatile char* obj, char val);
char _fetch_add8(volatile char* obj, char delta);
char _fetch_sub8(volatile char* obj, char delta);
char _exchange8(volatile char* obj, char val);
bool _cas8(volatile char* obj, char* expected, char desired);

short _load16(volatile short* obj);
void _store16(volatile short* obj, short val);
short _fetch_add16(volatile short* obj, short delta);
short _fetch_sub16(volatile short* obj, short delta);
short _exchange16(volatile short* obj, short val);
bool _cas16(volatile short* obj, short* expected, short desired);

int _load32(volatile int* obj);
void _store32(volatile int* obj, int val);
int _fetch_add32(volatile int* obj, int delta);
int _fetch_sub32(volatile int* obj, int delta);
int _exchange32(volatile int* obj, int val);
bool _cas32(volatile int* obj, int* expected, int desired);

void* _load_p(void* volatile* obj);
void _store_p(void* volatile* obj, void* ptr);
void* _exchange_p(void* volatile* obj, void* ptr);
bool _cas_p(void* volatile* obj, void** expected, void* desired);

int64_t _load64(volatile int64_t* obj);
void _store64(volatile int64_t* obj, int64_t val);
int64_t _fetch_add64(volatile int64_t* obj, int64_t delta);
int64_t _fetch_sub64(volatile int64_t* obj, int64_t delta);
int64_t _exchange64(volatile int64_t* obj, int64_t val);
bool _cas64(volatile int64_t* obj, int64_t* expected, int64_t desired);

#ifdef __cplusplus
}
#endif
