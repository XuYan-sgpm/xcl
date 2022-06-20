#include <assert.h>
#include <stdlib.h>
#include <string.h>
// #include <util/CBuffer.h>
#include <util/CString.h>

const static unsigned __STATE_MASK__ = 0x7fffffff;

static inline bool __strUseStack(const CString *string) {
  return string->state >> 31;
}

static inline int __strGetSize(const CString *string) {
  return string->state & __STATE_MASK__;
}

static inline void
__strSetState(CString *string, const int useStack, const int size) {
  string->state = (useStack << 31) | (size & __STATE_MASK__);
}

static inline void __strSetSize(CString *string, const int size) {
  __strSetState(string, __strUseStack(string), size);
}

static inline char *__strPtr(CString *string, const int pos) {
  return __strUseStack(string) ? string->mem.stack.data + pos
                               : string->mem.heap.ptr + pos;
}

static inline const char *__strCPtr(const CString *string, const int pos) {
  return __strUseStack(string) ? string->mem.stack.data + pos
                               : string->mem.heap.ptr + pos;
}

static inline int __strCap(const CString *string) {
  return __strUseStack(string) ? sizeof(string->mem.stack)
                               : string->mem.heap.cap;
}

static inline bool
__strAddCheck(const CString *string, const int pos, const int len) {
  return pos >= 0 && pos <= __strGetSize(string) && len >= 0 &&
         len <= __strCap(string) - __strGetSize(string);
}

static inline bool
__strRemoveCheck(const CString *string, const int pos, const int len) {
  return pos >= 0 && pos < __strGetSize(string) && len > 0 &&
         len <= __strGetSize(string) - pos;
}

static inline int __strGrow(const int cap, const int add) {
  return cap + (cap > add ? cap : add);
}

static void __strInit(CString *string, const int cap) {
  memset(&string->mem, 0, sizeof(string->mem));
  if (cap <= sizeof(string->mem.stack.data)) {
    __strSetState(string, true, 0);
  } else {
    char *p = (char *)malloc(cap);
    if (p) {
      string->mem.heap.ptr = p;
      string->mem.heap.cap = cap;
    }
    __strSetState(string, false, 0);
  }
}

static char *__strBeforeInsert(CString *string, const int pos, const int len) {
  char *insertCursor = NULL;
  char *p = __strPtr(string, pos);
  const int size = strSize(string);
  const int cap = __strCap(string);
  if (__strAddCheck(string, pos, len)) {
    memmove(p + len, p, size - pos);
    __strSetSize(string, size + len);
    insertCursor = p;
  } else {
    int newCap = __strGrow(cap, size + len - cap);
    char *newPtr = (char *)malloc(newCap);
    if (!newPtr) {
      return NULL;
    }
    memcpy(newPtr, __strPtr(string, 0), pos);
    memcpy(newPtr + pos + len, p, size - pos);
    if (!__strUseStack(string)) {
      free(string->mem.heap.ptr);
    }
    string->mem.heap.ptr = newPtr;
    string->mem.heap.cap = newCap;
    __strSetState(string, false, size + len);
    insertCursor = newPtr + pos;
  }
  return insertCursor;
}

static char *__strBeforeAssign(CString *string, const int pos, const int len) {
  char *assignCursor = NULL;
  const int cap = __strCap(string);
  char *p = __strPtr(string, pos);
  if (pos + len <= cap) {
    __strSetSize(string, pos + len);
    assignCursor = p;
  } else {
    int newCap = __strGrow(cap, pos + len - cap);
    char *newPtr = (char *)malloc(newCap);
    if (!newPtr) {
      return NULL;
    }
    memcpy(newPtr, __strPtr(string, 0), pos);
    if (!__strUseStack(string)) {
      free(string->mem.heap.ptr);
    }
    string->mem.heap.ptr = newPtr;
    string->mem.heap.cap = newCap;
    __strSetState(string, false, pos + len);
    assignCursor = newPtr + pos;
  }
  return assignCursor;
}

static bool
__strInsert(CString *string, const int pos, const char *src, const int len) {
  char *insertCursor = __strBeforeInsert(string, pos, len);
  if (insertCursor) {
    memcpy(insertCursor, src, len);
  }
  return insertCursor;
}

static bool
__strAssign(CString *string, const int pos, const char *src, const int len) {
  char *assignCursor = __strBeforeAssign(string, pos, len);
  if (assignCursor) {
    memcpy(assignCursor, src, len);
  }
  return assignCursor;
}

CString makeString(const char *str) { return makeString2(str, strlen(str)); }

CString makeString2(const char *str, const int len) {
  CString string;
  __strInit(&string, len);
  memcpy(__strPtr(&string, 0), str, len);
  __strSetSize(&string, len);
  return string;
}

CString strAlloc(const int cap) {
  assert(cap > 0);
  CString string;
  __strInit(&string, cap);
  return string;
}

void strRelease(CString *string) {
  if (!__strUseStack(string)) {
    free(string->mem.heap.ptr);
  }
  __strSetState(string, true, 0);
}

void strClear(CString *string) { __strSetSize(string, 0); }

int strSize(const CString *string) { return __strGetSize(string); }

int strCap(const CString *string) { return __strCap(string); }

bool strEmpty(const CString *string) { return __strGetSize(string) == 0; }

bool strAssign(CString *string, const char *str) {
  return __strAssign(string, 0, str, strlen(str));
}

bool strAssignRegion(CString *string, const char *str, const int len) {
  return __strAssign(string, 0, str, len);
}

bool strAssignStr(CString *string, const CString *src) {
  return __strAssign(string, 0, __strCPtr(src, 0), strSize(src));
}

bool strPushChar(CString *string, char ch) {
  char *cursor = __strBeforeInsert(string, strSize(string), 1);
  if (cursor) {
    *cursor = ch;
  }
  return cursor;
}

bool strPushChars(CString *string, const int n, char ch) {
  char *cursor = __strBeforeInsert(string, strSize(string), n);
  if (cursor) {
    memset(cursor, ch, n);
  }
  return cursor;
}

bool strPushRegion(CString *string, const char *str, const int len) {
  char *cursor = __strBeforeInsert(string, strSize(string), len);
  if (cursor) {
    memcpy(cursor, str, len);
  }
  return cursor;
}

bool strPush(CString *string, const char *str) {
  return strPushRegion(string, str, strlen(str));
}

bool strPushStr(CString *string, const CString *src) {
  return strPushRegion(string, __strCPtr(src, 0), strSize(src));
}

bool strWriteChar(CString *string, const int pos, char ch) {
  char *cursor = __strBeforeInsert(string, pos, 1);
  if (cursor) {
    *cursor = ch;
  }
  return cursor;
}

bool strWriteChars(CString *string, const int pos, const int n, char ch) {
  char *cursor = __strBeforeInsert(string, pos, n);
  if (cursor) {
    memset(cursor, ch, n);
  }
  return cursor;
}

bool strWriteRegion(CString *string,
                    const int pos,
                    const char *str,
                    const int len) {
  char *cursor = __strBeforeInsert(string, pos, len);
  if (cursor) {
    memcpy(cursor, str, len);
  }
  return cursor;
}

bool strWrite(CString *string, const int pos, const char *str) {
  const int len = strlen(str);
  char *cursor = __strBeforeInsert(string, pos, len);
  if (cursor) {
    memcpy(cursor, str, len);
  }
  return cursor;
}

bool strWriteStr(CString *string, const int pos, const CString *src) {
  return strWriteRegion(string, pos, __strCPtr(src, 0), strSize(src));
}

bool strPop(CString *string, char *dst) {
  if (strEmpty(string)) {
    return false;
  }
  char *p = __strPtr(string, 0);
  *dst = *p;
  const int size = strSize(string);
  memcpy(p, p + 1, size - 1);
  __strSetSize(string, size - 1);
  return true;
}

bool strPopBack(CString *string, char *dst) {
  if (strEmpty(string)) {
    return false;
  }
  const int size = strSize(string);
  char *p = __strPtr(string, size - 1);
  *dst = *p;
  __strSetSize(string, size - 1);
  return true;
}

bool strGet(const CString *string, const int pos, char *dst) {
  if (pos >= 0 && pos < strSize(string)) {
    *dst = *__strCPtr(string, pos);
    return true;
  }
  return false;
}

int strGets(const CString *string, const int pos, const int len, char *dst) {
  if (!__strRemoveCheck(string, pos, len)) {
    return -1;
  }
  memcpy(dst, __strCPtr(string, pos), len);
  return len;
}

void strDelete(CString *string, const int pos) {
  const int size = strSize(string);
  if (pos < 0 || pos >= size) {
    return;
  }
  char *p = __strPtr(string, pos);
  memcpy(p, p + 1, size - pos - 1);
  __strSetSize(string, size - 1);
}

void strDeleteRegion(CString *string, const int pos, const int len) {
  if (__strRemoveCheck(string, pos, len)) {
    char *p = __strPtr(string, pos);
    const int size = strSize(string);
    memcpy(p, p + len, size - pos - len);
    __strSetSize(string, size - len);
  }
}

const char *strQueryChar(const CString *string, const bool left, char ch) {
  const char *p = __strCPtr(string, 0);
  const int size = strSize(string);
  if (left) {
    for (int i = 0; i < size; i++) {
      if (*p == ch) {
        return p;
      }
    }
  } else {
    for (int i = size - 1; i >= 0; i--) {
      if (*p == ch) {
        return p;
      }
    }
  }
  return NULL;
}

// static const char *
// __strCmp(const char *src, const char *pattern) {
//   const char *cursor1 = src;
//   const char *cursor2 = pattern;
//   while (*cursor1 && *cursor2) {
//     if (*cursor1 != *cursor2) {
//       break;
//     }
//     ++cursor1;
//     ++cursor2;
//   }
//   return cursor1;
// }

static const char *
__strCmpRegion(const char *src, const char *pattern, const int len) {
  int i;
  for (i = 0; i < len; i++) {
    if (src[i] != pattern[i]) {
      break;
    }
  }
  return src + i;
}

static const char *
__strDirectSearch(const CString *string, const char *pattern, const int len) {
  for (int i = 0; i < strSize(string) - len; i++) {
    const char *start = __strCPtr(string, i);
    const char *cursor = __strCmpRegion(start, pattern, len);
    if (cursor - start == len) {
      return start;
    }
  }
  return NULL;
}

const static const int STR_QUERY_THRESHOLD = 32;

static void __strGenNext(const char *pattern, int *next, const int len) {
  next[0] = -1;
  int idx = 0;
  int val = -1;
  while (idx < len - 1) {
    if (val == -1 || pattern[idx] == pattern[val]) {
      if (pattern[++idx] == pattern[++val]) {
        next[idx] = next[val];
      } else {
        next[idx] = val;
      }
    } else {
      val = next[val];
    }
  }
}

static const char *
__strKmpSearch(const CString *string, const char *pattern, const int len) {
  int next[len];
  __strGenNext(pattern, next, len);
  int i, j;
  const char *start = __strCPtr(string, 0);
  for (i = 0, j = 0; i < strSize(string) && j < len; i++, j++) {
    if (j == -1 || start[i] == pattern[j]) {
      ++i;
      ++j;
    } else {
      j = next[j];
    }
  }
  if (j == len) {
    return &start[i - len];
  }
  return NULL;
}

const char *strQuery(const CString *string, const char *str) {
  return strQueryRegion(string, str, str ? strlen(str) : 0);
}

const char *
strQueryRegion(const CString *string, const char *str, const int len) {
  if (strSize(string) < len) {
    return NULL;
  }
  return len <= STR_QUERY_THRESHOLD ? __strDirectSearch(string, str, len)
                                    : __strKmpSearch(string, str, len);
}

char strAt(const CString *string, const int pos) {
  return *__strCPtr(string, pos);
}

CString strDup(const CString *string) {
  return strDupRegion(string, 0, strSize(string));
}

CString strDupRegion(const CString *string, const int pos, const int len) {
  CString copy;
  __strInit(&copy, len);
  memcpy(__strPtr(&copy, 0), __strCPtr(string, pos), len);
  __strSetSize(&copy, len);
  return copy;
}
