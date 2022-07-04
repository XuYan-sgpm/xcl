//
// Created by xuyan on 2022/6/30.
//

#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void assertIf(bool cond, const char *msg);
void assertMessage(bool cond, const char *fmt, ...);

#ifdef __cplusplus
}
#endif
