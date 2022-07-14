#pragma once

#include <stdint.h>
#include <xcl/lang/XclDef.h>

typedef struct _Config Config;

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * create config from specified ini path
 */
XCL_PUBLIC(Config*)
Config_new(const char*storePath);

/*
 * create config from usr specified stream
 */
XCL_PUBLIC(Config*)
Config_newFromStream(void*stream,
                     int32_t (*reader)(void*, char*, int32_t));

/*
 * read value from key under section
 */
XCL_PUBLIC(int32_t)
Config_read(Config*config,
            const char*section,
            const char*key,
            char*value,
            int*valueLen);

/*
 * write key-value parameter under section
 * write to memory and no disk flush
 */
XCL_PUBLIC(int32_t)
Config_write(Config*config,
             const char*section,
             const char*key,
             const char*value);

/*
 * flush config data to file specified by storePath
 * write data not includes comment, so it may be different
 * from config content before
 */
XCL_PUBLIC(int32_t)
Config_flushToPath(Config*config, const char*storePath);

/*
 * flush config data to usr specified stream
 * write data only contains available data without
 * comment string and useless blank lines
 */
XCL_PUBLIC(int32_t)
Config_flush(Config*config,
             void*stream,
             int32_t (*writer)(void*, const char*, int32_t));

/*
 * free config resources
 */
XCL_PUBLIC(void)
Config_delete(Config*config);

/*
 * print config data in console, only for Config_debug test
 */
XCL_PUBLIC(void)
Config_debug(Config*config);

#ifdef __cplusplus
}
#endif
