//
// Created by xuyan on 2022/7/4.
//

#include "xcl/lang/XclErr.h"
#include "xcl/lang/CSys.h"
#include <Windows.h>
#include <profileapi.h>
#include <stdbool.h>
#include <sysinfoapi.h>

static int64_t __NANO_FREQ_PER_SEC = -1;

static void
__SYS_initNanoFreq()
{
    LARGE_INTEGER freq;
    if (QueryPerformanceFrequency(&freq))
    {
        __NANO_FREQ_PER_SEC = freq.QuadPart;
    }
}

XCL_PUBLIC(int64_t)
currentMillis()
{
    return GetTickCount();
}

XCL_PUBLIC(int64_t)
nanos()
{
    if (__NANO_FREQ_PER_SEC == -1)
    {
        __SYS_initNanoFreq();
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

XCL_PUBLIC(void)
mSleep(int32_t timeout)
{
    timeBeginPeriod(1);
    Sleep(timeout);
    timeEndPeriod(1);
}

bool
__Win32_wait(HANDLE handle, DWORD timeout)
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
