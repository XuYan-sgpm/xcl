project(xcl)

set(CMAKE_DEBUG_POSTFIX "_d")
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(include "C:/Program Files (x86)/googletest-distribution/include")
link_directories("C:/Program Files (x86)/googletest-distribution/lib")

file(GLOB_RECURSE src_files src/*.c src/*.cc)
add_library(xcl SHARED ${src_files})

add_executable(main main.cc)
target_link_libraries(main xcl gtest gtest_main)