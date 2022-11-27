#include <gtest/gtest.h>
#include <iostream>
#include <xcl/io/file.h>
#include <xcl/lang/system.h>
using namespace std;

static void
__listDirCallback(void* obj, const TChar* name)
{
    cout << name << endl;
}

TEST(FileTest, func1)
{
    char path[2048];
    currentWorkingDir(path, sizeof(path));
    cout << boolalpha << File_listDir(path, __listDirCallback, nullptr) << endl;
}
