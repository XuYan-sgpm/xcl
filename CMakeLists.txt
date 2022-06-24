project(xcl)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_C_FLAGS "-g -Wall")
set(CMAKE_CXX_FLAGS "-g -Wall")
set(CMAKE_DEBUG_POSTFIX "_d")
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(COMPILE_STATIC true)

include_directories(include internal/inc "C:/Program Files (x86)/googletest-distribution/include")
link_directories("C:/Program Files (x86)/googletest-distribution/lib")

file(GLOB_RECURSE src_files src/*.c src/*.cc internal/src/*.c internal/src/*.cc)
if (COMPILE_STATIC)
    message(STATUS "STATIC")
    add_definitions(-DSTATIC)
    add_library(xcl STATIC ${src_files})
else ()
    message(STATUS "DYNAMIC")
    add_definitions(-DDYNAMIC)
    add_library(xcl SHARED ${src_files})
endif ()

file(GLOB_RECURSE test_files test/*.c test/*.cc)
add_executable(main main.cc ${test_files})
target_link_libraries(main xcl gtest gtest_main)