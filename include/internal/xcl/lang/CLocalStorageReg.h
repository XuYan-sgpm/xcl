//
// Created by xuyan on 2022/7/18.
//

#pragma once

#include <xcl/lang/CLocalStorage.h>
#include <xcl/lang/XclDef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if WINDOWS
typedef unsigned long ThreadId;
#else
typedef long ThreadId;
#endif

typedef struct {
    CLocalStorage* localStorage;
    ThreadId threadId;
} __RegData;

void __initializeRegQueue();

void __regLocalStorage(CLocalStorage* localStorage);

void __deregisterLocalStorage(ThreadId tid);

void __clearObsoleteStorages();

bool __hasRegLocalStorage();

#ifdef __cplusplus
}
#endif
