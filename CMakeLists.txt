cmake_minimum_required(VERSION 3.15)

project(xcl C CXX)

set(project_dir ${CMAKE_CURRENT_LIST_DIR})

include_directories(include include/xcl)

include(cmake/common.cmake)
set(xcl_lib_name xcl)
set(exclude_dirs)
set(link_libs)

function(make_xcl_lib)
    compile_library(${xcl_lib_name} ${BUILD_LIB_TYPE} ${ENABLE_CXX_COMPILE}
                    ${project_dir}/src ${exclude_dirs})
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

add_custom_command(
    TARGET ${xcl_lib_name}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${LIBRARY_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${LIBRARY_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} -E echo
            "make library directories ${LIBRARY_OUTPUT_PATH}"
    VERBATIM)

install(
    CODE "execute_process(COMMAND python -u ${CMAKE_CURRENT_LIST_DIR}/install.py ${CMAKE_INSTALL_PREFIX} ${CMAKE_BUILD_TYPE} ${architecture} ${BUILD_LIB_TYPE} ${CMAKE_CURRENT_LIST_DIR})"
)
