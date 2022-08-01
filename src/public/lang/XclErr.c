//
// Created by xuyan on 2022/7/14.
//

#include <errno.h>
#include <xcl/lang/XclErr.h>

XCL_PUBLIC(unsigned)
Err_get()
{
    return errno;
}

XCL_PUBLIC(void)
Err_set(unsigned errorCode)
{
    errno = (int)errorCode;
}
