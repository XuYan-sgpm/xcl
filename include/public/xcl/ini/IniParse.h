#pragma once

/*
 * author: xuyan
 * a strong ini parse implementation
 * simple and graceful
 * use it and have fun!
 */

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    const char* str;
    int len;
} Region;

/*
 * cb when ini parser find result
 * first param is usr data
 * second param is section region
 * third param is key region
 * last param is value region
 * when find section, key and value is empty
 * if section has found, next invoke will pass empty
 */
typedef void (*IniCallback)(void*, Region, Region, Region);

/*
 * usr specified reader to read ini data
 * first param is stream object to read ini data
 * second param is buf to store ini data
 * third param is buf length
 * last param is
 */
typedef int32_t (*IniReadFunc)(void*, char*, int);

typedef struct {
    /*
     * when Err_get occurred, continue parse or interrupt
     */
    const bool allowError;
    /*
     * allow inline comment like aaa#bbb
     */
    const bool allowInlineComment;
    /*
     * buf memory size
     */
    const int length;
    /*
     * buf to store ini characters
     */
    char* const buf;
} Ini;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * parse ini data
 * usr can be a specified object to invoke when cb is called
 */
bool Ini_parseStream(Ini ini, void* stream, IniReadFunc reader, IniCallback cb,
                     void* usr);

/*
 * parse ini data from file
 */
bool Ini_parse(Ini ini, const char* filePath, IniCallback cb, void* usr);

/*
 * parse ini data from str
 */
bool Ini_parseStr(Ini ini, const char* str, IniCallback cb, void* usr);

#ifdef __cplusplus
}
#endif
