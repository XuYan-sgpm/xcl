cmake_minimum_required(VERSION 3.20)
project(xcl)

set(CMAKE_CXX_STANDARD 17)
Set(CMAKE_CXX_FLAGS -Wall)

include_directories(include)

file(GLOB_RECURSE ALL_SRCS src/*.cc src/*.cpp)
add_library(xcl_shared SHARED ${ALL_SRCS})
add_library(xcl_static STATIC ${ALL_SRCS})

add_executable(test_thread test/test_thread.cc)
target_link_libraries(test_thread xcl_shared)

add_executable(test_recycler test/test_recycler.cc)
target_link_libraries(test_recycler xcl_shared)
