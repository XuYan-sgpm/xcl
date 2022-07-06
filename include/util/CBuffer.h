#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char* data;
  int state;
  int size;
} CBuffer;

CBuffer Buffer_new(int cap);

CBuffer Buffer_newRegion(char* src, int len);

int Buffer_cap(const CBuffer* buffer);

CBuffer wrapBuf(char* src, int len);

CBuffer wrapBuf2(const CBuffer* buffer, int pos, int len);

bool Buffer_free(CBuffer* buffer);

bool Buffer_push(CBuffer* buffer, char ch);

int Buffer_appendRegion(CBuffer* buffer, const char* src, int len);

int Buffer_append(CBuffer* buffer, const char* src);

int Buffer_appendChars(CBuffer* buffer, int n, char ch);

bool Buffer_pop(CBuffer* buffer, char* dst);

bool Buffer_get(const CBuffer* buffer, int pos, char* dst);

bool Buffer_writeChar(CBuffer* buffer, int pos, char ch);

bool Buffer_writeChars(CBuffer* buffer, int pos, int n, char ch);

int Buffer_writeRegion(CBuffer* buffer, int pos, const char* src, int len);

int Buffer_write(CBuffer* buffer, int pos, const char* src);

int Buffer_readRegion(const CBuffer* buffer, int pos, char* dst, int len);

int Buffer_read(const CBuffer* buffer, int pos, char* dst);

bool Buffer_expand(CBuffer* buffer, int cap);

void Buffer_removeRegion(CBuffer* buffer, int pos, int len);

void Buffer_removePos(CBuffer* buffer, int pos);

void Buffer_clear(CBuffer* buffer);

bool Buffer_realloc(CBuffer* buffer, int cap);

char* Buffer_at(const CBuffer* buffer, int pos);

#ifdef __cplusplus
}
#endif
