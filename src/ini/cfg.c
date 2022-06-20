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

typedef struct _Paramter {
  char *key;
  char *value;
} Paramter;

typedef struct _SectionData {
  char *name;
  CList *paramters;
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
  const Paramter *paramter = (const Paramter *)arg1;
  return __cmpStrWithRegion(paramter->key, arg2);
}

/*
 * get section data if section exists
 */
static SectionData *__ConfigGetSection(Config *config, Region secRegion) {
  CListIter it =
      listQuery(config->sections, &secRegion, __cmpSectionWithRegion);
  return listIterEquals(it, listEnd(config->sections))
             ? NULL
             : (SectionData *)it.cur->data;
}

/*
 * if section exists, return section data
 * otherwise, create section and put into config->sections
 */
static SectionData *__ConfigCheckoutSection(Config *config, Region secRegion) {
  CListIter secIt =
      listQuery(config->sections, &secRegion, __cmpSectionWithRegion);
  if (listIterEquals(secIt, listEnd(config->sections))) {
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
      CList *params = makeList();
      if (params) {
        SectionData *secData = (SectionData *)secNode->data;
        secData->name = name;
        secData->paramters = params;
        listPush(config->sections, secNode);
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
 * get paramter from section paramter list
 */
static Paramter *__ConfigGetParamter(CList *params, Region keyRegion) {
  CListIter paramIt = listQuery(params, &keyRegion, __cmpParamWithRegion);
  if (!listIterEquals(paramIt, listEnd(params))) {
    return (Paramter *)paramIt.cur->data;
  }
  return NULL;
}

/*
 * get paramter from section paramters if exists
 * otherwise, create paramter and put into paramter list
 */
static Paramter *
__ConfigCheckoutParamter(CList *params, Region keyRegion, Region valueRegion) {
  CListIter paramIt = listQuery(params, &keyRegion, __cmpParamWithRegion);
  if (!listIterEquals(paramIt, listEnd(params))) {
    /*
     * key exists, check value
     * if value not equals, update value
     */
    CListNode *paramNode = paramIt.cur;
    Paramter *param = (Paramter *)paramNode->data;
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
        (CListNode *)malloc(sizeof(CListNode) + sizeof(Paramter));
    if (!paramNode) {
      return NULL;
    }
    Paramter *param = (Paramter *)paramNode->data;
    param->key = __newStr(keyRegion.str, keyRegion.len);
    if (param->key || !keyRegion.len) {
      param->value = __newStr(valueRegion.str, valueRegion.len);
      if (param->value || !valueRegion.len) {
        listPush(params, paramNode);
        return param;
      }
      free(param->key);
    }
    free(paramNode);
    return NULL;
  }
}

/*
 * use struct to store section name in read callback (__CONFIG_iniReadCb)
 * after section found, section region will be empty
 * in callback if paramter found. we need to figure out
 * which section the current key-value paramter belongs to
 */
typedef struct _IniReadCbArgs {
  Config *const config;
  char *sec;  // current section name
  int secLen; // current section length
} IniReadCbArgs;

/*
 * invoked when section or key-value paramter found
 * in ini parse
 */
static void __CONFIG_iniReadCb(void *usr,
                               Region secRegion,
                               Region keyRegion,
                               Region valueRegion) {
  IniReadCbArgs *args = (IniReadCbArgs *)usr;
  Config *config = args->config;
  if (secRegion.str) {
    /*
     * section found, record section name in read cb args
     */
    SectionData *secData = __ConfigCheckoutSection(config, secRegion);
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
     * all key-value paramters ignored util
     * next section found
     */
    if (!args->secLen) {
      return;
    }
    Region secRegion = {args->sec, args->secLen};
    SectionData *secData = __ConfigGetSection(config, secRegion);
    if (!secData) {
      /*
       * should not happen
       */
      return;
    }
    __ConfigCheckoutParamter(secData->paramters, keyRegion, valueRegion);
  }
}

static int32_t __CONFIG_ReadFile(void *stream, char *buf, int32_t len) {
  return fread(buf, 1, len, (FILE *)stream);
}

static int32_t __CONFIG_WriteFile(void *stream, const char *buf, int32_t len) {
  return fwrite(buf, 1, len, (FILE *)stream);
}

Config *makeConfig(const char *storePath) {
  FILE *fp = fopen(storePath, "r");
  if (!fp) {
    return NULL;
  }
  Config *config = makeConfig2(fp, __CONFIG_ReadFile);
  fclose(fp);
  return config;
}

Config *makeConfig2(void *stream, int32_t (*reader)(void *, char *, int32_t)) {
  char buf[1024];
  Ini ini = {true, true, sizeof(buf), buf};
  Config *config = (Config *)malloc(sizeof(Config));
  if (!config) {
    return NULL;
  }
  config->sections = makeList();
  if (!config->sections) {
    free(config);
    return NULL;
  }
  char sec[128];
  IniReadCbArgs args = {config, sec, 0};
  iniParseStream(ini, stream, reader, __CONFIG_iniReadCb, &args);
  return config;
}

int32_t readConfig(Config *config,
                   const char *section,
                   const char *key,
                   char *value,
                   int *valueLen) {
  if (!config) {
    return -1;
  }
  Region secRegion = {section, strlen(section)};
  SectionData *secData = __ConfigGetSection(config, secRegion);
  if (secData) {
    Region keyRegion = {key, strlen(key)};
    Paramter *param = __ConfigGetParamter(secData->paramters, keyRegion);
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

int32_t writeConfig(Config *config,
                    const char *section,
                    const char *key,
                    const char *value) {
  Region secRegion = {section, strlen(section)};
  SectionData *secData = __ConfigCheckoutSection(config, secRegion);
  if (!secData) {
    return -1;
  }
  Region keyRegion = {key, strlen(key)};
  Region valueRegion = {value, strlen(value)};
  return __ConfigCheckoutParamter(secData->paramters, keyRegion, valueRegion)
             ? 0
             : -1;
}

int32_t flushToFile(Config *config, const char *storePath) {
  FILE *fp = fopen(storePath, "w");
  if (!fp) {
    return -1;
  }
  int ret = flushConfig(config, fp, __CONFIG_WriteFile);
  fclose(fp);
  return ret;
}

static int32_t __streamFlush(void *stream,
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

int32_t flushConfig(Config *config,
                    void *stream,
                    int32_t (*writer)(void *, const char *, int32_t)) {
  char line[1024];
  CListIter secIt = listBegin(config->sections);
  int total = 0;
  while (!listIterEquals(secIt, listEnd(config->sections))) {
    SectionData *secData = (SectionData *)secIt.cur->data;
    total = snprintf(line, sizeof(line), "[%s]\n", secData->name);
    if (__streamFlush(stream, writer, line, total) != total) {
      return -1;
    }
    CListIter paramIt = listBegin(secData->paramters);
    while (!listIterEquals(paramIt, listEnd(secData->paramters))) {
      Paramter *param = (Paramter *)paramIt.cur->data;
      total = snprintf(line, sizeof(line), "%s=%s\n", param->key, param->value);
      if (__streamFlush(stream, writer, line, total) != total) {
        return -1;
      }
      paramIt = listNext(paramIt);
    }
    secIt = listNext(secIt);
  }
  return 0;
}

void debug(Config *config) {
  CList *sections = config->sections;
  CListIter sectionIt = listBegin(sections);
  while (!listIterEquals(sectionIt, listEnd(sections))) {
    SectionData *sectionData = (SectionData *)sectionIt.cur->data;
    printf("[%s]\n", sectionData->name);
    CList *paramters = (CList *)(sectionData->paramters);
    CListIter configIt = listBegin(paramters);
    while (!listIterEquals(configIt, listEnd(paramters))) {
      Paramter *configData = (Paramter *)configIt.cur->data;
      printf("%s=%s\n", configData->key, configData->value);
      configIt = listNext(configIt);
    }
    sectionIt = listNext(sectionIt);
  }
}

void freeConfig(Config *config) {
  CListNode *secNode = NULL;
  while ((secNode = listPop(config->sections))) {
    SectionData *secData = (SectionData *)secNode->data;
    free(secData->name);
    CList *params = secData->paramters;
    CListNode *paramNode = NULL;
    while ((paramNode = listPop(params))) {
      Paramter *param = (Paramter *)paramNode->data;
      free(param->key);
      free(param->value);
      free(paramNode);
    }
    freeList(params);
    free(secNode);
  }
  freeList(config->sections);
  free(config);
}
