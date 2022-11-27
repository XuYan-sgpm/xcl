#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <xcl/lang/xcl_def.h>

typedef struct Config Config;

/*
 * create config from specified ini path
 */
XCL_EXPORT Config* XCL_API
Config_new(const char* storePath);

/*
 * create config from obj specified stream
 */
XCL_EXPORT Config* XCL_API
Config_newFromStream(void* stream, int32_t (*reader)(void*, char*, int32_t));

/*
 * read value from key under section
 */
XCL_EXPORT int32_t XCL_API
Config_read(Config* config,
            const char* section,
            const char* key,
            char* value,
            int* valueLen);

/*
 * write key-value parameter under section
 * write to memory and no disk flush
 */
XCL_EXPORT int32_t XCL_API
Config_write(Config* config,
             const char* section,
             const char* key,
             const char* value);

/*
 * flush config data to file specified by storePath
 * write data not includes comment, so it may be different
 * from config content before
 */
XCL_EXPORT int32_t XCL_API
Config_flushToPath(Config* config, const char* storePath);

/*
 * flush config data to obj specified stream
 * write data only contains available data without
 * comment string and useless blank lines
 */
XCL_EXPORT int32_t XCL_API
Config_flush(Config* config,
             void* stream,
             int32_t (*writer)(void*, const char*, int32_t));

/*
 * free config resources
 */
XCL_EXPORT void XCL_API
Config_delete(Config* config);

/*
 * print config data in console, only for Config_debug test
 */
XCL_EXPORT void XCL_API
Config_debug(Config* config);

#ifdef __cplusplus
}
#endif
