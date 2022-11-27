import os
import time

__WORDS__ = [
    'inter', 'xcl', 'global', 'ini', 'cond', 'mutex', 'cfg', 'linked',
    'blocking', 'queue', 'thread', 'local', 'atomic', 'storage', 'def', 'err',
    'size', 'class', 'ring', 'buffer', 'single', 'list', 'string', 'algo',
    'bits', 'blocker', 'bytes', 'file', 'rb', 'tree', 'lock', 'pool', 'str',
    'system', 'parse', 'api', 'id', 'platform'
]

__NAME_MAP__ = {}


def get_cxx_file_name(name: str):
    dot = name.find('.')
    if dot == -1:
        return name
    return name[:dot]


def check_file_name(name: str):
    dot = name.rfind('.')
    s = name if dot == -1 else name[:dot]
    cursor = 0
    global __WORDS__
    while cursor < len(s):
        found = False
        for word in __WORDS__:
            if s[cursor].lower() != word[0] or len(s) - cursor < len(word):
                continue
            off = 0
            if s[cursor].isupper():
                off += 1
            for i in range(off, len(word)):
                if word[i] != s[cursor + i]:
                    break
            else:
                found = True
            if found:
                if off == 0:
                    s = s[:cursor] + s[cursor].upper() + s[cursor + 1:]
                cursor += len(word)
                break
        if not found:
            raise RuntimeError('word recognized:', s[cursor:], s, name, cursor)
    check_name = s + (name[dot:] if dot != -1 else '')
    global __NAME_MAP__
    __NAME_MAP__[check_name.lower()] = check_name
    return check_name


def traverse(folder: str):
    if not os.path.isdir(folder):
        raise ValueError('passing path is not directory')
    names = os.listdir(folder)
    for name in names:
        abs_path = os.path.join(folder, name)
        if os.path.isdir(abs_path):
            yield from traverse(abs_path)
        else:
            yield folder, name


def rename_files_in_folder(path: str):
    for folder, name in traverse(path):
        old = os.path.join(folder, name)
        new = os.path.join(folder, check_file_name(name))
        print('rename:{}->{}'.format(old, new))
        os.rename(old, new)


def handle_include_line(line: str):
    beg = -1
    end = -1
    for beg_tag in ['"', '<']:
        beg = line.find(beg_tag)
        if beg != -1:
            break
    for end_tag in ['"', '>']:
        end = line.rfind(end_tag)
        if end != -1:
            break
    if beg == -1 or end == -1 or end - beg <= 1:
        raise RuntimeError('invalid include:', line)
    header = line[beg + 1:end].strip()
    sep = header.rfind('/')
    if sep != -1:
        header_name = header[sep + 1:]
    else:
        header_name = header
    for key, value in __NAME_MAP__.items():
        if header_name == key:
            header = header[:sep + 1] + value + header[sep + 1 + len(key):]
            return line[:beg + 1] + header + line[end:]
    return ''


def _get_tmp_name(name: str):
    dot = name.rfind('.')
    if dot == -1:
        return name + '~'
    else:
        return name[:dot] + '~' + name[dot:]


def _handle_one_file(folder: str, name: str):
    abs_path = os.path.join(folder, name)
    tmp_name = _get_tmp_name(name)
    with open(abs_path, "r+") as file:
        with open(os.path.join(folder, tmp_name), 'w') as tmp_file:
            for _, line in enumerate(file):
                result = ''
                line1 = line.strip()
                if line1.startswith("#include"):
                    result = handle_include_line(line1)
                if result:
                    tmp_file.write(result + '\n')
                else:
                    tmp_file.write(line)
    os.system('del /f {}'.format(abs_path))
    os.system('ren {} {}'.format(os.path.join(folder, tmp_name), name))


def modify_include_in_folder(path: str):
    for folder, name in traverse(path):
        _handle_one_file(folder, name)


def _clean_tmp_files(path: str):
    for folder, name in traverse(path):
        dot = name.rfind('.')
        if dot == -1:
            if name[-1] == '~':
                os.remove(os.path.join(folder, name))
        else:
            if name[:dot][-1] == '~':
                os.remove(os.path.join(folder, name))


if __name__ == '__main__':
    _clean_tmp_files(os.path.join('.', 'include'))
    _clean_tmp_files(os.path.join('.', 'src'))
    _clean_tmp_files(os.path.join('.', 'UnitTest', 'src'))
    rename_files_in_folder(os.path.join('.', 'include'))
    rename_files_in_folder(os.path.join('.', 'src'))
    modify_include_in_folder(os.path.join('.', 'include'))
    modify_include_in_folder(os.path.join('.', 'src'))
    modify_include_in_folder(os.path.join('.', 'UnitTest', 'src'))
