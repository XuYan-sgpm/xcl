cmake_minimum_required(VERSION 3.15)

project(UnitTest)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/bin/${build_type})
file(GLOB test_src_files src/*.cc)

if (WIN32)
    set(third_party)
else ()
    set(third_party /usr/local)
endif ()

link_directories(${xcl_lib_dir} ${third_party}/googletest/lib)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../${xcl_inc_dir} ${third_party}/googletest/include)

add_executable(unit_test main.cc ${test_src_files})

if (WIN32)
    target_link_libraries(unit_test gtest gtest_main winmm)
else ()
    target_link_libraries(unit_test gtest gtest_main pthread)
endif ()

add_custom_command(TARGET unit_test PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${EXECUTABLE_OUTPUT_PATH}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${EXECUTABLE_OUTPUT_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${xcl_lib_dir} ${EXECUTABLE_OUTPUT_PATH}
        COMMAND ${CMAKE_COMMAND} -E echo "finish")