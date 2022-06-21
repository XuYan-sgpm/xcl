//
// Created by xuyan on 2022/6/20.
//

#pragma once

#include <lang/platform.h>

#ifdef __cplusplus
extern "C" {
#endif

void setLocal(void *obj);
void *getLocal();

#ifdef __cplusplus
}
#endif
