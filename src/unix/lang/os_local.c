//
// Created by 徐琰 on 2023/10/17.
//

#include <errno.h>
#include <internal/lang/local_storage.h>
#include <internal/lang/os_local.h>
#include <pthread.h>

void* osLocalGet(unsigned long key) { return pthread_getspecific(key); }
void osLocalSet(unsigned long key, void* obj) { pthread_setspecific(key, obj); }
unsigned long allocLocalKey() {
  pthread_key_t key;
  int err = pthread_key_create(&key, LocalStorage_free);
  if (err)
    errno = err, key = INVALID_LOCAL_KEY;
  return key;
}
void destroyLocalKey(unsigned long key) { pthread_key_delete(key); }
