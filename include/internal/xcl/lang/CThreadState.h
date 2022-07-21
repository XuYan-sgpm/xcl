//
// Created by xuyan on 2022/7/21.
//

#pragma once

typedef enum
{
    INVALID = 1 << 0,
    SUSPEND = 1 << 1,
    NORMAL = 1 << 2,
    CLEANING = 1 << 3,
    WAITING = 1 << 4,
    TERMINATED = 1 << 5,
    DETACHED = 1 << 6
} CThreadState;
