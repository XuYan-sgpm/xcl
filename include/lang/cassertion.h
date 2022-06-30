//
// Created by xuyan on 2022/6/30.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void assertMsg(bool cond, const char *msg);
void assertFmt(bool cond, const char *fmt, ...);

#ifdef __cplusplus
}
#endif
