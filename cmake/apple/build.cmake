# if(COMPILE_X86_TARGET) message(WARNING "x86 build is deprecated on macosx,
# turn off") set(COMPILE_X86_TARGET OFF) endif() set(architecture x64)
set(LIBRARY_OUTPUT_PATH ${project_dir}/lib/${BUILD_LIB_TYPE})

list(APPEND exclude_dirs windows win32 linux)
set(link_libs pthread)
make_xcl_lib()
