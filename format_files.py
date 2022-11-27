
import os

__EXCLUDES__ = ['bin', 'build', 'lib', '.vscode', '.git']
__CXX__ = ['.cc', '.cpp', '.cxx', '.c', '.h', '.hpp', '.hxx']
__CMAKE_FORMAT_SETTINGS__ = '.cmake-format.py'


def get_cmake_format_settings():
    global __CMAKE_FORMAT_SETTINGS__
    settingsPath = os.path.join('.', __CMAKE_FORMAT_SETTINGS__)
    if os.path.exists(settingsPath):
        return
    settingsPath = os.path.join('.', '.cmake-format.json')
    if os.path.exists(settingsPath):
        __CMAKE_FORMAT_SETTINGS__ = '.cmake-format.json'
    else:
        print('cmake settings file not found')
        __CMAKE_FORMAT_SETTINGS__ = None
    print(__CMAKE_FORMAT_SETTINGS__)


def is_excluded(folder_name):
    global __EXCLUDES__
    for exclude in __EXCLUDES__:
        if exclude == folder_name:
            return 1
    return 0


def handle_file_type(file_name: str):
    global __CXX__
    dot = file_name.rfind('.')
    if dot == -1:
        return 0
    fmt = file_name[dot:]
    for cxx in __CXX__:
        if fmt == cxx:
            return 1
    if file_name == 'CMakeLists.txt':
        return 2
    if fmt == '.cmake':
        return 2
    return 0


def format_files(folder):
    if not os.path.exists(folder):
        print('path {} is not exists'.format(folder))
        return
    names = os.listdir(folder)
    for name in names:
        absPath = os.path.join(folder, name)
        if os.path.isdir(absPath):
            if is_excluded(name):
                print('ignore folder:{}'.format(absPath))
            else:
                format_files(absPath)
            continue
        code = handle_file_type(name)
        if code == 0:
            continue
        cmd = None
        if code == 1:
            cmd = 'clang-format -style=file -i {}'.format(absPath)
        elif code == 2:
            global __CMAKE_FORMAT_SETTINGS__
            if __CMAKE_FORMAT_SETTINGS__:
                cmd = 'cmake-format -c ./{} -i {}'.format(
                    __CMAKE_FORMAT_SETTINGS__, absPath)
            else:
                cmd = 'cmake-format -i {}'.format(absPath)
        os.system(cmd)
        print(cmd)


if __name__ == '__main__':
    get_cmake_format_settings()
    format_files('.')