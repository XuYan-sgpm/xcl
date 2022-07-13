#pragma once

#include <stdbool.h>
#include <xcl/lang/XclDef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char* data;
  int state;
  int size;
} CBuffer;

XCL_EXPORT(CBuffer)
Buffer_new(int cap);

XCL_EXPORT(CBuffer)
Buffer_newRegion(char* src, int len);

XCL_EXPORT(int)
Buffer_cap(const CBuffer* buffer);

XCL_EXPORT(CBuffer)
wrapBuf(char* src, int len);

XCL_EXPORT(CBuffer)
wrapBuf2(const CBuffer* buffer, int pos, int len);

XCL_EXPORT(bool)
Buffer_free(CBuffer* buffer);

XCL_EXPORT(bool)
Buffer_push(CBuffer* buffer, char ch);

XCL_EXPORT(int)
Buffer_appendRegion(CBuffer* buffer, const char* src, int len);

XCL_EXPORT(int)
Buffer_append(CBuffer* buffer, const char* src);

XCL_EXPORT(int)
Buffer_appendChars(CBuffer* buffer, int n, char ch);

XCL_EXPORT(bool)
Buffer_pop(CBuffer* buffer, char* dst);

XCL_EXPORT(bool)
Buffer_get(const CBuffer* buffer, int pos, char* dst);

XCL_EXPORT(bool)
Buffer_writeChar(CBuffer* buffer, int pos, char ch);

XCL_EXPORT(bool)
Buffer_writeChars(CBuffer* buffer, int pos, int n, char ch);

XCL_EXPORT(int)
Buffer_writeRegion(CBuffer* buffer, int pos, const char* src, int len);

XCL_EXPORT(int)
Buffer_write(CBuffer* buffer, int pos, const char* src);

XCL_EXPORT(int)
Buffer_readRegion(const CBuffer* buffer, int pos, char* dst, int len);

XCL_EXPORT(int)
Buffer_read(const CBuffer* buffer, int pos, char* dst);

XCL_EXPORT(bool)
Buffer_expand(CBuffer* buffer, int cap);

XCL_EXPORT(void)
Buffer_removeRegion(CBuffer* buffer, int pos, int len);

XCL_EXPORT(void)
Buffer_removePos(CBuffer* buffer, int pos);

XCL_EXPORT(void)
Buffer_clear(CBuffer* buffer);

XCL_EXPORT(bool)
Buffer_realloc(CBuffer* buffer, int cap);

XCL_EXPORT(char*)
Buffer_at(const CBuffer* buffer, int pos);

#ifdef __cplusplus
}
#endif
