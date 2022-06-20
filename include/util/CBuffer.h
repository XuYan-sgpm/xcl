#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char *data;
  int state;
  int size;
} CBuffer;

CBuffer makeBuf(int cap);

CBuffer makeBuf2(char *src, int len);

int bufCap(const CBuffer *buffer);

CBuffer wrapBuf(char *src, int len);

CBuffer wrapBuf2(const CBuffer *buffer, int pos, int len);

bool bufFree(CBuffer *buffer);

bool bufPush(CBuffer *buffer, char ch);

int bufAppendRegion(CBuffer *buffer, const char *src, int len);

int bufAppend(CBuffer *buffer, const char *src);

int bufAppendChars(CBuffer *buffer, int n, char ch);

bool bufPop(CBuffer *buffer, char *dst);

bool bufGet(const CBuffer *buffer, int pos, char *dst);

bool bufWriteChar(CBuffer *buffer, int pos, char ch);

bool bufWriteChars(CBuffer *buffer, int pos, int n, char ch);

int bufWriteRegion(CBuffer *buffer, int pos, const char *src, int len);

int bufWrite(CBuffer *buffer, int pos, const char *src);

int bufRead2(const CBuffer *buffer, int pos, char *dst, int len);

int bufRead(const CBuffer *buffer, int pos, char *dst);

bool bufExpand(CBuffer *buffer, int cap);

void bufRemoveRegion(CBuffer *buffer, int pos, int len);

void bufRemovePos(CBuffer *buffer, int pos);

void bufClear(CBuffer *buffer);

bool bufRealloc(CBuffer *buffer, int cap);

char *bufAt(const CBuffer *buffer, int pos);

#ifdef __cplusplus
}
#endif
