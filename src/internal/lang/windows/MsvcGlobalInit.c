//
// Created by xy on 7/11/22.
//

#include <xcl/lang/XclDef.h>

#if MSVC
void
__LocalId_initQueue();
void
__LocalInit_allocTls();

/**
 * write init code there
 * invoked at current process attach
 */
void
__MsvcImplGlobalInit() {
  __LocalId_initQueue();
  __LocalInit_allocTls();
}
#endif