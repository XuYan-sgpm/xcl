# from curses.ascii import isupper
import os

__WORDS__ = [
    'inter', 'xcl', 'global', 'ini', 'cond', 'mutex', 'cfg', 'linked',
    'blocking', 'queue', 'thread', 'local', 'atomic', 'storage', 'def', 'err',
    'size', 'class', 'ring', 'buffer', 'single', 'list', 'string', 'algo',
    'bits', 'blocker', 'bytes', 'file', 'rb', 'tree', 'lock', 'pool', 'str',
    'system', 'parse', 'api', 'id', 'platform'
]


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
    return s + (name[dot:] if dot != -1 else '')


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


if __name__ == '__main__':
    rename_files_in_folder(os.path.join('.', 'include'))
    rename_files_in_folder(os.path.join('.', 'src'))
