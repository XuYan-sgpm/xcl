//
// Created by 徐琰 on 2022/6/26.
//

#include <pool/SizeClass.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
  int32_t log2Group;
  int32_t log2Delta;
  int32_t nDelta;
  bool isMultiPage;
  bool isSubPage;
  //   int32_t log2DeltaLookup;
  int32_t size;
} SizeTab;

const static int32_t LOG2_QUANTUM = 4;
const static int32_t PAGE_SIZE = 1 << 13;
const static int32_t CHUNK_SIZE = 1 << 24;
const static int32_t PAGE_SHIFTS = 13;
const static int32_t LOG2_GROUP_SIZE = 2;
const static int32_t SUB_PAGE_THRESHOLD = 28 << 10;

static uint32_t __log2(uint32_t i) {
  if (i == 0) {
    return 32;
  }
  uint32_t n = 1;
  if ((i >> 16) == 0) {
    n += 16;
    i <<= 16;
  }
  if ((i >> 24) == 0) {
    n += 8;
    i <<= 8;
  }
  if ((i >> 28) == 0) {
    n += 4;
    i <<= 4;
  }
  if ((i >> 30) == 0) {
    n += 2;
    i <<= 2;
  }
  n -= (i >> 31u);
  return 31u - n;
}

static SizeTab *sizeClasses = NULL;
static int32_t tabs = 0;

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

///**
// * initialize idx to size tabs
// * @param sizeIdx2size
// * @param pageIdx2size
// */
// static void __idx2sizeTab(int32_t *sizeIdx2size, int32_t *pageIdx2size);
//
///**
// * initialize size to idx tab
// * @param size2idx
// */
// static void __size2idxTab(int32_t *size2idx);

bool SizeClass_initialize() {
  if (sizeClasses) {
    return true;
  }
  bool success = false;
  do {
    int32_t groups = __log2(CHUNK_SIZE) + 1 - LOG2_QUANTUM;
    sizeClasses =
        (SizeTab *)malloc(sizeof(SizeTab) * (groups << LOG2_GROUP_SIZE));
    if (!sizeClasses) {
      break;
    }
    tabs = __initSizeClassTabs();
    //    sizeIdx2size = (int32_t *)malloc(tabs << 2);
    //    if (!sizeIdx2size) {
    //      break;
    //    }
    //    pageIdx2size = (int32_t *)malloc(tabs << 2);
    //    if (!pageIdx2size) {
    //      break;
    //    }
    success = true;
  } while (false);
  if (!success) {
    SizeClass_finalize();
  }
  return success;
}

void SizeClass_finalize() {
  free(sizeClasses);
  sizeClasses = NULL;
  //  free(sizeIdx2size);
  //  sizeIdx2size = NULL;
  //  free(pageIdx2size);
  //  pageIdx2size = NULL;
  //  free(size2idx);
  //  size2idx = NULL;
  tabs = 0;
}

int32_t SizeClass_size() { return tabs; }

bool SizeClass_get(int32_t idx, int32_t *out) {
  if (idx >= tabs) {
    return false;
  }
  SizeTab tab = sizeClasses[idx];
  out[0] = tab.log2Group;
  out[1] = tab.log2Delta;
  out[2] = tab.nDelta;
  out[3] = tab.isMultiPage;
  out[4] = tab.isSubPage;
  out[5] = tab.size;
  return true;
}

static void
__SC_getGroupAndDelta(uint32_t size, uint32_t *log2Group, uint32_t *log2Delta) {
  uint32_t log2Size = __log2(size);
  if (size <= (1 << (LOG2_QUANTUM + LOG2_GROUP_SIZE))) {
    *log2Group = LOG2_QUANTUM;
    *log2Delta = *log2Group;
  } else {
    *log2Group = log2Size;
    *log2Delta = *log2Group - LOG2_GROUP_SIZE;
  }
}

uint32_t SizeClass_normalize(uint32_t size) {
  if (size <= 16) {
    return 16;
  }
  if (size >= CHUNK_SIZE) {
    return size;
  }
  uint32_t log2Group, log2Delta;
  __SC_getGroupAndDelta(size, &log2Group, &log2Delta);
  uint32_t nDelta = (size - (1 << log2Group)) >> log2Delta;
  if (size - ((1 << log2Group) + (nDelta << log2Delta)) == 0) {
    return size;
  }
  if (log2Group == LOG2_QUANTUM) {
    if (nDelta < 3) {
      ++nDelta;
    } else {
      nDelta = 1;
      log2Group += LOG2_GROUP_SIZE;
    }
  } else {
    if (nDelta < 4) {
      ++nDelta;
    } else {
      nDelta = 1;
      ++log2Group;
      ++log2Delta;
    }
  }
  return (1u << log2Group) + (nDelta << log2Delta);
}

uint32_t SizeClass_size2pages(uint32_t size) {
  if (size <= PAGE_SIZE) {
    return 1;
  }
  uint32_t pages = size >> PAGE_SHIFTS;
  if ((size & (PAGE_SIZE - 1)) == 0) {
    return pages;
  }
  return pages + 1;
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
    while (nDelta <= nDeltaLimit && size < CHUNK_SIZE) {
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
  int32_t size = (1 << log2Group) + (nDelta << log2Delta);
  if (log2Delta >= PAGE_SHIFTS) {
    isMultiPage = true;
  } else {
    isMultiPage = size % PAGE_SIZE == 0;
  }
  bool isSubPage = size <= SUB_PAGE_THRESHOLD;
  SizeTab tab = {log2Group, log2Delta, nDelta, isMultiPage, isSubPage, size};
  sizeClasses[idx] = tab;
  return size;
}
