cmake_minimum_required(VERSION 3.15)

project(UnitTest)

set(CMAKE_CXX_STANDARD 11)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/bin/${build_type})
file(GLOB test_src_files src/*.cc)

if (WIN32)
    set(third_party)
else ()
    set(third_party /usr/local)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(build_type debug)
    set(xcl_lib_name xcld)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(build_type release)
    set(xcl_lib_name xcl)
endif ()

set(xcl_lib_type dynamic)

set(xcl_project_dir ${CMAKE_CURRENT_LIST_DIR}/..)
set(xcl_inc_dir ${xcl_project_dir}/include/public ${xcl_project_dir}/include/internal)
set(xcl_lib_dir ${xcl_project_dir}/lib/${xcl_lib_type}/${build_type})

link_directories(${xcl_lib_dir} ${third_party}/googletest/lib)
include_directories(${xcl_inc_dir} ${third_party}/googletest/include)

add_executable(unit_test main.cc ${test_src_files})

if (WIN32)
    target_link_libraries(unit_test ${xcl_lib_name} gtest gtest_main winmm)
else ()
    target_link_libraries(unit_test ${xcl_lib_name} gtest gtest_main pthread)
endif ()

add_custom_command(TARGET unit_test PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${EXECUTABLE_OUTPUT_PATH}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${EXECUTABLE_OUTPUT_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${xcl_lib_dir} ${EXECUTABLE_OUTPUT_PATH}
        COMMAND ${CMAKE_COMMAND} -E echo "finish")