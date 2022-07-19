#pragma once

#include <stdbool.h>
#include <xcl/lang/XclDef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char* data;
    uint32_t state;
    int size;
} CBuffer;

XCL_PUBLIC(CBuffer)
Buffer_make(int cap);

XCL_PUBLIC(CBuffer)
Buffer_newRegion(char* src, int len);

XCL_PUBLIC(int)
Buffer_cap(const CBuffer* buffer);

XCL_PUBLIC(CBuffer)
wrapBuf(const char* src, int len);

XCL_PUBLIC(CBuffer)
wrapBuf2(const CBuffer* buffer, int pos, int len);

XCL_PUBLIC(bool)
Buffer_release(CBuffer* buffer);

XCL_PUBLIC(bool)
Buffer_push(CBuffer* buffer, char ch);

XCL_PUBLIC(int)
Buffer_appendRegion(CBuffer* buffer, const char* src, int len);

XCL_PUBLIC(int)
Buffer_append(CBuffer* buffer, const char* src);

XCL_PUBLIC(int)
Buffer_appendChars(CBuffer* buffer, int n, char ch);

XCL_PUBLIC(bool)
Buffer_pop(CBuffer* buffer, char* dst);

XCL_PUBLIC(bool)
Buffer_get(const CBuffer* buffer, int pos, char* dst);

XCL_PUBLIC(bool)
Buffer_writeChar(CBuffer* buffer, int pos, char ch);

XCL_PUBLIC(bool)
Buffer_writeChars(CBuffer* buffer, int pos, int n, char ch);

XCL_PUBLIC(int)
Buffer_writeRegion(CBuffer* buffer, int pos, const char* src, int len);

XCL_PUBLIC(int)
Buffer_write(CBuffer* buffer, int pos, const char* src);

XCL_PUBLIC(int)
Buffer_readRegion(const CBuffer* buffer, int pos, char* dst, int len);

XCL_PUBLIC(int)
Buffer_read(const CBuffer* buffer, int pos, char* dst);

XCL_PUBLIC(bool)
Buffer_expand(CBuffer* buffer, int cap);

XCL_PUBLIC(void)
Buffer_removeRegion(CBuffer* buffer, int pos, int len);

XCL_PUBLIC(void)
Buffer_removePos(CBuffer* buffer, int pos);

XCL_PUBLIC(void)
Buffer_clear(CBuffer* buffer);

XCL_PUBLIC(bool)
Buffer_remake(CBuffer* buffer, int cap);

XCL_PUBLIC(char*)
Buffer_at(const CBuffer* buffer, int pos);

#ifdef __cplusplus
}
#endif
