cmake_minimum_required(VERSION 3.10)
project(xcl C CXX)

include(${PROJECT_SOURCE_DIR}/cmake/util.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/platform.cmake)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

if(ANDROID OR IOS OR NOT CMAKE_SYSTEM_NAME STREQUAL ${CMAKE_HOST_SYSTEM_NAME})
    set(BUILD_TEST OFF)
endif()

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
endif()

# 设置compiler到缓存变量，否则vscode没有高亮，这个问题在低版本cmake发现
set(CMAKE_C_COMPILER ${CMAKE_C_COMPILER} CACHE STRING "" FORCE)
set(CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER} CACHE STRING "" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(target xcl)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(SDK_STD 202501)
set(SDK_VERSION 2.2.2)

add_xcl(${target})
xcl_build(${target})

if(CMAKE_INSTALL_PREFIX)
    xcl_install(${target})
endif()

if(BUILD_TEST)
    add_subdirectory(tests)
endif()
