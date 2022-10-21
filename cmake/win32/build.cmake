set_arch_compile_flags(${COMPILE_X86_TARGET})
if(MSVC)
    message(STATUS "debug msvc:${CMAKE_BUILD_TYPE}")
    # set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})
else()
    if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
        if(COMPILE_X86_TARGET)

        else()
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=skylake")
        endif()
    elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(link_libs ${link_libs} atomic)
    endif()
    set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})
endif()

add_compile_definitions(_WIN32_WINNT=0x0A00)
set(link_libs winmm ntdll)

set(exclude_dirs unix macosx linux macos apple ios)
make_xcl_lib()

if(MSVC)
    if(NOT DEFINED FORCE_MDd_STATIC)
        option(FORCE_MDd_STATIC "force set msvc MDd for static lib" OFF)
    endif()

    if(NOT COMPILE_STATIC_LIB OR FORCE_MDd_STATIC)
        message(STATUS "xcl:MDd")
        set_property(
            TARGET ${xcl_lib_name}
            PROPERTY MSVC_RUNTIME_LIBRARY
                     "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
        message(STATUS "xcl:MTd")
        set_property(
            TARGET ${xcl_lib_name}
            PROPERTY MSVC_RUNTIME_LIBRARY
                     "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()
