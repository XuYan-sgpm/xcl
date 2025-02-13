message(STATUS "building xcl_test")
find_package(googletest REQUIRED)

function(collect_test_sources var)
    file(GLOB all_sources src/*.cc src/*.c src/*.cpp src/*.cxx)
    if(NOT ARGN)
        set(${var} ${all_sources} PARENT_SCOPE)
        return()
    endif()

    foreach(source ${all_sources})
        get_filename_component(test_name ${source} NAME_WE)
        if(test_name IN_LIST ARGN)
            list(APPEND source_list ${source})
        endif()
    endforeach()
    list(APPEND source_list ${CMAKE_CURRENT_LIST_DIR}/src/main.cc)
    set(${var} ${source_list} PARENT_SCOPE)
endfunction()

# collect_test_sources(test_sources test_path)
collect_test_sources(test_sources)
set(exclude_names test_os_wait test_once test_atomic test_log_writer
                  test_log_manager)
# if(NOT HAS_ATOMIC)
list(APPEND exclude_names test_concurrent_linked_queue
     test_concurrent_ring_queue test_atomic test_spin_lock)
# endif()
xcl_exclude_file(test_sources ${exclude_names})
foreach(source ${test_sources})
    message(STATUS "test source -> ${source}")
endforeach()
add_executable(xcl_test ${test_sources})
target_compile_definitions(xcl_test PRIVATE ${CMAKE_SYSTEM_NAME})
if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL Debug)
        include(${PROJECT_SOURCE_DIR}/cmake/vld.cmake)
        msvc_vld(xcl_test)
    endif()
    target_compile_options(xcl_test PRIVATE /std:c++14 /wd4244 /wd4018 /wd4267
                                            /utf-8)
else()
    target_compile_options(xcl_test PRIVATE -std=c++14)
endif()
target_link_libraries(xcl_test PRIVATE xcl googletest)
