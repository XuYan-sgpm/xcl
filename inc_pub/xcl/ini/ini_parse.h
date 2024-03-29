/**
 * @author xuyan
 * @brief simple ini parse implementation
 * @version 1.0
 * @date 2022-08-05
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

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
 * callback when ini parser find result
 * first param is obj data
 * second param is section region
 * third param is key region
 * last param is value region
 * when find section, key and value is empty
 * if section has found, next invoke will pass empty
 */
typedef void (*IniCallback)(void*, Region, Region, Region);

/*
 * obj specified reader to read ini data
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
    const bool allow_error;
    /*
     * allow inline comment like aaa#bbb
     */
    const bool allow_inline_comment;
    /*
     * buf memory size
     */
    const int length;
    /*
     * buf to store ini characters
     */
    char* const buf;
} Ini;

/*
 * parse ini data
 * obj can be a specified object to invoke when callback is called
 */
bool
Ini_parseStream(Ini ini,
                void* stream,
                IniReadFunc reader,
                IniCallback callback,
                void* obj);

/*
 * parse ini data from file
 */
bool
Ini_parse(Ini ini, const char* file_path, IniCallback callback, void* obj);

/*
 * parse ini data from str
 */
bool
Ini_parseStr(Ini ini, const char* str, IniCallback callback, void* obj);

#ifdef __cplusplus
}
#endif
