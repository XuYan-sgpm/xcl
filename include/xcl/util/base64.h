#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BASE64_ENCODE_OUT_SIZE(s) ((unsigned int)((((s) + 2) / 3) * 4 + 1))
#define BASE64_DECODE_OUT_SIZE(s) ((unsigned int)(((s) / 4) * 3))

/*
 * out is null-terminated encode string.
 * return values is out length, exclusive terminating `\0'
 */
unsigned int base64_encode(const unsigned char* in, unsigned int inlen, char* out);

bool base64_encode2(const unsigned char* in, unsigned int inlen,
                    bool (*write)(void*, const void*, unsigned), void* ctx);

/*
 * return values is out length
 */
unsigned int base64_decode(const char* in, unsigned int inlen, unsigned char* out);

bool base64_decode2(const char* in, unsigned int inlen, bool (*write)(void*, const void*, unsigned),
                    void* ctx);

#ifdef __cplusplus
}
#endif
