cmake_minimum_required(VERSION 3.15)

project(XclUnitTest)

set(CMAKE_CXX_STANDARD 11)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    include_directories(${xcl_inc_dir} D:\\third_party\\googletest\\clang\\${build_type}\\include)
    link_directories(${xcl_lib_dir} D:\\third_party\\googletest\\clang\\${build_type}\\lib)
else ()
    include_directories(${xcl_inc_dir} /usr/local/googletest/include)
    link_directories(${xcl_lib_dir} /usr/local/googletest/lib)
endif ()

file(GLOB test_files src/*.cc)
#set(test_files src/TestThreadLocal.cc)
add_executable(xcl_unit_test src/main.cc ${test_files})
target_link_libraries(xcl_unit_test xcl gtest gtest_main)
