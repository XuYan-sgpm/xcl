//
// Created by 14049 on 2023/12/19.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <xcl/lang/define.h>

typedef enum { BIG = 0, LITTLE } ByteEndian;

/**
 * byte buffer structure
 * for read/write operation, make sure cursor < limit
 */
typedef struct {
  char* ptr;
  unsigned cap;
  unsigned cursor, limit;
  unsigned info;
} ByteBuffer;

/**
 * get cpu byte endian
 * @return local byte endian
 */
ByteEndian localEndian();

/**
 * alloc byte buffer
 * @param cap max size_ of byte buffer
 * @param endian byte endian of byte buffer
 * @return byte buffer
 */
ByteBuffer ByteBuffer_alloc(unsigned cap, ByteEndian endian);

/**
 * create byte buffer from external memory
 * @param buf external pointer
 * @param len available bytes of external buffer
 * @param endian byte endian, BIG OR LITTLE
 * @return byte buffer
 */
ByteBuffer ByteBuffer_wrap(void* buf, unsigned len, ByteEndian endian);

/**
 * slice byte buffer
 * @param bb source byte buffer
 * @param offset byte offset
 * @param len slice byte length
 * @return slicing of source byte buffer
 */
ByteBuffer ByteBuffer_slice(ByteBuffer* bb, unsigned offset, unsigned len);

/**
 * finalize byte buffer
 * @param bb byte buffer
 */
void ByteBuffer_destroy(ByteBuffer* bb);

/**
 * put bytes into byte buffer
 * @param bb byte buffer
 * @param buf source buffer pointer
 * @param len source buffer length
 * @return true if put successfully, otherwise false
 */
bool ByteBuffer_put(ByteBuffer* bb, const void* buf, unsigned len);

/**
 * put 8-bit value into byte buffer
 * @param bb byte buffer
 * @param val 8-bit value
 * @return true if put successfully, otherwise false
 */
bool ByteBuffer_put8(ByteBuffer* bb, unsigned char val);

/**
 * put 16-bit value into byte buffer
 * @param bb byte buffer
 * @param val 16-bit value
 * @return true if put successfully, otherwise false
 */
bool ByteBuffer_put16(ByteBuffer* bb, unsigned short val);

/**
 * put 32-bit value into byte buffer
 * @param bb byte buffer
 * @param val 32-bit value
 * @return true if put successfully, otherwise false
 */
bool ByteBuffer_put32(ByteBuffer* bb, unsigned val);

/**
 * put 64-bit value into byte buffer
 * @param bb byte buffer
 * @param val 64-bit value
 * @return true if put successfully, otherwise false
 */
bool ByteBuffer_put64(ByteBuffer* bb, uint64_t val);

/**
 * set 8-bit value into position of byte buffer
 * @param bb byte buffer
 * @param offset byte offset
 * @param val 8-bit value
 * @return true if set successfully, otherwise false
 */
bool ByteBuffer_set8(ByteBuffer* bb, unsigned offset, unsigned char val);

/**
 * set 16-bit value into position of byte buffer
 * @param bb byte buffer
 * @param offset byte offset
 * @param val 16-bit value
 * @return true if set successfully, otherwise false
 */
bool ByteBuffer_set16(ByteBuffer* bb, unsigned offset, unsigned short val);

/**
 * set 32-bit value into position of byte buffer
 * @param bb byte buffer
 * @param offset byte offset
 * @param val 32-bit value
 * @return true if set successfully, otherwise false
 */
bool ByteBuffer_set32(ByteBuffer* bb, unsigned offset, unsigned val);

/**
 * set 64-bit value into position of byte buffer
 * @param bb byte buffer
 * @param offset byte offset
 * @param val 64-bit value
 * @return true if set successfully, otherwise false
 */
bool ByteBuffer_set64(ByteBuffer* bb, unsigned offset, uint64_t val);

/**
 * set bytes into position of byte buffer
 * @param bb byte buffer
 * @param offset byte offset
 * @param buf source buffer pointer
 * @param len source byte length
 * @return actual bytes set into byte buffer, may be smaller than len
 */
unsigned ByteBuffer_set(ByteBuffer* bb, unsigned offset, const void* buf, unsigned len);

/**
 * pop 8-bit value from byte buffer
 * @param bb byte buffer
 * @return unsigned char value if successfully, otherwise abort
 */
unsigned char ByteBuffer_take8(ByteBuffer* bb);

/**
 * pop 16-bit value from byte buffer
 * @param bb byte buffer
 * @return 16-bit value if successfully, otherwise abort
 */
unsigned short ByteBuffer_take16(ByteBuffer* bb);

/**
 * pop 32-bit value from byte buffer
 * @param bb byte buffer
 * @return 32-bit value if pop successfully, otherwise abort
 */
unsigned ByteBuffer_take32(ByteBuffer* bb);

/**
 * pop 64-bit value from byte buffer
 * @param bb byte buffer
 * @return 64-bit value if pop successfully, otherwise abort
 */
uint64_t ByteBuffer_take64(ByteBuffer* bb);

/**
 * pop bytes from byte buffer
 * @param bb byte buffer
 * @param buf dest buffer hold bytes
 * @param len pop bytes
 * @return true if pop successfully, otherwise false
 */
bool ByteBuffer_take(ByteBuffer* bb, void* buf, unsigned len);

/**
 * read 8-bit value at position of byte buffer
 * @param bb byte buffer
 * @param offset byte offset
 * @return 8-bit value if index is available, otherwise abort
 */
unsigned char ByteBuffer_get8(ByteBuffer* bb, unsigned offset);

/**
 * read 16-bit value at position of byte buffer
 * @param bb byte buffer
 * @param offset byte offset
 * @return 16-bit value if index is available, otherwise abort
 */
unsigned short ByteBuffer_get16(ByteBuffer* bb, unsigned offset);

/**
 * read 32-bit value at position of byte buffer
 * @param bb byte buffer
 * @param offset byte offset
 * @return 32-bit value if index is available, otherwise abort
 */
unsigned ByteBuffer_get32(ByteBuffer* bb, unsigned offset);

/**
 * read 64-bit value at position of byte buffer
 * @param bb byte buffer
 * @param offset byte offset
 * @return 64-bit value if index is available, otherwise abort
 */
uint64_t ByteBuffer_get64(ByteBuffer* bb, unsigned offset);

/**
 * read bytes from byte buffer
 * @param bb byte buffer
 * @param offset byte offset
 * @param buf dest byte buffer
 * @param len byte length desired to read
 * @return available bytes
 */
unsigned ByteBuffer_get(ByteBuffer* bb, unsigned offset, void* buf, unsigned len);

/**
 * reset byte buffer cursor and limit
 * @param bb byte buffer
 */
void ByteBuffer_rewind(ByteBuffer* bb);

/**
 * set limit to cursor, and set cursor to 0;
 * set available read region to [0,cursor)
 * @param bb byte buffer
 */
void ByteBuffer_flip(ByteBuffer* bb);

#ifdef __cplusplus
}
#endif
