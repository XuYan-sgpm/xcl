//
// Created by xuyan on 2022/7/14.
//

#include <errno.h>
#include <lang/XclErr.h>

XCL_EXPORT unsigned XCL_API
Err_get()
{
    return errno;
}

XCL_EXPORT void XCL_API
Err_set(unsigned errorCode)
{
    errno = (int)errorCode;
}
