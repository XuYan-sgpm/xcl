cmake_minimum_required(VERSION 3.15)

if(MSVC)
    cmake_policy(SET CMP0091 NEW)
endif()

project(xcl C CXX)

option(ENABLE_CXX_COMPILE "enable cxx compile" OFF)
option(BUILD_DYNAMIC_LIBRARY "build dynamic library" OFF)
option(ENABLE_C11 "enable c11 standard" OFF)
option(ENABLE_CXX11 "enable cxx11 standard" ON)

set(CMAKE_VERBOSE_MAKEFILE ON)

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
    set(wall
        "/Wall /wd4255 /wd5045 /wd4668 /wd4996 /wd4200 /wd4061 /wd4100 /wd4146")
    set(hidden)
else()
    set(wall "-Wall")
    set(hidden "-fvisibility=hidden")
endif()

set(CMAKE_C_FLAGS "${wall} ${hidden}")
set(CMAKE_CXX_FLAGS "${wall} ${hidden}")

message(STATUS "c flags:${CMAKE_C_FLAGS}")
message(STATUS "cxx flags:${CMAKE_CXX_FLAGS}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(build_type debug)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(build_type release)
endif()

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/${xcl_lib_type})

if(NOT MSVC)
    set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})
endif()

set(xcl_inc_dir include/public include/internal)
set(xcl_lib_dir ${LIBRARY_OUTPUT_PATH})
include_directories(${xcl_inc_dir})
link_directories(${xcl_lib_dir})

set(exclude_dirs)

message(STATUS ${CMAKE_SYSTEM_NAME})

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

if(NOT ENABLE_CXX_COMPILE)
    if(WIN32 AND CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(link_libs ${link_libs} atomic)
    endif()
endif()

target_link_libraries(${xcl_lib_name} ${link_libs})

if(MSVC)
    set_property(
        TARGET ${xcl_lib_name} PROPERTY MSVC_RUNTIME_LIBRARY
                                        "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_custom_command(
    TARGET ${xcl_lib_name}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${LIBRARY_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LIBRARY_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E echo
            "make library directories ${LIBRARY_OUTPUT_PATH}"
    VERBATIM)
