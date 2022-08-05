#include "xcl/util/CRbTree.h"
#include <xcl/lang/XclErr.h>
#include <stdlib.h>

CRbNode*
__RbTree_allocNode()
{
#ifdef _MSC_VER
    void* p = _aligned_malloc(sizeof(CRbNode), sizeof(long));
#else
    void* p = 0;
    int ret = posix_memalign(&p, sizeof(CRbNode), sizeof(long));
    if (ret)
    {
        Err_set(ret);
    }
#endif
    return p;
}