//
// Created by 14049 on 2024/6/3.
//

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char ctx[16 + sizeof(intptr_t)];
} String;

String String_make();
void String_destroy(String* string);
String String_from(const char* s, intptr_t n);
bool String_filln(String* string, uintptr_t n, char ch);
bool String_fill(String* string, const char* s, intptr_t n);
bool String_insertn(String* string, uintptr_t pos, uintptr_t n, char ch);
bool String_insert(String* string, uintptr_t pos, const char* s, intptr_t n);
void String_clear(String* string);
void String_erase(String* string, uintptr_t pos, intptr_t n);
int String_findc(String* string, uintptr_t pos, char ch);
int String_rfindc(String* string, uintptr_t pos, char ch);
int String_finds(String* string, uintptr_t pos, const char* s);
bool String_reserve(String* string, uintptr_t n);
int String_compare(String* string1, String* string2);
int String_compare2(String* string1, uintptr_t pos1, String* string2, uintptr_t pos2,
                    uintptr_t len);
const char* String_str(String* string);
bool String_push(String* string, char ch);
char String_pop(String* string);
char* String_at(String* string, uintptr_t pos);
bool String_appendn(String* string, uintptr_t n, char ch);
bool String_append(String* string, const char* s, intptr_t n);
char* String_back(String* string);
uintptr_t String_size(String* string);
uintptr_t String_capacity(String* string);
bool String_empty(String* string);

#ifdef __cplusplus
}
#endif
