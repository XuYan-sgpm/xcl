//
// Created by 14049 on 2024/5/9.
//

#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Blocker* Blocker_new(void* mut, void* cond);
void Blocker_free(struct Blocker* blocker);
void Blocker_wait(struct Blocker* blocker);
bool Blocker_wait2(struct Blocker* blocker, int millis);
void Blocker_signal(struct Blocker* blocker);
void Blocker_broadcast(struct Blocker* blocker);

#ifdef __cplusplus
}
#endif
