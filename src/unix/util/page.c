//
// Created by 徐琰 on 2023/10/23.
//

#include <sys/mman.h>
#include <unistd.h>
#include <xcl/util/page.h>

void* Page_alloc(void* p, size_t pages) {
  p = mmap(p, pages * getpagesize(), PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
  if (p == MAP_FAILED)
    return NULL;
  return p;
}

void Page_free(void* p, size_t pages) { munmap(p, pages * getpagesize()); }

size_t Page_size() { return getpagesize(); }
