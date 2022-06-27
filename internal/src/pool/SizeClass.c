//
// Created by 徐琰 on 2022/6/26.
//

#include <pool/SizeClass.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
  const int32_t log2Group;
  const int32_t log2Delta;
  const int32_t nDelta;
  const bool isMultiPage;
  const bool isSubPage;
  const int32_t log2DeltaLookup;
  const int32_t size;
} SizeTab;

const static int32_t LOG2_QUANTUM = 4;
const static int32_t PAGE_SIZE = 1 << 13;
const static int32_t CHUNK_SIZE = 1 << 24;
const static int32_t PAGE_SHIFTS = 13;
const static int32_t LOG2_GROUP_SIZE = 2;

static int32_t __log2(int32_t i) {
  if (i == 0) {
    return 32;
  }
  int n = 1;
  if (i >> 16 == 0) {
    n += 16;
    i <<= 16;
  }
  if (i >> 24 == 0) {
    n += 8;
    i <<= 8;
  }
  if (i >> 28 == 0) {
    n += 4;
    i <<= 4;
  }
  if (i >> 30 == 0) {
    n += 2;
    i <<= 2;
  }
  n -= i >> 31;
  return 31 - n;
}

static SizeTab *sizeClasses = NULL;
static int32_t tabs = 0;
static int32_t *sizeIdx2size = NULL;
static int32_t *pageIdx2size = NULL;
static int32_t *size2idx = NULL;

/**
 * initialize all size class tabs
 * @return available tabs count
 */
static int32_t __initSizeClassTabs();

/**
 * initialize single size class tab
 * @return current tab's memory size
 */
static int32_t __initSizeClassTab(int32_t idx,
                                  int32_t log2Group,
                                  int32_t log2Delta,
                                  int32_t nDelta);

/**
 * initialize idx to size tabs
 * @param sizeIdx2size
 * @param pageIdx2size
 */
static void __idx2sizeTab(int32_t *sizeIdx2size, int32_t *pageIdx2size);

/**
 * initialize size to idx tab
 * @param size2idx
 */
static void __size2idxTab(int32_t *size2idx);

bool SizeClass_initialize() {
  bool success = false;
  do {
    int32_t groups = __log2(CHUNK_SIZE) + 1 - LOG2_QUANTUM;
    sizeClasses = malloc(sizeof(SizeTab) * groups);
    if (!sizeClasses) {
      break;
    }
    tabs = __initSizeClassTabs();
    sizeIdx2size = malloc(tabs << 2);
    if (!sizeIdx2size) {
      break;
    }
    pageIdx2size = malloc(tabs << 2);
    if (!pageIdx2size) {
      break;
    }
  } while (false);
  if (!success) {
    SizeClass_finalize();
  }
  return success;
}

static int32_t __initSizeClassTabs() {
  int32_t idx = 0;
  int32_t size = 0;

  int32_t log2Group = LOG2_QUANTUM;
  int32_t log2Delta = LOG2_QUANTUM;

  int32_t nDelta = 0;
  int32_t nDeltaLimit = 1 << LOG2_GROUP_SIZE;
  while (nDelta < nDeltaLimit) {
    size = __initSizeClassTab(idx++, log2Group, log2Delta, nDelta++);
  }
  log2Group += LOG2_GROUP_SIZE;
  while (size < CHUNK_SIZE) {
    nDelta = 1;
    while (nDelta < nDeltaLimit && size < CHUNK_SIZE) {
      size = __initSizeClassTab(idx++, log2Group, log2Delta, nDelta++);
    }
    ++log2Group;
    ++log2Delta;
  }
  assert(size == CHUNK_SIZE);
  return idx;
}

static int32_t __initSizeClassTab(int32_t idx,
                                  int32_t log2Group,
                                  int32_t log2Delta,
                                  int32_t nDelta) {
  bool isMultiPage = false;
  if (log2Delta >= PAGE_SHIFTS) {
    isMultiPage = true;
  } else {
    int32_t size = (1 << log2Group) + (nDelta << log2Delta);
    isMultiPage = size % PAGE_SIZE == 0;
  }
}
