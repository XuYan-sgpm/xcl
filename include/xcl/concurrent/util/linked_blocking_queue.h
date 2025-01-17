//
// Created by 14049 on 2024/5/5.
//

#pragma once

#include <stdbool.h>
#include <xcl/concurrent/util/blocking_queue.h>

#ifdef __cplusplus
extern "C" {
#endif

BlockingQueue* LinkedBlockingQueue_new(int value_size, int max_size);
void LinkedBlockingQueue_free(BlockingQueue* bq);

#ifdef __cplusplus
}
#endif
