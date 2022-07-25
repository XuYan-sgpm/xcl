cmake_minimum_required(VERSION 3.15)

project(xcl C CXX)

set(BUILD_LIB_TYPE dynamic)

include(cmake/util.cmake)

check_c_cxx_compiler(MSVC Clang GNU AppleClang)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

set(CMAKE_DEBUG_POSTFIX "d")
set(xcl_lib_name xcl)

if (NOT DEFINED CMAKE_BUILD_TYPE)
    message(WARNING "cmake build type is not specified, auto set to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif ()

if (NOT DEFINED BUILD_LIB_TYPE)
    set(BUILD_LIB_TYPE static)
endif ()
if (BUILD_LIB_TYPE STREQUAL "static")
    add_definitions(-DSTATIC)
    set(compile_static_lib true)
elseif (BUILD_LIB_TYPE STREQUAL "dynamic")
    add_definitions(-DDYNAMIC)
    set(compile_static_lib false)
else ()
    message(FATAL_ERROR "invalid library type:${BUILD_LIB_TYPE}")
endif ()
add_definitions(-DBUILD_LIBRARY)

set(xcl_lib_type "${BUILD_LIB_TYPE}")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(wall "/Wall")
    set(hidden)
    add_definitions(-DENABLE_CXX)
    message(STATUS "detect msvc compiler, enable cxx")
else ()
    set(wall "-Wall")
    set(hidden "-fvisibility=hidden")
endif ()

set(CMAKE_C_FLAGS "${wall} ${hidden}")
set(CMAKE_CXX_FLAGS "${wall} ${hidden}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(build_type debug)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(build_type release)
endif ()

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/${xcl_lib_type}/${build_type})

set(xcl_inc_dir include/public include/internal)
set(xcl_lib_dir ${LIBRARY_OUTPUT_PATH})
include_directories(${xcl_inc_dir})
link_directories(${xcl_lib_dir})

set(lib_src_files)
set(exclude_dirs)

message(STATUS ${CMAKE_SYSTEM_NAME})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND exclude_dirs macosx linux unix)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND exclude_dirs windows linux)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND exclude_dirs windows macosx)
else ()
    message(FATAL_ERROR "unsupported platforms")
endif ()

collect_source_files(${CMAKE_CURRENT_LIST_DIR}/src ${exclude_dirs})

foreach (source_file ${lib_src_files})
    message(STATUS ${source_file})
endforeach ()

if (compile_static_lib)
    message(STATUS "static")
    add_library(${xcl_lib_name} STATIC ${lib_src_files})
else ()
    message(STATUS "dynamic")
    add_library(${xcl_lib_name} SHARED ${lib_src_files})
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(STATUS "windows")
    add_compile_definitions(_WIN32_WINNT=0x0A00)
    target_link_libraries(${xcl_lib_name} winmm)
else ()
    if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        message(STATUS "macos")
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        message(STATUS "linux")
    endif ()
    target_link_libraries(${xcl_lib_name} pthread)
endif ()

add_custom_command(TARGET ${xcl_lib_name} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${LIBRARY_OUTPUT_PATH}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${LIBRARY_OUTPUT_PATH}
        COMMAND ${CMAKE_COMMAND} -E echo "make library directories ${LIBRARY_OUTPUT_PATH}"
        VERBATIM)
