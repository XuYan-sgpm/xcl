cmake_minimum_required(VERSION 3.15)

project(xcl C CXX)

set(project_dir ${CMAKE_CURRENT_LIST_DIR})

include_directories(inc_pub inc_pri inc_pub/xcl inc_pri/xcl)

include(cmake/common.cmake)
set(xcl_lib_name xcl)
set(exclude_dirs)
set(link_libs)

message(STATUS "build type:${CMAKE_BUILD_TYPE}")
function(make_xcl_lib)
    set(opt_str)
    if(ENABLE_CXX_COMPILE)
        set(opt_str ENABLE_CXX)
    endif()
    compile_library2(
        ${xcl_lib_name}
        ${BUILD_LIB_TYPE}
        ${opt_str}
        ${project_dir}/src
        ${project_dir}/inc_pub
        ${project_dir}/inc_pri
        EXCLUDE
        ${exclude_dirs})
    target_link_libraries(${xcl_lib_name} ${link_libs})
endfunction()

if(WIN32)
    set(platform win32)
elseif(APPLE)
    set(platform apple)
elseif(UNIX)
    set(platform unix)
endif()

include(cmake/${platform}/build.cmake)
message(STATUS ${LIBRARY_OUTPUT_PATH})

add_custom_command(
    TARGET ${xcl_lib_name}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LIBRARY_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E echo
            "make library directories ${LIBRARY_OUTPUT_PATH}"
    VERBATIM)

add_custom_command(
    TARGET ${xcl_lib_name}
    POST_BUILD
    COMMAND echo "xcl lib path:$<TARGET_FILE:${xcl_lib_name}>")

install(
    CODE "execute_process(COMMAND python -u ${CMAKE_CURRENT_LIST_DIR}/install.py ${CMAKE_INSTALL_PREFIX} ${CMAKE_BUILD_TYPE} ${architecture} ${BUILD_LIB_TYPE} ${CMAKE_CURRENT_LIST_DIR})"
)
