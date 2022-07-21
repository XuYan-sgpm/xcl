//
// Created by xuyan on 2022/7/21.
//

#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include "xcl/lang/CThreadProcess.h"
#include "xcl/util/algo.h"

typedef struct {
    int32_t count;
    const CThreadState* states;
} CThreadStateWhiteList;

const static CThreadState __ThreadProcess_suspendList[] = {NORMAL};
const static CThreadState __ThreadProcess_normalList[] = {CLEANING,
                                                          WAITING,
                                                          DETACHED};
const static CThreadState __ThreadProcess_terminatedList[] = {};
const static CThreadState __ThreadProcess_invalidList[] = {SUSPEND, NORMAL};
const static CThreadState __ThreadProcess_detachedList[] = {};
const static CThreadState __ThreadProcess_waitingList[] = {CLEANING,
                                                           TERMINATED};
const static CThreadState __ThreadProcess_cleaningList[] = {WAITING,
                                                            TERMINATED,
                                                            DETACHED};

const static CThreadStateWhiteList __ThreadProcess_whiteLists[] = {
    {sizeof(__ThreadProcess_suspendList) / sizeof(CThreadState),
     __ThreadProcess_suspendList},
    {sizeof(__ThreadProcess_normalList) / sizeof(CThreadState),
     __ThreadProcess_normalList},
    {sizeof(__ThreadProcess_terminatedList) / sizeof(CThreadState),
     __ThreadProcess_terminatedList},
    {sizeof(__ThreadProcess_invalidList) / sizeof(CThreadState),
     __ThreadProcess_invalidList},
    {sizeof(__ThreadProcess_detachedList) / sizeof(CThreadState),
     __ThreadProcess_detachedList},
    {sizeof(__ThreadProcess_waitingList) / sizeof(CThreadState),
     __ThreadProcess_waitingList},
    {sizeof(__ThreadProcess_cleaningList) / sizeof(CThreadState),
     __ThreadProcess_cleaningList},
};

void ThreadProcess_init(CThreadProcess* process)
{
    process->cursor = -1;
    memset(process->tabs, 0, sizeof(process->tabs));
}

void ThreadProcess_push(CThreadProcess* process, CThreadState state)
{
    if (process->cursor == sizeof(process->tabs) / sizeof(CThreadState))
    {
        assert(false);
    }
    CThreadState previous = process->tabs[process->cursor];
    const CThreadStateWhiteList wl =
        __ThreadProcess_whiteLists[_log2(previous)];
    for (int i = 0; i < wl.count; i++)
    {
        if (state == wl.states[i])
        {
            process->tabs[++process->cursor] = state;
            return;
        }
    }
    assert(false);
}

bool ThreadProcess_check(CThreadProcess* process,
                         bool exist,
                         CThreadState state)
{
    if (process->cursor < 0)
    {
        if (exist)
        {
            return false;
        }
    }
    bool ret = !exist;
    for (int i = 0; i <= process->cursor; i++)
    {
        if (state == process->tabs[i])
        {
            ret = !ret;
            break;
        }
    }
    return ret;
}

bool ThreadProcess_check2(CThreadProcess* process, bool exist, int32_t n, ...)
{
    assert(n >= 0);
    if (n == 0)
    {
        return true;
    }
    if (process->cursor < 0)
    {
        if (exist)
        {
            return false;
        }
    }
    bool ret = !exist;
    va_list states;
    va_start(states, n);
    for (int i = 0; i < n && ret == !exist; i++)
    {
        CThreadState state = va_arg(states, CThreadState);
        for (int j = 0; j <= process->cursor; j++)
        {
            if (state == process->tabs[j])
            {
                ret = !ret;
                break;
            }
        }
    }
    va_end(states);
    return ret;
}

CThreadState ThreadProcess_current(CThreadProcess* process)
{
    assert(process->cursor >= 0);
    return process->tabs[process->cursor];
}
