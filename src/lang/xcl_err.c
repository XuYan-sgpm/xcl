//
// Created by xuyan on 2022/7/14.
//

#include <errno.h>
#include <xcl/lang/xcl_err.h>

XCL_EXPORT unsigned XCL_API
Err_get()
{
    return errno;
}

XCL_EXPORT void XCL_API
Err_set(unsigned error_code)
{
    errno = (int)error_code;
}
