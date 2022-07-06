#pragma once

#include <stdint.h>

typedef struct _Config Config;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * create config from specified ini path
 */
Config* Config_new(const char* storePath);

/*
 * create config from usr specified stream
 */
Config* Config_newFromStream(void* stream,
                             int32_t (*reader)(void*, char*, int32_t));

/*
 * read value from key under section
 */
int32_t Config_read(Config* config,
                    const char* section,
                    const char* key,
                    char* value,
                    int* valueLen);

/*
 * write key-value paramter under section
 * write to memory and no disk flush
 */
int32_t Config_write(Config* config,
                     const char* section,
                     const char* key,
                     const char* value);

/*
 * flush config data to file specified by storePath
 * write data not includes comment, so it may be different
 * from config content before
 */
int32_t Config_flushToPath(Config* config, const char* storePath);

/*
 * flush config data to usr specified stream
 * write data only contains available data without
 * comment string and useless blank lines
 */
int32_t Config_flush(Config* config,
                     void* stream,
                     int32_t (*writer)(void*, const char*, int32_t));

/*
 * free config resources
 */
void Config_delete(Config* config);

/*
 * print config data in console, only for Config_debug test
 */
void Config_debug(Config* config);

#ifdef __cplusplus
}
#endif
