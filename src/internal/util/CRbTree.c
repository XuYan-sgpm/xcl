#include "xcl/util/CRbTree.h"
#include <xcl/lang/XclErr.h>
#include <stdlib.h>
#include <stdint.h>

CRbNode*
__RbTree_allocNode(int32_t size)
{
#ifdef _MSC_VER
    void* p = _aligned_malloc(sizeof(CRbNode) + size, sizeof(long));
    if (!p)
    {
        Err_set(XCL_MEMORY_ERR);
    }
#else
    void* p = 0;
    int ret = posix_memalign(&p, sizeof(CRbNode) + size, sizeof(long));
    if (ret)
    {
        Err_set(ret);
    }
#endif
    return p;
}

void
__RbTree_deallocNode(CRbNode* node, int32_t size)
{
#ifdef _MSC_VER
    _aligned_free(node);
#else
    free(node);
#endif
}
