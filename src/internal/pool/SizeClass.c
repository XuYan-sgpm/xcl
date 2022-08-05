//
// Created by 徐琰 on 2022/6/26.
//

#include "xcl/pool/SizeClass.h"
#include "xcl/pool/PoolDef.h"
#include "xcl/pool/CPool.h"
#include "xcl/util/algo.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    int32_t log2Group;
    int32_t log2Delta;
    int32_t nDelta;
    bool isMultiPage;
    bool isSubPage;
    //   int32_t log2DeltaLookup;
    int32_t size;
} SizeTab;

static SizeTab* sizeClasses = NULL;

static int32_t tabs = 0;

/**
 * initialize all size class tabs
 * @return available tabs count
 */
static int32_t
__initSizeClassTabs();

/**
 * initialize single size class tab
 * @return current tab's memory size
 */
static int32_t
__initSizeClassTab(int32_t idx,
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

XCL_EXPORT bool XCL_API
SizeClass_initialize()
{
    if (sizeClasses)
    {
        return true;
    }
    bool success = false;
    do
    {
        uint32_t groups = _log2(CHUNK_SIZE) + 1 - LOG2_QUANTUM;
        sizeClasses = (SizeTab*)Pool_alloc(NULL,
                                           sizeof(SizeTab)
                                               * (groups << LOG2_GROUP_SIZE));
        if (!sizeClasses)
        {
            break;
        }
        tabs = __initSizeClassTabs();
        success = true;
    } while (false);
    if (!success)
    {
        SizeClass_finalize();
    }
    return success;
}

XCL_EXPORT void XCL_API
SizeClass_finalize()
{
    uint32_t groups = _log2(CHUNK_SIZE) + 1 - LOG2_QUANTUM;
    Pool_dealloc(NULL,
                 sizeClasses,
                 sizeof(SizeTab) * (groups << LOG2_GROUP_SIZE));
    sizeClasses = NULL;
    tabs = 0;
}

XCL_EXPORT int32_t XCL_API
SizeClass_size()
{
    return tabs;
}

XCL_EXPORT bool XCL_API
SizeClass_get(int32_t idx, int32_t* out)
{
    if (idx >= tabs)
    {
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
__SC_getGroupAndDelta(uint32_t size, uint32_t* log2Group, uint32_t* log2Delta)
{
    uint32_t log2Size = _log2(size);
    if (size <= (1 << (LOG2_QUANTUM + LOG2_GROUP_SIZE)))
    {
        *log2Group = LOG2_QUANTUM;
        *log2Delta = *log2Group;
    }
    else
    {
        *log2Group = log2Size;
        *log2Delta = *log2Group - LOG2_GROUP_SIZE;
    }
}

XCL_EXPORT uint32_t XCL_API
SizeClass_normalize(uint32_t size)
{
    if (size <= 16)
    {
        return 16;
    }
    if (size >= CHUNK_SIZE)
    {
        return size;
    }
    uint32_t log2Group, log2Delta;
    __SC_getGroupAndDelta(size, &log2Group, &log2Delta);
    uint32_t nDelta = (size - (1 << log2Group)) >> log2Delta;
    if (size - ((1 << log2Group) + (nDelta << log2Delta)) == 0)
    {
        return size;
    }
    if (log2Group == LOG2_QUANTUM)
    {
        if (nDelta < 3)
        {
            ++nDelta;
        }
        else
        {
            nDelta = 1;
            log2Group += LOG2_GROUP_SIZE;
        }
    }
    else
    {
        if (nDelta < 4)
        {
            ++nDelta;
        }
        else
        {
            nDelta = 1;
            ++log2Group;
            ++log2Delta;
        }
    }
    return (1u << log2Group) + (nDelta << log2Delta);
}

XCL_EXPORT uint32_t XCL_API
SizeClass_size2pages(uint32_t size)
{
    if (size <= PAGE_SIZE)
    {
        return 1;
    }
    uint32_t pages = size >> PAGE_SHIFTS;
    if ((size & (PAGE_SIZE - 1)) == 0)
    {
        return pages;
    }
    return pages + 1;
}

static int32_t
__initSizeClassTabs()
{
    int32_t idx = 0;
    int32_t size = 0;

    int32_t log2Group = LOG2_QUANTUM;
    int32_t log2Delta = LOG2_QUANTUM;

    int32_t nDelta = 0;
    int32_t nDeltaLimit = 1 << LOG2_GROUP_SIZE;
    while (nDelta < nDeltaLimit)
    {
        size = __initSizeClassTab(idx++, log2Group, log2Delta, nDelta++);
    }
    log2Group += LOG2_GROUP_SIZE;
    while (size < CHUNK_SIZE)
    {
        nDelta = 1;
        while (nDelta <= nDeltaLimit && size < CHUNK_SIZE)
        {
            size = __initSizeClassTab(idx++, log2Group, log2Delta, nDelta++);
        }
        ++log2Group;
        ++log2Delta;
    }
    assert(size == CHUNK_SIZE);
    return idx;
}

static int32_t
__initSizeClassTab(int32_t idx,
                   int32_t log2Group,
                   int32_t log2Delta,
                   int32_t nDelta)
{
    bool isMultiPage = false;
    int32_t size = (1 << log2Group) + (nDelta << log2Delta);
    if (log2Delta >= PAGE_SHIFTS)
    {
        isMultiPage = true;
    }
    else
    {
        isMultiPage = size % PAGE_SIZE == 0;
    }
    bool isSubPage = size <= SUB_PAGE_THRESHOLD;
    SizeTab tab = {log2Group, log2Delta, nDelta, isMultiPage, isSubPage, size};
    sizeClasses[idx] = tab;
    return size;
}
