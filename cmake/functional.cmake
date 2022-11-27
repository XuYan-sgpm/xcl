function(check_c_cxx_compiler)
    set(idx 0)
    while(idx LESS ${ARGC})
        list(APPEND valid_compilers ${ARGV${idx}})
        math(EXPR idx "${idx}+1")
    endwhile()
    function(_check_compiler compiler)
        set(is_valid FALSE)
        foreach(valid_compiler ${valid_compilers})
            if(${valid_compiler} STREQUAL ${compiler})
                set(is_valid TRUE)
                break()
            endif()
        endforeach()
        if(NOT is_valid)
            message(FATAL_ERROR "invalid compiler")
        endif()
    endfunction()
    _check_compiler(${CMAKE_CXX_COMPILER_ID})
    _check_compiler(${CMAKE_C_COMPILER_ID})
endfunction()

list(
    APPEND
    cxx_extensions
    .cc
    .cxx
    .cpp
    .hpp
    .hxx)
list(APPEND c_extensions .c .h)

function(compile_library2)
    if(${ARGC} LESS 3)
        message(FATAL_ERROR "too less paramters")
        return()
    endif()
    set(lib_src_files)
    set(lib_name ${ARGV0})
    set(lib_type ${ARGV1})
    set(idx 2)
    set(enable_cxx 0)
    if(${ARGV2} STREQUAL "ENABLE_CXX")
        set(enable_cxx 1)
        math(EXPR idx "${idx}+1")
    endif()
    set(compile_files)
    set(exclude_files)
    set(is_source 1)
    while(idx LESS ${ARGC})
        set(param ${ARGV${idx}})
        math(EXPR idx "${idx}+1")
        if(param STREQUAL "EXCLUDE")
            set(is_source 0)
            continue()
        endif()
        if(is_source)
            list(APPEND compile_files ${param})
        else()
            list(APPEND exclude_files ${param})
        endif()
    endwhile()
    while(compile_files)
        list(POP_FRONT compile_files current_file)
        set(filtered 0)
        if(NOT IS_DIRECTORY ${current_file})
            get_filename_component(ext ${current_file} LAST_EXT)
            if(NOT enable_cxx)
                foreach(cxx_ext ${cxx_extensions})
                    if(cxx_ext STREQUAL ${ext})
                        message(WARNING "ignore file:${current_file}")
                        set(filtered 1)
                        break()
                    endif()
                endforeach()
            endif()
            if(NOT filtered)
                message(STATUS "add source file:${current_file}")
                list(APPEND lib_src_files ${current_file})
            endif()
            continue()
        endif()
        foreach(exclude ${exclude_files})
            string(REGEX MATCH "/${exclude}$|/${exclude}/" matched
                         ${current_file})
            if(matched STREQUAL "/${exclude}" OR matched STREQUAL
                                                 "/${exclude}/")
                set(filtered 1)
                break()
            endif()
        endforeach()
        if(filtered)
            message(WARNING "ignore directory ${current_file}")
            continue()
        endif()
        file(GLOB sub_files ${current_file}/*)
        list(APPEND compile_files ${sub_files})
    endwhile()
    add_library(${lib_name} ${lib_type} ${lib_src_files})
endfunction()
