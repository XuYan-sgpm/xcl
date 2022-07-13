//
// Created by xy on 7/11/22.
//

#include <xcl/lang/XclDef.h>

#if _MSC_VER && defined(DYNAMIC)

void
__LocalId_initQueue();
void
__allocTls();

/**
 * write init code there
 * invoked at current process attach
 */
void
__MsvcImplGlobalInit() {
  __LocalId_initQueue();
  __allocTls();
}

#endif
