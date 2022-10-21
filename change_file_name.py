import os

c_family_formats = ['.c', '.h', '.cc', '.cxx', '.cpp', '.hpp', '.hxx']


def __do_name_change(abs_path: str, file_name_no_ext: str, file_format: str):
    indexes = []
    for i, c in enumerate(file_name_no_ext):
        if c.isupper():
            indexes.append(i)
    if not indexes:
        print('no file need to be changed')
        return
    tmp = file_name_no_ext.lower()
    words = []
    indexes.append(len(tmp))
    for i in range(0, len(indexes) - 1):
        words.append(tmp[indexes[i]:indexes[i + 1]])
    final_name = '_'.join(words) + file_format
    # print(abs_path, file_name_no_ext, file_format, final_name)
    final_path = os.path.join(os.path.dirname(abs_path), final_name)
    print('{} -> {}'.format(abs_path, final_path))
    os.rename(abs_path, final_path)


def change_file_name(folder_path: str):
    if not os.path.isdir(folder_path):
        print('not folder:', folder_path)
        return
    if not os.path.exists(folder_path):
        print(folder_path, 'is not exists')
        return
    global c_family_formats
    names = os.listdir(folder_path)
    for name in names:
        abs_path = os.path.join(folder_path, name)
        if os.path.isdir(abs_path):
            change_file_name(abs_path)
            continue
        dot = name.rfind('.')
        if dot == -1:
            print('unknown file format')
            continue
        file_format = name[dot:]
        if not file_format in c_family_formats:
            print('invalid file format')
            continue
        __do_name_change(abs_path, name[:dot], file_format)


change_file_name('./include')
change_file_name('./src')
change_file_name('./UnitTest/src')
