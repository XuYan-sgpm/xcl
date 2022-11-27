cmake_minimum_required(VERSION 3.15)

project(UnitTest)

set(CMAKE_VERBOSE_MAKEFILE ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(NOT xcl_build_type)
    set(xcl_build_type Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(build_type debug)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(build_type release)
endif()

if(xcl_build_type STREQUAL "Debug")
    set(xcl_lib_name xcld)
else()
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
# set(test_src_files src/test_bits.cc)

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

if(xcl_lib_type STREQUAL "SHARED")
    add_definitions(-DSHARED)
else()
    add_definitions(-DSTATIC)
endif()

set(xcl_project_dir ${CMAKE_CURRENT_LIST_DIR}/..)
set(xcl_inc_dir ${xcl_project_dir}/inc_pri ${xcl_project_dir}/inc_pub)
set(xcl_lib_dir ${xcl_project_dir}/lib/${xcl_lib_type})
set(xcl_lib_dir ${xcl_lib_dir}/${xcl_build_type})

message(STATUS ${xcl_inc_dir})
message(STATUS ${xcl_lib_dir})

set(third_party d:/libraries/${compiler_id})

link_directories(${xcl_lib_dir} ${third_party}/dynamic/x86/googletest/lib)
include_directories(${xcl_inc_dir}
                    ${third_party}/dynamic/x86/googletest/include)

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
    if(xcl_lib_type STREQUAL "static")
        set_property(
            TARGET unit_test PROPERTY MSVC_RUNTIME_LIBRARY
                                      "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        set_property(
            TARGET unit_test
            PROPERTY MSVC_RUNTIME_LIBRARY
                     "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    endif()

endif()

add_custom_command(
    TARGET unit_test
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${run_path}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${xcl_lib_dir} ${run_path}
    COMMAND ${CMAKE_COMMAND} -E echo "finish")
