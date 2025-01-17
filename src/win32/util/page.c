//
// Created by 徐琰 on 2023/10/23.
//

#include <Windows.h>
#include <xcl/util/algo.h>
#include <xcl/util/page.h>

size_t Page_size() {
  static DWORD page_size = 0;
  if (!page_size) {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    page_size = info.dwPageSize;
  }
  return page_size;
}

void* Page_alloc(void* hint, size_t pages) {
  return VirtualAlloc(NULL, pages * Page_size(), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void Page_free(void* p, size_t pages) { VirtualFree(p, 0, MEM_RELEASE); }
