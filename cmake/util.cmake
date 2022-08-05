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

list(APPEND cxx_extensions .cc .cxx .cpp)
list(APPEND c_extensions .c)

set(LIBRARY_SOURCE_FILES)

function(collect_source_files)
  set(enable_cxx_compile ${ARGV0})
  message(STATUS "enable cxx:${enable_cxx_compile}")
  set(source_dirs)
  list(APPEND source_dirs ${ARGV1})
  set(paramIdx 2)
  while(paramIdx LESS ${ARGC})
    set(exclude ${ARGV${paramIdx}})
    set(matched)
    string(REGEX MATCH "[^/]+(/[^/]+)*" matched ${exclude})
    if(NOT matched STREQUAL exclude)
      message(STATUS "exclude changed:${exclude}->${matched}")
      set(exclude ${matched})
    endif()
    list(APPEND exclude_dirs ${exclude})
    math(EXPR paramIdx "${paramIdx}+1")
  endwhile()
  while(source_dirs)
    list(POP_FRONT source_dirs current_dir)
    if(NOT IS_DIRECTORY ${current_dir})
      message(SEND_ERROR "not directory")
      continue()
    endif()
    set(accept TRUE)
    foreach(exclude ${exclude_dirs})
      string(REGEX MATCH "/${exclude}$|/${exclude}/" matched ${current_dir})
      if(matched STREQUAL "/${exclude}" OR matched STREQUAL "/${exclude}/")
        set(accept FALSE)
        break()
      endif()
    endforeach()
    if(NOT accept)
      message(WARNING "ignore directory: ${current_dir}")
      continue()
    endif()
    file(GLOB current_files ${current_dir}/*)
    foreach(path ${current_files})
      if(IS_DIRECTORY ${path})
        list(APPEND source_dirs ${path})
        continue()
      endif()
      set(add_path FALSE)
      get_filename_component(file_last_ext ${path} LAST_EXT)
      foreach(c_ext ${c_extensions})
        if(c_ext STREQUAL "${file_last_ext}")
          set(add_path TRUE)
          break()
        endif()
      endforeach()
      if(NOT add_path AND enable_cxx_compile)
        foreach(cxx_ext ${cxx_extensions})
          if(file_last_ext STREQUAL "${cxx_ext}")
            set(add_path TRUE)
            break()
          endif()
        endforeach()
      endif()
      if(add_path)
        list(APPEND LIBRARY_SOURCE_FILES ${path})
      else()
        message(WARNING "ignore file: ${path}")
      endif()
    endforeach()
  endwhile()
  set(LIBRARY_SOURCE_FILES
      ${LIBRARY_SOURCE_FILES}
      PARENT_SCOPE)
endfunction()
