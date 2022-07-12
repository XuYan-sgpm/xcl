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

XCL_PUBLIC CBuffer XCL_API
Buffer_new(int cap);

XCL_PUBLIC CBuffer XCL_API
Buffer_newRegion(char* src, int len);

XCL_PUBLIC int XCL_API
Buffer_cap(const CBuffer* buffer);

XCL_PUBLIC CBuffer XCL_API
wrapBuf(char* src, int len);

XCL_PUBLIC CBuffer XCL_API
wrapBuf2(const CBuffer* buffer, int pos, int len);

XCL_PUBLIC bool XCL_API
Buffer_free(CBuffer* buffer);

XCL_PUBLIC bool XCL_API
Buffer_push(CBuffer* buffer, char ch);

XCL_PUBLIC int XCL_API
Buffer_appendRegion(CBuffer* buffer, const char* src, int len);

XCL_PUBLIC int XCL_API
Buffer_append(CBuffer* buffer, const char* src);

XCL_PUBLIC int XCL_API
Buffer_appendChars(CBuffer* buffer, int n, char ch);

XCL_PUBLIC bool XCL_API
Buffer_pop(CBuffer* buffer, char* dst);

XCL_PUBLIC bool XCL_API
Buffer_get(const CBuffer* buffer, int pos, char* dst);

XCL_PUBLIC bool XCL_API
Buffer_writeChar(CBuffer* buffer, int pos, char ch);

XCL_PUBLIC bool XCL_API
Buffer_writeChars(CBuffer* buffer, int pos, int n, char ch);

XCL_PUBLIC int XCL_API
Buffer_writeRegion(CBuffer* buffer, int pos, const char* src, int len);

XCL_PUBLIC int XCL_API
Buffer_write(CBuffer* buffer, int pos, const char* src);

XCL_PUBLIC int XCL_API
Buffer_readRegion(const CBuffer* buffer, int pos, char* dst, int len);

XCL_PUBLIC int XCL_API
Buffer_read(const CBuffer* buffer, int pos, char* dst);

XCL_PUBLIC bool XCL_API
Buffer_expand(CBuffer* buffer, int cap);

XCL_PUBLIC void XCL_API
Buffer_removeRegion(CBuffer* buffer, int pos, int len);

XCL_PUBLIC void XCL_API
Buffer_removePos(CBuffer* buffer, int pos);

XCL_PUBLIC void XCL_API
Buffer_clear(CBuffer* buffer);

XCL_PUBLIC bool XCL_API
Buffer_realloc(CBuffer* buffer, int cap);

XCL_PUBLIC char* XCL_API
Buffer_at(const CBuffer* buffer, int pos);

#ifdef __cplusplus
}
#endif
