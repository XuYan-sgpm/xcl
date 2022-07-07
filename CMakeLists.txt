cmake_minimum_required(VERSION 3.15)

project(XclProject)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/bin)

set(CMAKE_DEBUG_POSTFIX "d")
set(xcl_lib_name xcl)

if (NOT DEFINED CMAKE_BUILD_TYPE)
    message(WARNING "cmake build type is not specified, auto set to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif ()

set(COMPILE_STATIC false)

set(xcl_inc_dir ${CMAKE_CURRENT_LIST_DIR}/xcl/include ${CMAKE_CURRENT_LIST_DIR}/xcl/internal/inc)
set(xcl_lib_dir ${LIBRARY_OUTPUT_PATH})

add_subdirectory(xcl)
add_subdirectory(XclUnitTest)
