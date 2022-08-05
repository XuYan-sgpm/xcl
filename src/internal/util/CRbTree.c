#include "xcl/util/CRbTree.h"
#include <stdlib.h>

CRbNode*
__RbTree_allocNode()
{
#ifdef _MSC_VER
    void* p = _aligned_malloc(sizeof(CRbNode), sizeof(long));
#else
    void* p = posix_memalign(sizeof(CRbNode), sizeof(long));
#endif
    return p;
}