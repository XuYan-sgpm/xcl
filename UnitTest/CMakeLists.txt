cmake_minimum_required(VERSION 3.15)

if(MSVC)
    cmake_policy(SET CMP0091 NEW)
endif()

project(UnitTest)

set(CMAKE_VERBOSE_MAKEFILE ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(build_type debug)
    set(xcl_lib_name xcld)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(build_type release)
    set(xcl_lib_name xcl)
endif()

set(CMAKE_CXX_STANDARD 11)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/bin)
if(NOT MSVC)
    set(EXECUTABLE_OUTPUT_PATH
        ${CMAKE_CURRENT_LIST_DIR}/bin/${CMAKE_BUILD_TYPE})
endif()

set(run_path ${CMAKE_CURRENT_LIST_DIR}/bin/${CMAKE_BUILD_TYPE})

file(GLOB test_src_files src/*.cc src/*.c)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(compiler_id msvc)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(WIN32)
        set(compiler_id mingw64)
    else()
        set(compiler_id gnuc)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL
                                                 "AppleClang")
    set(compiler_id clang)
endif()

message(STATUS ${compiler_id})

# set(xcl_lib_type dynamic)

if(WIN32)
    set(third_party D:/third_party/${compiler_id}/${build_type})
elseif(APPLE)
    set(third_party /usr/local/${compiler_id})
else()
    set(third_party /usr/local/${compiler_id}/${build_type})
endif()

message(STATUS ${CMAKE_BUILD_TYPE})
message(STATUS "third_party:${third_party}")

if(xcl_lib_type STREQUAL "dynamic")
    add_definitions(-DDYNAMIC)
endif()

set(xcl_project_dir ${CMAKE_CURRENT_LIST_DIR}/..)
set(xcl_inc_dir ${xcl_project_dir}/include/public
                ${xcl_project_dir}/include/internal)
set(xcl_lib_dir ${xcl_project_dir}/lib/${xcl_lib_type})

if(COMPILE_X86_TARGET)
    set(xcl_lib_dir ${xcl_lib_dir}/x86)
else()
    set(xcl_lib_dir ${xcl_lib_dir}/x64)
endif()

set(xcl_lib_dir ${xcl_lib_dir}/${CMAKE_BUILD_TYPE})

message(STATUS ${xcl_inc_dir})
message(STATUS ${xcl_lib_dir})

link_directories(${xcl_lib_dir} ${third_party}/googletest/lib)
include_directories(${xcl_inc_dir} ${third_party}/googletest/include)

add_executable(unit_test main.cc ${test_src_files})

if(WIN32)
    set(link_libs ${xcl_lib_name} gtest gtest_main winmm)
else()
    set(link_libs ${xcl_lib_name} gtest gtest_main pthread)
endif()

if(NOT ENABLE_CXX_COMPILE)
    if(WIN32 AND CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(link_libs ${link_libs} atomic)
    endif()
endif()

message(STATUS ${link_libs})

target_link_libraries(unit_test ${link_libs})

if(MSVC)
    set_property(
        TARGET unit_test PROPERTY MSVC_RUNTIME_LIBRARY
                                  "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

if(WIN32)
    add_custom_command(
        TARGET unit_test
        PRE_BUILD
        COMMAND cd ${CMAKE_CURRENT_LIST_DIR} && del /q
                ".\\bin\\${CMAKE_BUILD_TYPE}\\*"
        COMMAND echo "clean run path:${run_path}")
else()
    add_custom_command(
        TARGET unit_test
        PRE_BUILD
        COMMAND rm -rf ${run_path}/*
        COMMAND echo "clean run path:${run_path}")
endif()

add_custom_command(
    TARGET unit_test
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${run_path}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${xcl_lib_dir} ${run_path}
    COMMAND ${CMAKE_COMMAND} -E echo "finish")
