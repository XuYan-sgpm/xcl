//
// Created by xy on 7/10/22.
//

#include <errno.h>
#include <stddef.h>
#include <sys/time.h>
#include <time.h>
#include <xcl/lang/system.h>

#if _POSIX_C_SOURCE < 199309L
#  include <unistd.h>
#endif

int64_t XCL_API
currentMillis()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    int64_t s1 = (int64_t)(time.tv_sec) * 1000;
    int64_t s2 = (time.tv_usec / 1000);
    return s1 + s2;
}

int64_t XCL_API
nanos()
{
#ifdef CLOCK_MONOTONIC
    struct timespec tv = {0, 0};
    clock_gettime(CLOCK_REALTIME, &tv);
    return (int64_t)tv.tv_sec * 1000000000 + tv.tv_nsec;
#else
    struct timeval time;
    gettimeofday(&time, NULL);
    int64_t s1 = (int64_t)(time.tv_sec) * 1000000;
    int64_t s2 = (time.tv_usec);
    return 1000 * (s1 + s2);
#endif
}

void XCL_API
mSleep(int32_t timeout)
{
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts = {0, 0};
    ts.tv_sec = timeout / 1000;
    ts.tv_nsec = (int64_t)(timeout - ts.tv_sec * 1000) * 1000000L;
    nanosleep(&ts, NULL);
#else
    usleep((int64_t)timeout * 1000);
#endif
}
