include_guard(GLOBAL)

function(vld_custom target)
    get_target_property(type ${target} TYPE)
    if(NOT type STREQUAL EXECUTABLE)
        message(FATAL_ERROR "vld only used for executable")
    endif()

    if(NOT TARGET libvld)
        message(FATAL_ERROR "libvld not found")
    endif()

    get_target_property(vld_ini libvld INTERFACE_VLD_INI)
    get_filename_component(vld_ini_name ${vld_ini} NAME)
    get_target_property(vld_bin_dir libvld INTERFACE_VLD_BINARY_DIR)
    get_target_property(target_bin_dir ${target} BINARY_DIR)
    add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${vld_bin_dir}
                $<TARGET_FILE_DIR:${target}> VERBATIM)
    if(NOT EXISTS ${target_bin_dir}/${CMAKE_BUILD_TYPE}/${vld_ini_name})
        add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${vld_ini}
                    $<TARGET_FILE_DIR:${target}> VERBATIM)
    else()
        message(STATUS "vld.ini exists")
    endif()
endfunction()

function(make_libvld)
    set(vld_inc_dir ${VLD_PREFIX}/include)
    if(CMAKE_SIZEOF_VOID_P STREQUAL 4)
        set(vld_arch_dir Win32)
        set(vld_arch x86)
    else()
        set(vld_arch_dir Win64)
        set(vld_arch x64)
    endif()

    set(vld_bin_dir ${VLD_PREFIX}/bin/${vld_arch_dir})
    set(vld_lib ${VLD_PREFIX}/lib/${vld_arch_dir}/vld.lib)
    set(vld_dll ${VLD_PREFIX}/bin/${vld_arch_dir}/vld_${vld_arch}.dll)
    set(vld_ini ${VLD_PREFIX}/vld.ini)
    set(vld_req_paths ${vld_lib} ${vld_dll} ${vld_inc_dir} ${vld_ini})

    foreach(path ${vld_req_paths})
        if(NOT EXISTS ${path})
            message(FATAL_ERROR "missing ${path}, broken vld package")
        endif()
    endforeach()

    message(STATUS "find vld include directory:${vld_inc_dir}")
    message(STATUS "find vld target:${vld_dll}")
    add_library(libvld INTERFACE)
    target_link_libraries(libvld
                          INTERFACE ${VLD_PREFIX}/lib/${vld_arch_dir}/vld.lib)
    set_target_properties(
        libvld PROPERTIES INTERFACE_VLD_INI ${vld_ini} INTERFACE_VLD_BINARY_DIR
                                                       ${vld_bin_dir})
    target_include_directories(libvld INTERFACE ${vld_inc_dir})
endfunction()

function(msvc_vld target)
    if(NOT WIN32)
        message(FATAL_ERROR "vld only support on win32")
    endif()

    if(NOT TARGET libvld)
        make_libvld()
    endif()

    target_compile_definitions(${target} PRIVATE VLD_ENABLE)
    target_link_libraries(${target} PRIVATE libvld)

    vld_custom(${target})
endfunction()
