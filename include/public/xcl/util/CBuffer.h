#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <xcl/lang/XclDef.h>
#include <stdint.h>

typedef struct {
    char* data;
    uint32_t state;
    int size;
} CBuffer;

/**
 * @brief make buffer object
 * @param cap buffer capacity
 * @return buffer object
 * @author xuyan
 * @date 2022-08-01
 */
XCL_EXPORT CBuffer XCL_API
Buffer_make(int cap);

/**
 * @brief create buffer from char region
 * @param src pointer store data
 * @param len src data length
 * @return buffer object
 * @author xuyan
 * @date 2022-08-01
 */
XCL_EXPORT CBuffer XCL_API
Buffer_newRegion(char* src, int len);

/**
 * @brief get buffer capacity
 * @param buffer buffer object
 * @return buffer capacity
 * @author xuyan
 * @date 2022-08-01
 */
XCL_EXPORT int XCL_API
Buffer_cap(const CBuffer* buffer);

XCL_EXPORT CBuffer XCL_API
wrapBuf(const char* src, int len);

XCL_EXPORT CBuffer XCL_API
wrapBuf2(const CBuffer* buffer, int pos, int len);

XCL_EXPORT bool XCL_API
Buffer_release(CBuffer* buffer);

XCL_EXPORT bool XCL_API
Buffer_push(CBuffer* buffer, char ch);

XCL_EXPORT int XCL_API
Buffer_appendRegion(CBuffer* buffer, const char* src, int len);

XCL_EXPORT int XCL_API
Buffer_append(CBuffer* buffer, const char* src);

XCL_EXPORT int XCL_API
Buffer_appendChars(CBuffer* buffer, int n, char ch);

XCL_EXPORT bool XCL_API
Buffer_pop(CBuffer* buffer, char* dst);

XCL_EXPORT bool XCL_API
Buffer_get(const CBuffer* buffer, int pos, char* dst);

XCL_EXPORT bool XCL_API
Buffer_writeChar(CBuffer* buffer, int pos, char ch);

XCL_EXPORT bool XCL_API
Buffer_writeChars(CBuffer* buffer, int pos, int n, char ch);

XCL_EXPORT int XCL_API
Buffer_writeRegion(CBuffer* buffer, int pos, const char* src, int len);

XCL_EXPORT int XCL_API
Buffer_write(CBuffer* buffer, int pos, const char* src);

XCL_EXPORT int XCL_API
Buffer_readRegion(const CBuffer* buffer, int pos, char* dst, int len);

XCL_EXPORT int XCL_API
Buffer_read(const CBuffer* buffer, int pos, char* dst);

XCL_EXPORT bool XCL_API
Buffer_expand(CBuffer* buffer, int cap);

XCL_EXPORT void XCL_API
Buffer_removeRegion(CBuffer* buffer, int pos, int len);

XCL_EXPORT void XCL_API
Buffer_removePos(CBuffer* buffer, int pos);

XCL_EXPORT void XCL_API
Buffer_clear(CBuffer* buffer);

XCL_EXPORT bool XCL_API
Buffer_remake(CBuffer* buffer, int cap);

XCL_EXPORT char* XCL_API
Buffer_at(const CBuffer* buffer, int pos);

XCL_EXPORT bool XCL_API
test(char c, bool b);

#ifdef __cplusplus
}
#endif
