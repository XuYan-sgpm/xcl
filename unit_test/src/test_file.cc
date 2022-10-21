#include <gtest/gtest.h>
#include <iostream>
#include <xcl/io/file.h>
#include <xcl/lang/system.h>
using namespace std;

TEST(FileTest, func1)
{
    char path[2048];
    getCwd(path, sizeof(path));
    cout << boolalpha << File_listDir(path, nullptr, nullptr) << endl;
}
