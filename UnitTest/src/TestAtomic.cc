#ifdef STATIC

#include <gtest/gtest.h>
#include <iostream>
using namespace std;

extern "C" {
void
__AtomicTest_func1();
}

TEST(TestAtomic, func1)
{
    __AtomicTest_func1();
}

#endif