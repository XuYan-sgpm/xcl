cmake_minimum_required(VERSION 3.15)

project(XclProject)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/bin)

option(copy_xcl_dynamic_lib "copy xcl dynamic library to executable path" ON)

set(CMAKE_DEBUG_POSTFIX "d")
set(xcl_lib_name xcl)

if (NOT DEFINED CMAKE_BUILD_TYPE)
    message(WARNING "cmake build type is not specified, auto set to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif ()

set(compile_static_lib false)

set(xcl_inc_dir ${CMAKE_CURRENT_LIST_DIR}/xcl/include ${CMAKE_CURRENT_LIST_DIR}/xcl/internal/inc)
set(xcl_lib_dir ${LIBRARY_OUTPUT_PATH})
set(xcl_lib_file_name)

add_subdirectory(xcl)
add_subdirectory(XclUnitTest)
