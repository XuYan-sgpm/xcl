//
// Created by xuyan on 2022/7/18.
//

#pragma once

#include <xcl/lang/CLocalStorage.h>
#include <xcl/lang/XclDef.h>
#include <xcl/lang/CInterThreadApi.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    CLocalStorage* localStorage;
    ThreadHandle handle;
} __RegData;

void __initializeRegQueue();

void __regLocalStorage(CLocalStorage* localStorage);

void __deregisterLocalStorage(ThreadHandle handle);

void __clearObsoleteStorages();

bool __hasRegLocalStorage();

#ifdef __cplusplus
}
#endif
