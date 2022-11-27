include(cmake/functional.cmake)
check_c_cxx_compiler("MSVC" "Clang" "GNU" "AppleClang")

if(NOT DEFINED ENABLE_CXX_COMPILE)
    message(STATUS "ENABLE_CXX_COMPILE not passing")
    option(ENABLE_CXX_COMPILE "enable cxx compile" OFF)
endif()
if(NOT DEFINED BUILD_DYNAMIC_LIBRARY)
    message(STATUS "BUILD_DYNAMIC_LIBRARY not passing")
    option(BUILD_DYNAMIC_LIBRARY "build dynamic library" ON)
endif()
if(NOT DEFINED ENABLE_C11)
    message(STATUS "ENABLE_C11 not passing")
    option(ENABLE_C11 "enable c11 standard" OFF)
endif()
if(NOT DEFINED ENABLE_CXX11)
    message(STATUS "ENABLE_CXX11 not passing")
    option(ENABLE_CXX11 "enable cxx11 standard" ON)
endif()
# if(NOT DEFINED COMPILE_X86_TARGET) message(STATUS "COMPILE_X86_TARGET not
# passing") option(COMPILE_X86_TARGET "compile x86 target" OFF) endif()
if(NOT DEFINED UNICODE)
    message(STATUS "UNICODE not passing")
    option(UNICODE "using unicode for path" OFF)
endif()

if(UNICODE)
    add_definitions(-DUNICODE -D_UNICODE)
    message(STATUS "enable unicode")
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)

if(ENABLE_C11)
    set(CMAKE_C_STANDARD 11)
    add_definitions(-DENABLE_C11)
else()
    set(CMAKE_C_STANDARD 99)
endif()
if(ENABLE_CXX11)
    set(CMAKE_CXX_STANDARD 11)
    add_definitions(-DENABLE_CXX11)
else()
    set(CMAKE_CXX_STANDARD 98)
endif()

message(STATUS "CMAKE_C_STANDARD:${CMAKE_C_STANDARD}")
message(STATUS "CMAKE_CXX_STANDARD:${CMAKE_CXX_STANDARD}")

if(ENABLE_CXX_COMPILE)
    add_definitions(-DENABLE_CXX)
    message(STATUS "enable cxx compile")
endif()

set(CMAKE_DEBUG_POSTFIX "d")

if(NOT DEFINED CMAKE_BUILD_TYPE)
    message(WARNING "cmake build type is not specified, auto set to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(BUILD_DYNAMIC_LIBRARY)
    set(BUILD_LIB_TYPE "SHARED")
else()
    set(BUILD_LIB_TYPE "STATIC")
endif()

if(BUILD_LIB_TYPE STREQUAL "STATIC")
    add_definitions(-DSTATIC)
    set(COMPILE_STATIC_LIB true)
elseif(BUILD_LIB_TYPE STREQUAL "SHARED")
    add_definitions(-DSHARED)
    set(COMPILE_STATIC_LIB false)
else()
    message(FATAL_ERROR "invalid library type:${BUILD_LIB_TYPE}")
endif()

add_definitions(-DBUILD_LIBRARY)
set(LIBRARY_OUTPUT_PATH ${project_dir}/lib/${BUILD_LIB_TYPE})
