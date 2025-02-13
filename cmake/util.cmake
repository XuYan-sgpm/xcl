include_guard(GLOBAL)
include(CheckSymbolExists)

# xcl_install_path(<source> <destination>)
function(xcl_install_path)
    if(NOT CMAKE_INSTALL_PREFIX)
        return()
    endif()
    if(NOT IS_DIRECTORY ${ARGV0})
        install(FILES ${ARGV0} DESTINATION ${ARGV1})
        return()
    endif()
    get_filename_component(dir_name ${ARGV0} NAME)
    file(GLOB sub_files ${ARGV0}/*)
    foreach(sub_file ${sub_files})
        xcl_install_path(${sub_file} ${ARGV1}/${dir_name})
    endforeach()
endfunction()

function(xcl_install target)
    xcl_install_path($<TARGET_FILE:${target}> ${CMAKE_INSTALL_PREFIX}/lib)
    xcl_install_path(${PROJECT_SOURCE_DIR}/include/cgf
                     ${CMAKE_INSTALL_PREFIX}/include)
endfunction()

# check_symbol(<target> <symbol> <headers>)
function(xcl_check_symbol target symbol)
    string(TOUPPER ${symbol} upper_symbol)
    set(macro_name HAS_${upper_symbol})
    check_symbol_exists(${symbol} "${ARGN}" ${symbol}_FOUND)
    set(${symbol}_FOUND ${${symbol}_FOUND} PARENT_SCOPE)
    if(${symbol}_FOUND)
        target_compile_definitions(${target} PRIVATE ${macro_name})
    endif()
endfunction()

function(xcl_exclude_file list)
    set(exclude_names ${ARGN})
    message(STATUS "exclude names -> ${exclude_names}")
    foreach(file ${${list}})
        get_filename_component(file_name ${file} NAME_WE)
        if(file_name IN_LIST exclude_names)
            message(STATUS "ignore file -> ${file}")
            list(REMOVE_ITEM ${list} ${file})
        endif()
    endforeach()
    set(${list} ${${list}} PARENT_SCOPE)
endfunction()

function(xcl_filter_sources var)
    set(list1 ${${var}})
    set(sources)
    foreach(source ${list1})
        get_filename_component(source_name ${source} NAME_WE)
        if(source_name IN_LIST ARGN)
            list(APPEND sources ${source})
            message(STATUS "match source ${source_name} -> ${source}")
        endif()
    endforeach()
    set(${var} ${sources} PARENT_SCOPE)
endfunction()

function(xcl_check_atomic)
    set(sources ${PROJECT_SOURCE_DIR}/src/common/tests/test_have_atomic.c)
    if(WIN32)
        list(APPEND sources ${PROJECT_SOURCE_DIR}/src/win32/concurrent/atomic.c
             ${PROJECT_SOURCE_DIR}/src/common/lang/int128.c)
    else()
        list(APPEND sources ${PROJECT_SOURCE_DIR}/src/unix/concurrent/atomic.c)
    endif()
    if(CMAKE_C_COMPILER_ID STREQUAL GNU)
        set(atomic_link_libs atomic)
    endif()
    try_compile(
        result ${CMAKE_CURRENT_BINARY_DIR} SOURCES ${sources}
        CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${PROJECT_SOURCE_DIR}/include"
        LINK_LIBRARIES ${atomic_link_libs} OUTPUT_VARIABLE var)
    if(result)
        message(STATUS "atomic feature supported")
        target_compile_definitions(${target} PUBLIC HAS_ATOMIC)
    else()
        message(STATUS "atomic feature not supported:\n${var}")
    endif()
    set(HAS_ATOMIC ${result} CACHE INTERNAL "enable atomic feature")
endfunction()

function(xcl_sources var)
    file(GLOB_RECURSE sources src/common/*.c src/common/*.cpp)
    file(GLOB test_files src/common/tests/*.c src/common/tests/*.cpp)
    list(REMOVE_ITEM sources ${test_files})
    if(WIN32)
        file(GLOB_RECURSE win32_files src/win32/*.c)
        list(APPEND sources ${win32_files})
    else()
        file(GLOB_RECURSE unix_files src/unix/*.c)
        list(APPEND sources ${unix_files})
    endif()
    set(args ${ARGN})
    if(args)
        list(GET args 0 first_arg)
        if(first_arg STREQUAL "EXCLUDE")
            list(REMOVE_AT args 0)
            xcl_exclude_file(sources ${args})
        else()
            xcl_filter_sources(sources ${args})
        endif()
    endif()
    set(${var} ${sources} PARENT_SCOPE)
endfunction()

function(add_xcl target)
    xcl_sources(sources ${ARGN})
    add_library(${target} ${sources})
endfunction()
