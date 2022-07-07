cmake_minimum_required(VERSION 3.15)

project(XclUnitTest)

set(CMAKE_CXX_STANDARD 11)

message(STATUS ${xcl_inc_dir})
message(STATUS ${xcl_lib_dir})

if (WIN32)
    message(STATUS "win32")
    include_directories(${xcl_inc_dir} D:\\third_party\\googletest\\include)
    link_directories(${xcl_lib_dir} D:\\third_party\\googletest\\lib)
else ()
    if (APPLE)
        message(STATUS "macos")
    elseif (UNIX)
        message(STATUS "unix")
    endif ()
    include_directories(${xcl_inc_dir} /usr/local/googletest/include)
    link_directories(${xcl_lib_dir} /usr/local/googletest/lib)
endif ()

set(test_files src/TestLock.cc)
add_executable(main src/main.cc ${test_files})
target_link_libraries(main xcl gtest gtest_main)
