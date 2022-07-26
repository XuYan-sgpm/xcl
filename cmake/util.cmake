function(check_c_cxx_compiler)
    set(idx 0)
    while (idx LESS ${ARGC})
        list(APPEND valid_compilers ${ARGV${idx}})
        math(EXPR idx "${idx}+1")
    endwhile ()
    function(_check_compiler compiler)
        set(is_valid false)
        foreach (valid_compiler ${valid_compilers})
            if (${valid_compiler} STREQUAL ${compiler})
                set(is_valid true)
                break()
            endif ()
        endforeach ()
        if (NOT ${is_valid})
            message(FATAL_ERROR "invalid compiler")
        endif ()
    endfunction()
    _check_compiler(${CMAKE_CXX_COMPILER_ID})
    _check_compiler(${CMAKE_C_COMPILER_ID})
endfunction()

list(APPEND cxx_extensions .cc .cxx .cpp)

function(collect_source_files)
    function(_is_excluded path exclude)
        set(ret)
        string(SUBSTRING ${exclude} 0 1 front)
        string(LENGTH ${exclude} len)
        math(EXPR finish "${len}-1")
        string(SUBSTRING ${exclude} ${finish} 1 back)
        if (front STREQUAL "/")
            math(EXPR len "${len}-1")
            string(SUBSTRING ${exclude} 1 ${len} exclude)
        endif ()
        if (back STREQUAL "/")
            math(EXPR len "${len}-1")
            string(SUBSTRING ${exclude} 0 ${len} exclude)
        endif ()
        string(REGEX MATCH "/${exclude}$|/${exclude}/" ret ${path})
        if (ret STREQUAL "/${exclude}" OR ret STREQUAL "/${exclude}/")
            set(excluded true PARENT_SCOPE)
        else ()
            set(excluded false PARENT_SCOPE)
        endif ()
    endfunction()
    set(source_dir ${ARGV0})
    if (NOT IS_DIRECTORY ${source_dir})
        message(SEND_ERROR "not directory")
        return()
    endif ()
    set(idx 1)
    while (idx LESS ${ARGC})
        list(APPEND exclude_dirs ${ARGV${idx}})
        math(EXPR idx "${idx}+1")
    endwhile ()
    get_filename_component(source_dir_name ${source_dir} NAME)
    set(accept true)
    foreach (exclude ${exclude_dirs})
        set(excluded false)
        _is_excluded(${source_dir} ${exclude})
        if (${excluded})
            set(accept false)
            break()
        endif ()
    endforeach ()
    if (NOT ${accept})
        message(WARNING "ignore path ${source_dir}")
        return()
    endif ()
    file(GLOB current_files ${source_dir}/*)
    foreach (path ${current_files})
        if (NOT IS_DIRECTORY ${path})
            set(add_path true)
            if (ENABLE_CXX_COMPILE)
            else ()
                get_filename_component(file_last_ext ${path} LAST_EXT)
                if (NOT file_last_ext)
                    message(WARNING "${path} has no extension info, accepted")
                else ()
                    foreach (cxx_ext ${cxx_extensions})
                        if (file_last_ext STREQUAL "${cxx_ext}")
                            message(WARNING "ignore cxx file:${path}")
                            set(add_path false)
                            break()
                        endif ()
                    endforeach ()
                endif ()
            endif ()
            if (add_path)
                list(APPEND lib_src_files ${path})
            endif ()
        else ()
            collect_source_files(${path} ${exclude_dirs})
        endif ()
    endforeach ()
    set(lib_src_files ${lib_src_files} PARENT_SCOPE)
endfunction()