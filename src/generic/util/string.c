//
// Created by 14049 on 2024/6/3.
//

#include <internal/util/str_impl.h>
#include <stdlib.h>
#include <string.h>
#include <xcl/util/log.h>
#include <xcl/util/string.h>

static inline StrImpl* str2impl(String* string) {
  if (sizeof(String) != sizeof(StrImpl)) {
    LOG_P(LOG_ERROR, "sizeof(String) != sizeof(StrImpl)");
    abort();
  }
  return (void*)string;
}

String String_make() {
  StrImpl o = StrImpl_make(false);
  return *(String*)&o;
}
void String_destroy(String* string) { StrImpl_destroy(str2impl(string)); }
String String_from(const char* s, intptr_t n) {
  StrImpl str_impl = StrImpl_make(false);
  StrImpl_fill(&str_impl, s, n < 0 ? strlen(s) : n);
  return *(String*)&str_impl;
}
bool String_filln(String* string, uintptr_t n, char val) {
  return StrImpl_filln(str2impl(string), n, val);
}
bool String_fill(String* string, const char* s, intptr_t n) {
  return StrImpl_fill(str2impl(string), s, n < 0 ? strlen(s) : n);
}
bool String_insertn(String* string, uintptr_t pos, uintptr_t n, char val) {
  return StrImpl_insertn(str2impl(string), pos, n, val);
}
bool String_insert(String* string, uintptr_t pos, const char* s, intptr_t n) {
  return StrImpl_insert(str2impl(string), pos, s, n < 0 ? strlen(s) : n);
}
void String_clear(String* string) { str2impl(string)->size = 0; }
void String_erase(String* string, uintptr_t pos, intptr_t n) {
  StrImpl_erase(str2impl(string), pos, n < 0 ? str2impl(string)->size - pos : n);
}
int String_findc(String* string, uintptr_t pos, char val) {
  return StrImpl_findc(str2impl(string), false, pos, val);
}
int String_rfindc(String* string, uintptr_t pos, char val) {
  return StrImpl_findc(str2impl(string), true, pos, val);
}
int String_finds(String* string, uintptr_t pos, const char* s) {
  return StrImpl_finds(str2impl(string), pos, s);
}
bool String_reserve(String* string, uintptr_t n) { return StrImpl_reserve(str2impl(string), n); }
int String_compare(String* string1, String* string2) {
  return strcmp(String_str(string1), String_str(string2));
}
int String_compare2(String* string1, uintptr_t pos1, String* string2, uintptr_t pos2,
                    uintptr_t len) {
  return StrImpl_ncmp(str2impl(string1), pos1, str2impl(string2), pos2, len);
}
const char* String_str(String* string) { return StrImpl_terminate(str2impl(string)); }
bool String_push(String* string, char val) { return StrImpl_push(str2impl(string), val); }
char String_pop(String* string) {
  int val = 0;
  StrImpl_pop(str2impl(string), &val);
  return val;
}
char* String_at(String* string, uintptr_t pos) {
  return pos < str2impl(string)->size ? StrImpl_at(str2impl(string), pos) : NULL;
}
bool String_appendn(String* string, uintptr_t n, char val) {
  return StrImpl_appendn(str2impl(string), n, val);
}
bool String_append(String* string, const char* s, intptr_t n) {
  return StrImpl_append(str2impl(string), s, n < 0 ? strlen(s) : n);
}
char* String_back(String* string) { return StrImpl_back(str2impl(string)); }
uintptr_t String_size(String* string) { return str2impl(string)->size; }
uintptr_t String_capacity(String* string) { return StrImpl_cap(str2impl(string)); }
bool String_empty(String* string) { return str2impl(string)->size == 0; }
