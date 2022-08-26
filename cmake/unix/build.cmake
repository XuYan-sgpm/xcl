set_arch_compile_flags(${COMPILE_X86_TARGET})

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")

else()

endif()

set(exclude_dirs macos macosx windows win32 ios apple)
set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})
set(link_libs pthread)
make_xcl_lib()
