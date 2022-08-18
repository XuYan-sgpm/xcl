from genericpath import isdir
import os
import sys


def install_header(src_dir: str, destination: str):
    names = os.listdir(src_dir)
    for name in names:
        abs_path = os.path.join(src_dir, name)
        if os.path.isdir(abs_path):
            dest_dir = os.path.join(destination, name)
            if not os.path.exists(dest_dir):
                os.makedirs(dest_dir)
            install_header(abs_path, dest_dir)
            continue
        cmd = 'copy /y {} {} >nul'.format(abs_path, destination)
        print('installing:', os.path.join(destination, name))
        # print(cmd)
        os.system(cmd)


def install_library(src_dir, lib_type, build_type, architecture, destination):
    dest_full_dir = os.path.join(destination, lib_type, architecture,
                                 build_type)
    if not os.path.exists(dest_full_dir):
        os.makedirs(dest_full_dir)
    names = os.listdir(src_dir)
    for name in names:
        abs_path = os.path.join(src_dir, name)
        cmd = 'copy /y {} {} >nul'.format(abs_path, dest_full_dir)
        print('installing:', os.path.join(dest_full_dir, name))
        # print(cmd)
        os.system(cmd)


if __name__ == '__main__':
    install_prefix = sys.argv[1]
    build_type = sys.argv[2]
    architecture = sys.argv[3]
    lib_type = sys.argv[4]
    cwd = sys.argv[5]
    cwd = cwd.replace('/', os.sep)
    install_prefix = install_prefix.replace('/', os.sep)
    print('install prefix:', install_prefix)
    print('build type:', build_type)
    print('architecture:', architecture)
    print('lib type:', lib_type)
    print('cwd:', cwd)
    if not os.path.exists(install_prefix):
        os.makedirs(install_prefix)
    origin_lib_dir = os.path.join(cwd, 'lib', lib_type, architecture,
                                  build_type)
    print('original lib dir:', origin_lib_dir)
    inc_dir = os.path.join(cwd, 'include')
    install_header(inc_dir, os.path.join(install_prefix, 'include', 'xcl'))
    install_library(origin_lib_dir, lib_type, build_type, architecture,
                    os.path.join(install_prefix, 'lib'))
