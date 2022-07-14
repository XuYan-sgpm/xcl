//
// Created by xuyan on 2022/7/14.
//

#include <xcl/lang/XclErr.h>
#include <errno.h>

XCL_PUBLIC(unsigned)
error()
{ return errno; }

XCL_PUBLIC(void)
setErr(unsigned errorCode)
{ errno = (int)errorCode; }
