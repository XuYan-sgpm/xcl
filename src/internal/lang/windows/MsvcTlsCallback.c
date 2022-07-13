//
// Created by xuyan on 2022/7/9.
//

/**
 * using for global initialization for xcl library
 * not using c++ class but compiler feature to make
 * sure it can works on compiler which not support
 * cpp. this is only necessary for msvc compiler.
 * however, msvc is a big problem, but we should
 * make it compatible
 */

#if _MSC_VER && defined(DYNAMIC)

#  include <xcl/lang/CBaseThreadImpl.h>
#  include <windows.h>
#  pragma comment(lib, "ntdll.lib")

void
__MsvcImplGlobalInit();

void NTAPI
__GlobalInit_cb(PVOID DllHandle, DWORD Reason, PVOID Reserved) {
  switch (Reason) {
  case DLL_PROCESS_ATTACH: {
    __MsvcImplGlobalInit();
    break;
  }
  case DLL_THREAD_DETACH: {
    __Thread_releaseLocalStorage();
    break;
  }
  default:
    break;
  }
}

#  ifdef _WIN64
#    pragma comment(linker, "/INCLUDE:_tls_used")
#    pragma comment(linker, "/INCLUDE:tls_callback_func")
#  else
#    pragma comment(linker, "/INCLUDE:__tls_used")
#    pragma comment(linker, "/INCLUDE:_tls_callback_func")
#  endif

#  ifdef _WIN64
#    pragma const_seg(".CRT$XLF")
const
#  else
#    pragma data_seg(".CRT$XLF")
#  endif
    PIMAGE_TLS_CALLBACK tls_callback_func = __GlobalInit_cb;
#  ifdef _WIN64
#    pragma const_seg()
#  else
#    pragma data_seg()
#  endif //_WIN64

#endif