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

/**
 * @brief wrap buffer with specified str
 * @param src source data pointer
 * @param len source data length
 * @return buffer object
 * @author xuyan
 * @date 2022-08-02
 */
XCL_EXPORT CBuffer XCL_API
wrapBuf(const char* src, int len);

/**
 * @brief wrap buffer with specified buffer object
 * @param buffer source buffer object
 * @param pos wrapped source buffer index
 * @param len wrapped source length
 * @return buffer object
 * @author xuyan
 * @date 2022-08-02
 */
XCL_EXPORT CBuffer XCL_API
wrapBuf2(const CBuffer* buffer, int pos, int len);

/**
 * @brief release buffer memory
 * @param buffer buffer object
 * @return true if release successfully, otherwise false
 * @author xuyan
 * @date 2022-08-02
 */
XCL_EXPORT bool XCL_API
Buffer_release(CBuffer* buffer);

/**
 * @brief push character to a buffer object
 * @param buffer buffer object
 * @param ch character
 * @return true if push successfully, otherwise false
 * @author xuyan
 * @date 2022-08-02
 */
XCL_EXPORT bool XCL_API
Buffer_push(CBuffer* buffer, char ch);

/**
 * @brief append source region to buffer
 * @param buffer buffer object
 * @param src source pointer
 * @param len source length
 * @return character count appended, if append false, return 0
 * @author xuyan
 * @date 2022-08-02
 */
XCL_EXPORT int XCL_API
Buffer_appendRegion(CBuffer* buffer, const char* src, int len);

/**
 * @brief append str to buffer
 * @param buffer buffer object
 * @param src source pointer
 * @return character count appended, if append false, return 0
 * @author xuyan
 * @date 2022-08-02
 */
XCL_EXPORT int XCL_API
Buffer_append(CBuffer* buffer, const char* src);

/**
 * @brief append characters to buffer
 * @param buffer buffer object
 * @param n character count
 * @param ch character value
 * @return character count pushed, otherwise 0
 * @author xuyan
 * @date 2022-08-03
 */
XCL_EXPORT int XCL_API
Buffer_appendChars(CBuffer* buffer, int n, char ch);

/**
 * @brief pop character at first position of buffer
 * @param buffer buffer object
 * @param dst output pointer store character
 * @return true if pop successfully, otherwise false
 * @author xuyan
 * @date 2022-08-03
 */
XCL_EXPORT bool XCL_API
Buffer_pop(CBuffer* buffer, char* dst);

/**
 * @brief get character at position of buffer
 * @param buffer buffer object
 * @param pos get position
 * @param dst output pointer store character
 * @return true if get successfully, otherwise false
 * @author xuyan
 * @date 2022-08-03
 */
XCL_EXPORT bool XCL_API
Buffer_get(const CBuffer* buffer, int pos, char* dst);

/**
 * @brief write character at position
 * @param buffer buffer object
 * @param pos write position, should be [0, size)
 * @param ch written character
 * @return true if write successfully, otherwise false
 * @author xuyan
 * @date 2022-08-03
 */
XCL_EXPORT bool XCL_API
Buffer_writeChar(CBuffer* buffer, int pos, char ch);

/**
 * @brief write characters to the buffer
 * @param buffer buffer object
 * @param pos write position
 * @param n write count of characters
 * @param ch character
 * @return true if write successfully, otherwise false
 * @author xuyan
 * @date 2022-08-03
 */
XCL_EXPORT bool XCL_API
Buffer_writeChars(CBuffer* buffer, int pos, int n, char ch);

/**
 * @brief write characters of [src,src+len) to the buffer
 * @param buffer buffer object
 * @param pos write position, should be [0,size)
 * @param src string pointer
 * @param len string length
 * @return true if write string successfully, otherwise false
 * @author xuyan
 * @date 2022-08-03
 */
XCL_EXPORT int XCL_API
Buffer_writeRegion(CBuffer* buffer, int pos, const char* src, int len);

/**
 * @brief write string to the buffer, string should be terminated by '\0'
 * @param buffer buffer object
 * @param pos write position
 * @param src string pointer
 * @return true if write string successfully, otherwise false
 * @author xuyan
 * @date 2022-08-03
 */
XCL_EXPORT int XCL_API
Buffer_write(CBuffer* buffer, int pos, const char* src);

/**
 * @brief read data at [pos,pos+len) from buffer, store data to dst
 * @param buffer buffer object
 * @param pos read position of the buffer
 * @param dst output pointer store buffer data
 * @param len read length
 * @return data size of read, otherwise 0
 * @author xuyan
 * @date 2022-08-03
 */
XCL_EXPORT int XCL_API
Buffer_readRegion(const CBuffer* buffer, int pos, char* dst, int len);

/**
 * @brief read all data at [pos,size) from buffer, and store data to dst
 * @param buffer buffer object
 * @param pos read position
 * @param dst output pointer hold read data
 * @return count of characters read
 * @author xuyan
 * @date 2022-08-03
 */
XCL_EXPORT int XCL_API
Buffer_read(const CBuffer* buffer, int pos, char* dst);

/**
 * @brief expand buffer capacity to new cap, hold original buffer data.
 * if buffer is wrapped from external memory, create a new buffer
 * @param buffer buffer object
 * @param cap new buffer capacity
 * @return true if expand successfully, otherwise false
 * @author xuyan
 * @date 2022-08-03
 */
XCL_EXPORT bool XCL_API
Buffer_expand(CBuffer* buffer, int cap);

/**
 * @brief remove characters store at region [pos,pos+len) of buffer
 * @param buffer buffer object
 * @param pos remove position
 * @param len remove length
 * @author xuyan
 * @date 2022-08-03
 */
XCL_EXPORT void XCL_API
Buffer_removeRegion(CBuffer* buffer, int pos, int len);

/**
 * @brief remove data store at pos of buffer
 * @param buffer buffer object
 * @param pos remove position
 * @author xuyan
 * @date 2022-08-03
 */
XCL_EXPORT void XCL_API
Buffer_removePos(CBuffer* buffer, int pos);

/**
 * @brief clear buffer object
 * @param buffer buffer object
 * @author xuyan
 * @date 2022-08-03
 */
XCL_EXPORT void XCL_API
Buffer_clear(CBuffer* buffer);

/**
 * @brief realloc buffer to cap. if buffer is wrapped from
 * external memory, create a new buffer object, otherwise
 * if realloc memory failed, return false, not request a new
 * memory for buffer.
 * @param buffer buffer object
 * @param cap new buffer capacity
 * @return true if realloc memory successfully, otherwise false
 * @author xuyan
 * @date 2022-08-03
 */
XCL_EXPORT bool XCL_API
Buffer_remake(CBuffer* buffer, int cap);

/**
 * @brief get pointer at pos of buffer
 * @param buffer buffer object
 * @param pos buffer position
 * @return pointer point to position pos of buffer
 * @author xuyan
 * @date 2022-08-03
 */
XCL_EXPORT char* XCL_API
Buffer_at(const CBuffer* buffer, int pos);

#ifdef __cplusplus
}
#endif
