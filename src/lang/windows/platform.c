//
// Created by xuyan on 2022/7/4.
//

#include "lang/xcl_err.h"
#include "lang/system.h"
#include <Windows.h>
#include <profileapi.h>
#include <stdbool.h>
#include <sysinfoapi.h>

static int64_t __NANO_FREQ_PER_SEC = -1;

int64_t XCL_API
currentMillis()
{
    return GetTickCount();
}

int64_t XCL_API
nanos()
{
    if (__NANO_FREQ_PER_SEC == -1)
    {
        LARGE_INTEGER freq;
        if (QueryPerformanceFrequency(&freq))
        {
            __NANO_FREQ_PER_SEC = freq.QuadPart;
        }
    }
    if (__NANO_FREQ_PER_SEC > 0)
    {
        LARGE_INTEGER current;
        QueryPerformanceCounter(&current);
        double tmp = ((double)NANOS_PER_SEC / (double)__NANO_FREQ_PER_SEC);
        return (int64_t)((double)current.QuadPart * tmp);
    }
    else
    {
        return -1;
    }
}

void XCL_API
sleepMs(int32_t timeout)
{
    timeBeginPeriod(1);
    Sleep(timeout);
    timeEndPeriod(1);
}

bool
__waitHandle(HANDLE handle, DWORD timeout)
{
    DWORD ret = WaitForSingleObject(handle, timeout);
    if (ret != WAIT_OBJECT_0)
    {
        if (ret == WAIT_FAILED)
        {
            Err_set(GetLastError());
        }
        else
        {
            Err_set(ret);
        }
        return false;
    }
    else
    {
        return true;
    }
}

bool XCL_API
currentWorkingDir(TChar* cwd, int32_t len)
{
    if (!cwd)
    {
        return false;
    }
    DWORD ret = GetModuleFileName(NULL, cwd, len);
    if (ret == 0)
    {
        Err_set(GetLastError());
        return false;
    }
    if (ret >= len)
    {
        cwd[0] = 0;
        return false;
    }
    cwd[ret] = 0;
    char* p = strrchr(cwd, '\\');
    *p = 0;
    return true;
}
