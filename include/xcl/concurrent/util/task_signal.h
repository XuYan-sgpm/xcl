#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  TASK_STATE_INIT,
  TASK_STATE_EXECUTING,
  TASK_STATE_CANCELLED,
  TASK_STATE_ERROR,
  TASK_STATE_COMPLETE
} TaskState;

typedef struct TaskSignal TaskSignal;
void TaskSignal_free(TaskSignal* sig);
bool TaskSignal_trigger(TaskSignal* sig, int state);
int TaskSignal_state(TaskSignal* sig);
int TaskSignal_wait(TaskSignal* sig, int timeout);

#ifdef __cplusplus
}
#endif
