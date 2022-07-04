#include "ini/cfg.h"

#include <stdbool.h>
#include <stdio.h>
// #include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ini/IniParse.h"
#include "util/CList.h"

static char *__newStr(const char *str, size_t size) {
  if (size == 0) {
    return NULL;
  }
  char *result = (char *)malloc(size + 1);
  if (!result) {
    return NULL;
  }
  strncpy(result, str, size);
  result[size] = 0;
  return result;
}

typedef struct {
  char *key;
  char *value;
} Parameter;

typedef struct {
  char *name;
  CList *parameters;
} SectionData;

struct _Config {
  CList *sections;
};

static int __cmpStrWithRegion(const void *str, const void *arg) {
  if (!str && !((const Region *)arg)->str) {
    return 0;
  }
  const Region *region = (const Region *)arg;
  if (!str || !region->str) {
    return -1;
  }
  if (strlen(str) != region->len) {
    return -1;
  }
  return strncmp(str, region->str, region->len);
}

static int __cmpSectionWithRegion(const void *arg1, const void *arg2) {
  const SectionData *sectionData = (const SectionData *)arg1;
  return __cmpStrWithRegion(sectionData->name, arg2);
}

static int __cmpParamWithRegion(const void *arg1, const void *arg2) {
  const Parameter *parameter = (const Parameter *)arg1;
  return __cmpStrWithRegion(parameter->key, arg2);
}

/*
 * get section data if section exists
 */
static SectionData *__Config_getSection(Config *config, Region secRegion) {
  CListIter it =
      List_query(config->sections, &secRegion, __cmpSectionWithRegion);
  return List_iterEquals(it, List_end(config->sections))
             ? NULL
             : (SectionData *)it.cur->data;
}

/*
 * if section exists, return section data
 * otherwise, create section and put into config->sections
 */
static SectionData *__Config_checkoutSection(Config *config, Region secRegion) {
  CListIter secIt =
      List_query(config->sections, &secRegion, __cmpSectionWithRegion);
  if (List_iterEquals(secIt, List_end(config->sections))) {
    /*
     * section not exists
     */
    CListNode *secNode =
        (CListNode *)malloc(sizeof(CListNode) + sizeof(SectionData));
    if (!secNode) {
      return NULL;
    }
    char *name = __newStr(secRegion.str, secRegion.len);
    if (name) {
      CList *params = List_new();
      if (params) {
        SectionData *secData = (SectionData *)secNode->data;
        secData->name = name;
        secData->parameters = params;
        List_push(config->sections, secNode);
        return secData;
      }
      free(name);
    }
    free(secNode);
    return NULL;
  } else {
    /*
     * section already exists
     */
    CListNode *secNode = secIt.cur;
    return ((SectionData *)secNode->data);
  }
}

/*
 * get parameter from section parameter list
 */
static Parameter *__Config_getParameter(CList *params, Region keyRegion) {
  CListIter paramIt = List_query(params, &keyRegion, __cmpParamWithRegion);
  if (!List_iterEquals(paramIt, List_end(params))) {
    return (Parameter *)paramIt.cur->data;
  }
  return NULL;
}

/*
 * get parameter from section parameters if exists
 * otherwise, create parameter and put into parameter list
 */
static Parameter *__Config_checkoutParameter(CList *params,
                                             Region keyRegion,
                                             Region valueRegion) {
  CListIter paramIt = List_query(params, &keyRegion, __cmpParamWithRegion);
  if (!List_iterEquals(paramIt, List_end(params))) {
    /*
     * key exists, check value
     * if value not equals, update value
     */
    CListNode *paramNode = paramIt.cur;
    Parameter *param = (Parameter *)paramNode->data;
    if (__cmpStrWithRegion(param->value, &valueRegion) != 0) {
      char *newValue = __newStr(valueRegion.str, valueRegion.len);
      if (newValue || !valueRegion.len) {
        free(param->value);
        param->value = newValue;
      }
    }
    return param;
  } else {
    CListNode *paramNode =
        (CListNode *)malloc(sizeof(CListNode) + sizeof(Parameter));
    if (!paramNode) {
      return NULL;
    }
    Parameter *param = (Parameter *)paramNode->data;
    param->key = __newStr(keyRegion.str, keyRegion.len);
    if (param->key || !keyRegion.len) {
      param->value = __newStr(valueRegion.str, valueRegion.len);
      if (param->value || !valueRegion.len) {
        List_push(params, paramNode);
        return param;
      }
      free(param->key);
    }
    free(paramNode);
    return NULL;
  }
}

/*
 * use struct to store section name in read callback (__Config_iniReadCb)
 * after section found, section region will be empty
 * in callback if parameter found. we need to figure out
 * which section the current key-value parameter belongs to
 */
typedef struct {
  Config *const config;
  char *sec;  // current section name
  int secLen; // current section length
} IniReadCbArgs;

/*
 * invoked when section or key-value parameter found
 * in ini parse
 */
static void __Config_iniReadCb(void *usr,
                               Region secRegion,
                               Region keyRegion,
                               Region valueRegion) {
  IniReadCbArgs *args = (IniReadCbArgs *)usr;
  Config *config = args->config;
  if (secRegion.str) {
    /*
     * section found, record section name in read cb args
     */
    SectionData *secData = __Config_checkoutSection(config, secRegion);
    if (secData) {
      if (args->secLen) {
        if (args->secLen == secRegion.len &&
            strncmp(args->sec, secRegion.str, secRegion.len) == 0) {
          return;
        }
      }
      strncpy(args->sec, secRegion.str, secRegion.len);
      args->secLen = secRegion.len;
    } else {
      args->secLen = 0;
    }
  } else {
    /*
     * no section or parse section failed
     * all key-value parameters ignored util
     * next section found
     */
    if (!args->secLen) {
      return;
    }
    SectionData *secData =
        __Config_getSection(config, (Region){args->sec, args->secLen});
    if (!secData) {
      /*
       * should not happen
       */
      return;
    }
    __Config_checkoutParameter(secData->parameters, keyRegion, valueRegion);
  }
}

static int32_t __Config_readFile(void *stream, char *buf, int32_t len) {
  return fread(buf, 1, len, (FILE *)stream);
}

static int32_t __Config_writeFile(void *stream, const char *buf, int32_t len) {
  return fwrite(buf, 1, len, (FILE *)stream);
}

Config *Config_new(const char *storePath) {
  FILE *fp = fopen(storePath, "r");
  if (!fp) {
    return NULL;
  }
  Config *config = Config_newFromStream(fp, __Config_readFile);
  fclose(fp);
  return config;
}

Config *Config_newFromStream(void *stream,
                             int32_t (*reader)(void *, char *, int32_t)) {
  char buf[1024];
  Ini ini = {true, true, sizeof(buf), buf};
  Config *config = (Config *)malloc(sizeof(Config));
  if (!config) {
    return NULL;
  }
  config->sections = List_new();
  if (!config->sections) {
    free(config);
    return NULL;
  }
  char sec[128];
  IniReadCbArgs args = {config, sec, 0};
  iniParseStream(ini, stream, reader, __Config_iniReadCb, &args);
  return config;
}

int32_t Config_read(Config *config,
                    const char *section,
                    const char *key,
                    char *value,
                    int *valueLen) {
  if (!config) {
    return -1;
  }
  Region secRegion = {section, strlen(section)};
  SectionData *secData = __Config_getSection(config, secRegion);
  if (secData) {
    Region keyRegion = {key, strlen(key)};
    Parameter *param = __Config_getParameter(secData->parameters, keyRegion);
    if (param) {
      int vl = strlen(param->value);
      if (!value) {
        *valueLen = vl;
      } else {
        *valueLen = *valueLen > vl ? vl : *valueLen;
        strncpy(value, param->value, *valueLen);
      }
      return 0;
    }
  }
  return -1;
}

int32_t Config_write(Config *config,
                     const char *section,
                     const char *key,
                     const char *value) {
  Region secRegion = {section, strlen(section)};
  SectionData *secData = __Config_checkoutSection(config, secRegion);
  if (!secData) {
    return -1;
  }
  Region keyRegion = {key, strlen(key)};
  Region valueRegion = {value, strlen(value)};
  return __Config_checkoutParameter(secData->parameters, keyRegion, valueRegion)
             ? 0
             : -1;
}

int32_t Config_flushToPath(Config *config, const char *storePath) {
  FILE *fp = fopen(storePath, "w");
  if (!fp) {
    return -1;
  }
  int ret = Config_flush(config, fp, __Config_writeFile);
  fclose(fp);
  return ret;
}

static int32_t
__Config_streamFlush(void *stream,
                     int32_t (*writer)(void *, const char *, int32_t),
                     const char *buf,
                     int32_t len) {
  int write, total = 0;
  while (total < len) {
    write = writer(stream, buf + total, len - total);
    if (write < 0) {
      break;
    }
    total += write;
  }
  return total;
}

int32_t Config_flush(Config *config,
                     void *stream,
                     int32_t (*writer)(void *, const char *, int32_t)) {
  char line[1024];
  CListIter secIt = List_begin(config->sections);
  int total = 0;
  while (!List_iterEquals(secIt, List_end(config->sections))) {
    SectionData *secData = (SectionData *)secIt.cur->data;
    total = snprintf(line, sizeof(line), "[%s]\n", secData->name);
    if (__Config_streamFlush(stream, writer, line, total) != total) {
      return -1;
    }
    CListIter paramIt = List_begin(secData->parameters);
    while (!List_iterEquals(paramIt, List_end(secData->parameters))) {
      Parameter *param = (Parameter *)paramIt.cur->data;
      total = snprintf(line, sizeof(line), "%s=%s\n", param->key, param->value);
      if (__Config_streamFlush(stream, writer, line, total) != total) {
        return -1;
      }
      paramIt = List_next(paramIt);
    }
    secIt = List_next(secIt);
  }
  return 0;
}

void Config_debug(Config *config) {
  CList *sections = config->sections;
  CListIter sectionIt = List_begin(sections);
  while (!List_iterEquals(sectionIt, List_end(sections))) {
    SectionData *sectionData = (SectionData *)sectionIt.cur->data;
    printf("[%s]\n", sectionData->name);
    CList *parameters = (CList *)(sectionData->parameters);
    CListIter configIt = List_begin(parameters);
    while (!List_iterEquals(configIt, List_end(parameters))) {
      Parameter *configData = (Parameter *)configIt.cur->data;
      printf("%s=%s\n", configData->key, configData->value);
      configIt = List_next(configIt);
    }
    sectionIt = List_next(sectionIt);
  }
}

void Config_delete(Config *config) {
  CListNode *secNode = NULL;
  while ((secNode = List_pop(config->sections))) {
    SectionData *secData = (SectionData *)secNode->data;
    free(secData->name);
    CList *params = secData->parameters;
    CListNode *paramNode = NULL;
    while ((paramNode = List_pop(params))) {
      Parameter *param = (Parameter *)paramNode->data;
      free(param->key);
      free(param->value);
      free(paramNode);
    }
    List_delete(params);
    free(secNode);
  }
  List_delete(config->sections);
  free(config);
}
