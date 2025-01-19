#include <internal/lang/local_reserve_id.h>
#include <internal/lang/local_storage.h>
#include <xcl/lang/thread_local.h>

LocalPair* Local_get0(uintptr_t id);
unsigned* _usr_err() {
  LocalPair* ld = Local_get0(LOCAL_ID_ERROR);
  return (unsigned*)&ld->attach;
}
