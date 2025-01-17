//
// Created by 14049 on 2024/6/3.
//

#include <internal/util/str_impl.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <xcl/lang/alloc.h>
#include <xcl/lang/define.h>
#include <xcl/util/log.h>

static inline void _set(void* dest, uintptr_t n, int val, bool wide) {
  wide ? wmemset(dest, val, n) : memset(dest, val, n);
}
static inline void _cpy(void* dest, const void* src, uintptr_t n, bool wide) {
  memcpy(dest, src, wide ? n * sizeof(wchar_t) : n);
}
static inline void _move(void* dest, const void* src, uintptr_t n, bool wide) {
  memmove(dest, src, wide ? n * sizeof(wchar_t) : n);
}
static inline intptr_t _diff(const void* p, const void* q, bool wide) {
  return wide ? (const wchar_t*)p - (const wchar_t*)q : (const char*)p - (const char*)q;
}
static intptr_t _chr(const void* buf, int val, bool wide) {
  void* p = wide ? (void*)wcschr(buf, val) : (void*)strchr(buf, val);
  return !p ? -1 : _diff(p, buf, wide);
}
static intptr_t _rchr(const void* buf, int val, bool wide) {
  void* p = wide ? (void*)wcsrchr(buf, val) : (void*)strrchr(buf, val);
  return !p ? -1 : _diff(p, buf, wide);
}
static intptr_t _str(const void* buf, const void* s, bool wide) {
  void* p = wide ? (void*)wcsstr(buf, s) : (void*)strstr(buf, s);
  return !p ? -1 : _diff(p, buf, wide);
}
static inline int _cmp(const void* s1, const void* s2, bool wide) {
  return !wide ? strcmp(s1, s2) : wcscmp(s1, s2);
}
static inline int _ncmp(const void* s1, const void* s2, uintptr_t n, bool wide) {
  return !wide ? strncmp(s1, s2, n) : wcsncmp(s1, s2, n);
}
static inline uintptr_t _grow(uintptr_t cap, uintptr_t n) { return (cap <<= 1) >= n ? cap : n; }

static inline uintptr_t StrImpl_itemSize(StrImpl* str_impl) {
  return str_impl->wide ? sizeof(wchar_t) : sizeof(char);
}
uintptr_t StrImpl_cap(StrImpl* str_impl) {
  return str_impl->expanded ? str_impl->cap
                            : sizeof(str_impl->tiny) / StrImpl_itemSize(str_impl) - 1;
}
static inline void* StrImpl_data(StrImpl* str_impl) {
  return str_impl->expanded ? str_impl->buf : str_impl->tiny;
}
void* StrImpl_at(StrImpl* str_impl, uintptr_t pos) {
  return str_impl->wide ? (void*)((wchar_t*)StrImpl_data(str_impl) + pos)
                        : (void*)((char*)StrImpl_data(str_impl) + pos);
}
void StrImpl_destroy(StrImpl* str_impl) {
  if (str_impl->expanded)
    pfree(str_impl->buf);
  str_impl->expanded = 0;
  str_impl->size = 0;
}
StrImpl StrImpl_make(bool wide) {
  StrImpl o = {0};
  o.wide = wide;
  return o;
}
static void* StrImpl_preIns(StrImpl* str_impl, uintptr_t pos, uintptr_t n) {
  if (pos > str_impl->size)
    return NULL;
  uintptr_t expected = str_impl->size + n;
  uintptr_t cap = StrImpl_cap(str_impl);
  if (cap >= expected) {
    _move(StrImpl_at(str_impl, pos + n), StrImpl_at(str_impl, pos), str_impl->size - pos,
          str_impl->wide);
    str_impl->size += n;
    return StrImpl_at(str_impl, pos);
  }
  StrImpl o = StrImpl_make(str_impl->wide);
  o.expanded = 1;
  o.cap = _grow(cap, expected);
  o.buf = palloc((o.cap + 1) * StrImpl_itemSize(&o));
  if (!o.buf)
    return NULL;
  _cpy(o.buf, StrImpl_at(str_impl, 0), pos, o.wide);
  _cpy(StrImpl_at(&o, pos + n), StrImpl_at(str_impl, pos), str_impl->size - pos, o.wide);
  o.size = str_impl->size + n;
  if (str_impl->expanded)
    pfree(str_impl->buf);
  *str_impl = o;
  return StrImpl_at(str_impl, pos);
}
static bool StrImpl_preFill(StrImpl* str_impl, uintptr_t n) {
  uintptr_t cap = StrImpl_cap(str_impl);
  if (cap >= n) {
    str_impl->size = 0;
    return true;
  }
  StrImpl o = StrImpl_make(str_impl->wide);
  o.expanded = 1;
  o.cap = _grow(cap, n);
  o.buf = palloc((o.cap + 1) * StrImpl_itemSize(&o));
  if (!o.buf)
    return false;
  if (str_impl->expanded)
    pfree(str_impl->buf);
  *str_impl = o;
  return true;
}
bool StrImpl_insert(StrImpl* str_impl, uintptr_t pos, const void* s, uintptr_t n) {
  void* p = StrImpl_preIns(str_impl, pos, n);
  if (p)
    _cpy(p, s, n, str_impl->wide);
  return p;
}
bool StrImpl_insertn(StrImpl* str_impl, uintptr_t pos, uintptr_t n, int val) {
  void* p = StrImpl_preIns(str_impl, pos, n);
  if (p)
    _set(p, n, val, str_impl->wide);
  return p;
}
bool StrImpl_reserve(StrImpl* str_impl, uintptr_t n) {
  uintptr_t cap = StrImpl_cap(str_impl);
  if (cap >= n)
    return true;
  StrImpl o = {0};
  o.wide = str_impl->wide;
  o.expanded = 1;
  o.cap = _grow(cap, n);
  o.buf = palloc((o.cap + 1) * StrImpl_itemSize(&o));
  if (!o.buf)
    return false;
  _cpy(o.buf, StrImpl_at(str_impl, 0), str_impl->size, o.wide);
  o.size = str_impl->size;
  if (str_impl->expanded)
    pfree(str_impl->buf);
  *str_impl = o;
  return true;
}
bool StrImpl_push(StrImpl* str_impl, int val) {
  if (!StrImpl_reserve(str_impl, str_impl->size + 1))
    return false;
  str_impl->wide ? (void)(*(wchar_t*)StrImpl_at(str_impl, str_impl->size++) = val)
                 : (void)(*(char*)StrImpl_at(str_impl, str_impl->size++) = val);
  return true;
}
bool StrImpl_appendn(StrImpl* str_impl, uintptr_t n, int val) {
  if (!StrImpl_reserve(str_impl, n + str_impl->size))
    return false;
  _set(StrImpl_at(str_impl, str_impl->size), n, val, str_impl->wide);
  str_impl->size += n;
  return true;
}
bool StrImpl_append(StrImpl* str_impl, const void* s, uintptr_t n) {
  if (!StrImpl_reserve(str_impl, n + str_impl->size))
    return false;
  _cpy(StrImpl_at(str_impl, str_impl->size), s, n, str_impl->wide);
  str_impl->size += n;
  return true;
}
void* StrImpl_back(StrImpl* str_impl) {
  return str_impl->size > 0 ? StrImpl_at(str_impl, str_impl->size - 1) : NULL;
}
bool StrImpl_pop(StrImpl* str_impl, int* val) {
  if (str_impl->size) {
    void* p = StrImpl_at(str_impl, --str_impl->size);
    *val = !str_impl->wide ? *(char*)p : *(wchar_t*)p;
    return true;
  }
  return false;
}
bool StrImpl_filln(StrImpl* str_impl, uintptr_t n, int val) {
  if (!StrImpl_preFill(str_impl, n))
    return false;
  _set(StrImpl_data(str_impl), n, val, str_impl->wide);
  str_impl->size = n;
  return true;
}
bool StrImpl_fill(StrImpl* str_impl, const void* s, uintptr_t n) {
  if (!StrImpl_preFill(str_impl, n))
    return false;
  _cpy(StrImpl_data(str_impl), s, n, str_impl->wide);
  str_impl->size = n;
  return true;
}
bool StrImpl_erase(StrImpl* str_impl, uintptr_t pos, uintptr_t n) {
  if (pos + n <= str_impl->size) {
    _move(StrImpl_at(str_impl, pos), StrImpl_at(str_impl, pos + n), (str_impl->size - pos - n),
          str_impl->wide);
    str_impl->size -= n;
    return true;
  }
  return false;
}
void* StrImpl_terminate(StrImpl* str_impl) {
  void* p = StrImpl_at(str_impl, str_impl->size);
  str_impl->wide ? (void)(*(wchar_t*)p = 0) : (void)(*(char*)p = 0);
  return StrImpl_data(str_impl);
}
intptr_t StrImpl_findc(StrImpl* str_impl, bool reverse, uintptr_t pos, int val) {
  if (pos >= str_impl->size)
    return -1;
  return (!reverse ? _chr : _rchr)(StrImpl_at(str_impl, pos), val, str_impl->wide);
}
intptr_t StrImpl_finds(StrImpl* str_impl, uintptr_t pos, const void* s) {
  if (pos >= str_impl->size)
    return -1;
  return _str(StrImpl_at(str_impl, pos), s, str_impl->wide);
}
int StrImpl_cmp(StrImpl* impl1, StrImpl* impl2) {
  LOG_ASSERT_P(impl1->wide == impl2->wide);
  return _cmp(StrImpl_data(impl1), StrImpl_data(impl2), impl1->wide);
}
int StrImpl_ncmp(StrImpl* impl1, uintptr_t pos1, StrImpl* impl2, uintptr_t pos2, uintptr_t n) {
  if (pos1 + n > impl1->size || pos2 + n > impl2->size)
    return -1;
  return _ncmp(StrImpl_at(impl1, pos1), StrImpl_at(impl2, pos2), n, impl1->wide);
}
