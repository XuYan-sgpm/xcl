//
// Created by 徐琰 on 2023/11/23.
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <xcl/concurrent/mutex.h>
#include <xcl/io/path.h>
#include <xcl/util/algo.h>
#include <xcl/util/log.h>

size_t roundUp(size_t size, size_t align) {
  return (align & (align - 1)) ? ((size + align - 1) / align) * align
                               : (size + align - 1) & ~(align - 1);
}

size_t roundDown(size_t size, size_t align) {
  return (align & (align - 1)) ? (size / align) * align : size & ~(align - 1);
}

uint64_t hash(const void* data, size_t len) {
  const static uint64_t hash_basis = 14695981039346656037ULL;
  const static uint64_t hash_prime = 1099511628211ULL;
  uint64_t val = hash_basis;
  for (int i = 0; i < len; i++) {
    val ^= ((const uint8_t*)data)[i];
    val *= hash_prime;
  }
  return val;
}

// 定义雪花算法参数
#define EPOCH 1728628057000  // 自定义的时间戳起始点，例如2018-01-01T00:00:00Z
#define MACHINE_ID_BITS 10   // 机器ID占用的位数
#define SEQUENCE_BITS 12     // 序列号占用的位数

#define MACHINE_ID_SHIFT (SEQUENCE_BITS)
#define TIMESTAMP_LEFT_SHIFT (MACHINE_ID_BITS + SEQUENCE_BITS)

#define MACHINE_ID_MASK ((1L << MACHINE_ID_BITS) - 1)
#define MAX_SEQUENCE_MASK ((1L << SEQUENCE_BITS) - 1)

typedef struct {
  uint64_t last_ts;
  uint64_t machine_id;
  uint64_t sequence;
} Snowflake;

static Snowflake snowflake = {0};

// 等待下一个时间戳
static uint64_t Snowflake_wait(uint64_t last_ts) {
  uint64_t ts = time(NULL);
  while (ts <= last_ts)
    ts = time(NULL);
  return ts;
}

// 生成下一个ID
uint64_t Snowflake_next() {
  while (snowflake.machine_id == 0)
    snowflake.machine_id = rand() & MACHINE_ID_MASK;
  uint64_t ts = time(NULL);
  if (ts < snowflake.last_ts) {
    // 时间戳回退，处理时钟回拨情况
    LOG_P(LOG_ERROR,
          "Clock moved backwards. Refusing to generate id for %d "
          "milliseconds\n",
          snowflake.last_ts - ts);
    return 0;
  }

  if (ts == snowflake.last_ts) {
    // 相同时间戳下，序列号+1
    snowflake.sequence = (snowflake.sequence + 1) & MAX_SEQUENCE_MASK;
    if (snowflake.sequence == 0) {
      // 序列号溢出，等待下一个时间戳
      ts = Snowflake_wait(snowflake.last_ts);
      snowflake.last_ts = ts;
      snowflake.sequence = 1;
    }
  } else {
    // 不同时间戳下，序列号重置为1
    snowflake.sequence = 1;
  }

  snowflake.last_ts = ts;

  // 构建ID
  return ((ts - EPOCH) << TIMESTAMP_LEFT_SHIFT) | (snowflake.machine_id << MACHINE_ID_SHIFT) |
         snowflake.sequence;
}

static uint64_t SimpleHasher_hash(struct Hasher* hasher, const void* item) {
  return hash(item, ((struct SimpleHasher*)hasher)->item_size);
}
struct SimpleHasher SimpleHasher_make(unsigned item_size) {
  static void* simple_hasher_vptr[] = {SimpleHasher_hash};
  struct SimpleHasher hasher = {.item_size = item_size, .super.__vptr = (void*)simple_hasher_vptr};
  return hasher;
}

#ifdef Windows
#define ARCHIVE_EXT "lib"
#define SHARED_EXT "dll"
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#ifdef __APPLE__
#define SHARED_EXT "dylib"
#else
#define SHARED_EXT "so"
#endif
#define ARCHIVE_EXT "a"
#endif
#include <ctype.h>

static bool DlModule_verifyDlExt(const char* ext) {
  if (strcmp(ext, SHARED_EXT) == 0 || strcmp(ext, ARCHIVE_EXT) == 0)
    return true;
#ifdef Windows
  if (strchr(ext, '.'))
    return false;
#else
  const char* ver = strchr(ext, '.');
  if (!ver)
    return false;
  char real_ext[8] = {0};
  memcpy(real_ext, ext, ver - ext);
  if (strcmp(real_ext, ARCHIVE_EXT) && strcmp(real_ext, SHARED_EXT))
    return false;
  const char* p = ver + 1;
  for (;;) {
    const char* q = strchr(p, '.');
    while (isdigit(p[0]) && p != q)
      ++p;
    if (!p[0])
      break;
    if (p != q)
      return false;
    p = q + 1;
  }
  LOG_P(LOG_DEBUG, "library version %s", ver);
#endif
  return true;
}
static const char* DlModule_join(const char* hint, const char* name, char* path,
                                 unsigned path_len) {
  char name_with_ext[MAX_PATH_NAME];
  const char* ext = strchr(name, '.');
  if (ext && !DlModule_verifyDlExt(ext + 1)) {
    LOG_P(LOG_ERROR, "invalid library extension %s", ext);
    return NULL;
  }
  if (!ext)
    snprintf(name_with_ext, MAX_PATH_NAME, "%s.%s", name, SHARED_EXT);
  else
    snprintf(name_with_ext, MAX_PATH_NAME, "%s", name);
  if (snprintf(path, path_len, "%s%c%s", hint, PATH_SEP, name_with_ext) >= path_len)
    return NULL;
  if (Path_exists(path))
    return path;
  if (strncmp(name, "lib", 3) &&
      snprintf(path, path_len, "%s%clib%s", hint, PATH_SEP, name_with_ext) < path_len &&
      Path_exists(path))
    return path;
  return NULL;
}

static const char* findName(const char* name, const char** hints, unsigned hint_size, char* path,
                            unsigned path_len) {
  while (hint_size-- > 0) {
    if (DlModule_join(hints[0], name, path, path_len)) {
      LOG_P(LOG_DEBUG, "find library %s -> %s", name, path);
      return path;
    }
    ++hints;
  }
  return NULL;
}

const char* findLibrary(const char** names, unsigned name_size, const char** hints,
                        unsigned hint_size, char* path, unsigned path_len) {
  while (name_size-- > 0) {
    if (findName(names[0], hints, hint_size, path, path_len))
      return path;
    ++names;
  }
  return NULL;
}

uintptr_t gcd(uintptr_t x, uintptr_t y) { return x % y == 0 ? y : gcd(y, x % y); }

uintptr_t lcm(uintptr_t x, uintptr_t y) {
  if (x < y) {
    uintptr_t tmp = x;
    x = y;
    y = tmp;
  }
  return x * (y / gcd(x, y));
}
