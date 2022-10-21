//
// Created by 徐琰 on 2022/6/26.
//

#include "pool/size_class.h"
#include "pool/pool_def.h"
#include "pool/pool.h"
#include "util/algo.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    int32_t log2_group;
    int32_t log2_delta;
    int32_t n_delta;
    bool is_multi_page;
    bool is_sub_page;
    //   int32_t log2DeltaLookup;
    int32_t size;
} SizeTab;

static SizeTab* size_classes = NULL;

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
                   int32_t log2_group,
                   int32_t log2_delta,
                   int32_t n_delta);

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
    if (size_classes)
    {
        return true;
    }
    bool success = false;
    do
    {
        uint32_t groups = _log2(CHUNK_SIZE) + 1 - LOG2_QUANTUM;
        size_classes = (SizeTab*)Pool_alloc(NULL,
                                            sizeof(SizeTab)
                                                * (groups << LOG2_GROUP_SIZE));
        if (!size_classes)
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
                 size_classes,
                 sizeof(SizeTab) * (groups << LOG2_GROUP_SIZE));
    size_classes = NULL;
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
    SizeTab tab = size_classes[idx];
    out[0] = tab.log2_group;
    out[1] = tab.log2_delta;
    out[2] = tab.n_delta;
    out[3] = tab.is_multi_page;
    out[4] = tab.is_sub_page;
    out[5] = tab.size;
    return true;
}

static void
__SC_getGroupAndDelta(uint32_t size, uint32_t* log2_group, uint32_t* log2_delta)
{
    uint32_t log2Size = _log2(size);
    if (size <= (1 << (LOG2_QUANTUM + LOG2_GROUP_SIZE)))
    {
        *log2_group = LOG2_QUANTUM;
        *log2_delta = *log2_group;
    }
    else
    {
        *log2_group = log2Size;
        *log2_delta = *log2_group - LOG2_GROUP_SIZE;
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
    uint32_t log2_group, log2_delta;
    __SC_getGroupAndDelta(size, &log2_group, &log2_delta);
    uint32_t n_delta = (size - (1 << log2_group)) >> log2_delta;
    if (size - ((1 << log2_group) + (n_delta << log2_delta)) == 0)
    {
        return size;
    }
    if (log2_group == LOG2_QUANTUM)
    {
        if (n_delta < 3)
        {
            ++n_delta;
        }
        else
        {
            n_delta = 1;
            log2_group += LOG2_GROUP_SIZE;
        }
    }
    else
    {
        if (n_delta < 4)
        {
            ++n_delta;
        }
        else
        {
            n_delta = 1;
            ++log2_group;
            ++log2_delta;
        }
    }
    return (1u << log2_group) + (n_delta << log2_delta);
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

    int32_t log2_group = LOG2_QUANTUM;
    int32_t log2_delta = LOG2_QUANTUM;

    int32_t n_delta = 0;
    int32_t n_delta_limit = 1 << LOG2_GROUP_SIZE;
    while (n_delta < n_delta_limit)
    {
        size = __initSizeClassTab(idx++, log2_group, log2_delta, n_delta++);
    }
    log2_group += LOG2_GROUP_SIZE;
    while (size < CHUNK_SIZE)
    {
        n_delta = 1;
        while (n_delta <= n_delta_limit && size < CHUNK_SIZE)
        {
            size = __initSizeClassTab(idx++, log2_group, log2_delta, n_delta++);
        }
        ++log2_group;
        ++log2_delta;
    }
    assert(size == CHUNK_SIZE);
    return idx;
}

static int32_t
__initSizeClassTab(int32_t idx,
                   int32_t log2_group,
                   int32_t log2_delta,
                   int32_t n_delta)
{
    bool is_multi_page = false;
    int32_t size = (1 << log2_group) + (n_delta << log2_delta);
    if (log2_delta >= PAGE_SHIFTS)
    {
        is_multi_page = true;
    }
    else
    {
        is_multi_page = size % PAGE_SIZE == 0;
    }
    bool is_sub_page = size <= SUB_PAGE_THRESHOLD;
    SizeTab tab
        = {log2_group, log2_delta, n_delta, is_multi_page, is_sub_page, size};
    size_classes[idx] = tab;
    return size;
}
