//
// Created by 14049 on 2024/1/23.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Factory Factory;
struct Factory {
  struct {
    void* (*create)(Factory*);
  }* __vptr;
};

#ifdef __cplusplus
}
#endif
