cmake_minimum_required(VERSION 3.15)

if(NOT DEFINED ENABLE_CXX_COMPILE)
    message(STATUS "ENABLE_CXX_COMPILE not passing")
    option(ENABLE_CXX_COMPILE "enable cxx compile" OFF)
endif()
if(NOT DEFINED BUILD_DYNAMIC_LIBRARY)
    message(STATUS "BUILD_DYNAMIC_LIBRARY not passing")
    option(BUILD_DYNAMIC_LIBRARY "build dynamic library" ON)
endif()
if(NOT DEFINED ENABLE_C11)
    message(STATUS "ENABLE_C11 not passing")
    option(ENABLE_C11 "enable c11 standard" OFF)
endif()
if(NOT DEFINED ENABLE_CXX11)
    message(STATUS "ENABLE_CXX11 not passing")
    option(ENABLE_CXX11 "enable cxx11 standard" ON)
endif()
if(NOT DEFINED COMPILE_X86_TARGET)
    message(STATUS "COMPILE_X86_TARGET not passing")
    option(COMPILE_X86_TARGET "compile x86 target" OFF)
endif()
if(NOT DEFINED UNICODE)
    message(STATUS "UNICODE not passing")
    option(UNICODE "using unicode for path" OFF)
endif()

project(xcl C CXX)

message(STATUS "platform:${CMAKE_SYSTEM_NAME}")
if(APPLE)
    if(COMPILE_X86_TARGET)
        message(WARNING "x86 build is deprecated on macosx, turn off")
        set(COMPILE_X86_TARGET OFF)
    endif()
elseif(MSVC)
    if(COMPILE_X86_TARGET)
        set(CMAKE_MODULE_LINKER_FLAGS "/machine:x86")
    else()
        set(CMAKE_MODULE_LINKER_FLAGS "/machine:x64")
    endif()
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL
                                             "Clang")
    if(COMPILE_X86_TARGET)
        set(CMAKE_C_FLAGS "-m32")
        set(CMAKE_CXX_FLAGS "-m32")
    else()
        set(CMAKE_C_FLAGS "-m64")
        set(CMAKE_CXX_FLAGS "-m64")
    endif()
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        message(STATUS "clang on windows")
        if(COMPILE_X86_TARGET)

        else()
            set(CMAKE_C_FLAGS "-march=skylake")
        endif()
    endif()
endif()

if(COMPILE_X86_TARGET)
    message(STATUS "compile x86 target")
    add_compile_definitions(X86=1 X64=0)
    set(architecture x86)
else()
    message(STATUS "compile x64 target")
    add_compile_definitions(X86=0 X64=1)
    set(architecture x64)
endif()

if(UNICODE)
    add_definitions(-DUNICODE -D_UNICODE)
    message(STATUS "enable unicode")
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)

if(NOT CMAKE_C_COMPILER_ID STREQUAL CMAKE_CXX_COMPILER_ID)
    message(FATAL_ERROR "xcl use same compiler for c and cxx code")
endif()

include(cmake/util.cmake)

check_c_cxx_compiler("MSVC" "Clang" "GNU" "AppleClang")

if(ENABLE_C11)
    set(CMAKE_C_STANDARD 11)
    add_definitions(-DENABLE_C11)
else()
    set(CMAKE_C_STANDARD 99)
endif()
if(ENABLE_CXX11)
    set(CMAKE_CXX_STANDARD 11)
    add_definitions(-DENABLE_CXX11)
else()
    set(CMAKE_CXX_STANDARD 98)
endif()

message(STATUS "CMAKE_C_STANDARD:${CMAKE_C_STANDARD}")
message(STATUS "CMAKE_CXX_STANDARD:${CMAKE_CXX_STANDARD}")

if(ENABLE_CXX_COMPILE)
    add_definitions(-DENABLE_CXX)
    message(STATUS "enable cxx compile")
endif()

set(CMAKE_DEBUG_POSTFIX "d")
set(xcl_lib_name xcl)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    message(WARNING "cmake build type is not specified, auto set to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(BUILD_DYNAMIC_LIBRARY)
    set(BUILD_LIB_TYPE dynamic)
else()
    set(BUILD_LIB_TYPE static)
endif()

if(BUILD_LIB_TYPE STREQUAL "static")
    add_definitions(-DSTATIC)
    set(compile_static_lib true)
elseif(BUILD_LIB_TYPE STREQUAL "dynamic")
    add_definitions(-DDYNAMIC)
    set(compile_static_lib false)
else()
    message(FATAL_ERROR "invalid library type:${BUILD_LIB_TYPE}")
endif()

add_definitions(-DBUILD_LIBRARY)

set(xcl_lib_type "${BUILD_LIB_TYPE}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(wall)
    set(hidden)
else()
    set(wall "-Wall")
    set(hidden "-fvisibility=hidden")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${wall} ${hidden}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${wall} ${hidden}")

message(STATUS "c flags:${CMAKE_C_FLAGS}")
message(STATUS "cxx flags:${CMAKE_CXX_FLAGS}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(build_type debug)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(build_type release)
endif()

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/${xcl_lib_type})
if(COMPILE_X86_TARGET)
    set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/x86)
else()
    set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/x64)
endif()

if(NOT MSVC)
    set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})
endif()

set(xcl_inc_dir include private/inc)
set(xcl_lib_dir ${LIBRARY_OUTPUT_PATH})
include_directories(${xcl_inc_dir})
link_directories(${xcl_lib_dir})

set(exclude_dirs)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND exclude_dirs macosx linux unix)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND exclude_dirs windows linux)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND exclude_dirs windows macosx)
else()
    message(FATAL_ERROR "unsupported platforms")
endif()

collect_source_files(${ENABLE_CXX_COMPILE} ${CMAKE_CURRENT_LIST_DIR}/src
                     ${exclude_dirs})

if(NOT LIBRARY_SOURCE_FILES)
    message(WARNING "no library source file")
    return()
else()
    foreach(source_file ${LIBRARY_SOURCE_FILES})
        message(STATUS ${source_file})
    endforeach()
endif()

if(compile_static_lib)
    message(STATUS "static")
    add_library(${xcl_lib_name} STATIC ${LIBRARY_SOURCE_FILES})
else()
    message(STATUS "dynamic")
    add_library(${xcl_lib_name} SHARED ${LIBRARY_SOURCE_FILES})
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_compile_definitions(_WIN32_WINNT=0x0A00)
    set(link_libs winmm ntdll)
else()
    set(link_libs pthread)
endif()

if(WIN32)
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(link_libs ${link_libs} atomic)
    endif()
endif()

target_link_libraries(${xcl_lib_name} ${link_libs})

if(MSVC)
    set_property(
        TARGET ${xcl_lib_name} PROPERTY MSVC_RUNTIME_LIBRARY
                                        "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(APPLE)

endif()

add_custom_command(
    TARGET ${xcl_lib_name}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${LIBRARY_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LIBRARY_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E echo
            "make library directories ${LIBRARY_OUTPUT_PATH}"
    VERBATIM)

install(
    CODE "execute_process(COMMAND python -u ${CMAKE_CURRENT_LIST_DIR}/install.py ${CMAKE_INSTALL_PREFIX} ${CMAKE_BUILD_TYPE} ${architecture} ${BUILD_LIB_TYPE} ${CMAKE_CURRENT_LIST_DIR})"
)
