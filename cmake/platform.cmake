include_guard(GLOBAL)
include(CheckTypeSize)

function(xcl_on_msvc)
    if(NOT HAS_ATOMIC)
        message(WARNING "only support win32 platforms which support atomic")
    endif()
    target_link_libraries(${target} PUBLIC winmm ntdll ws2_32)
    target_compile_options(
        ${target} PRIVATE /wd5105 /wd4113 /wd4244 /wd4996 /utf-8 /wd4018
                          /wd4267 /we4013 /volatile:iso)
endfunction()

function(xcl_on_linux)
    target_compile_options(${target} PUBLIC -fPIC)
    if(CMAKE_C_COMPILER_ID STREQUAL GNU)
        target_link_libraries(${target} PUBLIC atomic)
    endif()
endfunction()

function(xcl_on_darwin)

endfunction()

function(xcl_on_ios)

endfunction()

function(xcl_on_apple)
    if(IOS)
        xcl_on_ios()
    elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
        xcl_on_darwin()
    else()
        message(FATAL_ERROR "only support darwin or ios")
    endif()
endfunction()

function(xcl_on_android)
    target_link_libraries(${target} PUBLIC log)
endfunction()

function(xcl_on_unix)
    check_type_size("pthread_t" sizeof_pthread)
    message(STATUS "pthread_t -> ${sizeof_pthread}")
    target_compile_options(
        ${target}
        PUBLIC $<$<COMPILE_LANGUAGE:C>:-Werror=implicit-function-declaration>)
    if(NOT ANDROID)
        target_link_libraries(${target} PUBLIC pthread)
    endif()
    target_compile_definitions(
        ${target} PRIVATE _GNU_SOURCE SIZEOF_PTHREAD=${sizeof_pthread})
    target_link_libraries(${target} PUBLIC dl)
    xcl_check_symbol(${target} sendfile "<sys/sendfile.h>")
    if(ANDROID)
        xcl_on_android()
    elseif(APPLE)
        xcl_on_apple()
    else()
        xcl_on_linux()
    endif()
endfunction()

function(xcl_build target)
    set(AVAIL_PLATFORMS Windows Linux Darwin Android iOS)
    if(NOT CMAKE_SYSTEM_NAME IN_LIST AVAIL_PLATFORMS)
        message(FATAL_ERROR "platform ${CMAKE_SYSTEM_NAME} not support")
    endif()
    xcl_check_atomic()
    if(MSVC)
        xcl_on_msvc()
    elseif(WIN32)
        message(FATAL_ERROR "only support msvc on win32 platform")
    else()
        xcl_on_unix()
    endif()
    target_compile_definitions(
        ${target} PRIVATE STATIC ${CMAKE_SYSTEM_NAME} _${CMAKE_BUILD_TYPE}
                          ${CMAKE_C_COMPILER_ID})
    target_compile_definitions(
        ${target} PUBLIC __xcl_std__=${SDK_STD}
                         __xcl_version__="${SDK_VERSION}")
    target_include_directories(${target} PUBLIC include include/xcl)
endfunction()
