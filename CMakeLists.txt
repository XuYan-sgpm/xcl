project(xcl)

set(CMAKE_DEBUG_POSTFIX "_d")
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

option(COMPILE_STATIC "compile static library" ON)

include_directories(include internal/inc "C:/Program Files (x86)/googletest-distribution/include")
link_directories("C:/Program Files (x86)/googletest-distribution/lib")

file(GLOB_RECURSE src_files src/*.c src/*.cc internal/src/*.c internal/src/*.cc)
if (COMPILE_STATIC)
    message(STATUS "STATIC")
    add_definitions(-DSTATIC_LIB)
    add_library(xcl STATIC ${src_files})
else ()
    add_definitions(-DDYNAMIC_LIB)
    add_library(xcl SHARED ${src_files})
endif ()

file(GLOB_RECURSE test_files test/*.c test/*.cc)
add_executable(main main.cc ${test_files})
target_link_libraries(main xcl gtest gtest_main)