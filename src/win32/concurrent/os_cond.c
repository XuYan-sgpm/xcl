//
// Created by xuyan on 2022/7/5.
//

#include <Windows.h>
#include <xcl/concurrent/condition.h>

bool Cond_init(void* obj) {
  InitializeConditionVariable(obj);
  return true;
}
void* Cond_new() {
  void* cond = malloc(sizeof(CONDITION_VARIABLE));
  if (cond)
    Cond_init(cond);
  return cond;
}

void Cond_destroy(void* obj) {}
bool Cond_delete(void* cond) {
  free(cond);
  return true;
}

bool Cond_wait(void* mutex, void* cond) {
  return cond && SleepConditionVariableCS(cond, mutex, INFINITE);
}

bool Cond_waitFor(void* mutex, void* cond, unsigned millis) {
  return cond && mutex && SleepConditionVariableCS(cond, mutex, millis);
}

bool Cond_signal(void* cond) { return cond ? WakeAllConditionVariable(cond), true : false; }

bool Cond_broadcast(void* cond) { return cond ? WakeAllConditionVariable(cond), true : false; }
