include_guard(GLOBAL)

if(TARGET googletest)
    return()
endif()

if(EXISTS ${GTEST_PACKAGE})
    add_library(googletest INTERFACE)
    target_include_directories(googletest INTERFACE ${GTEST_PACKAGE}/include)
    find_library(gtest_lib_path NAMES gtest HINTS ${GTEST_PACKAGE}/lib)
    find_library(gtest_main_lib_path NAMES gtest_main
                 HINTS ${GTEST_PACKAGE}/lib)
    message(STATUS "Found gtest: ${gtest_lib_path}, ${gtest_main_lib_path}")
    target_link_libraries(googletest INTERFACE ${gtest_lib_path}
                                               ${gtest_main_lib_path})
    unset(gtest_lib_path)
    unset(gtest_main_lib_path)
elseif(googletest_FIND_REQUIRED)
    message(FATAL_ERROR "no googletest found")
endif()
