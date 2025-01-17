#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum LocalId {
  LOCAL_ID_EXIT_HANDLER = 0,
  LOCAL_ID_ERROR,
  LOCAL_ID_CURR_THREAD,
  LOCAL_ID_PATH_BUFFER,
  LOCAL_ID_MAX_RESERVED = 8,
};

#ifdef __cplusplus
}
#endif
