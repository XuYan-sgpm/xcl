# 通用组件库

## 项目介绍
> 为一些业务项目提供跨平台组件接口，使得业务项目的代码于平台特性无关，将一些组件性的接口重复利用，避免其他项目二次开发
> 
> 在目前的接口当中，主要支持windows，Linux，Android，macos和iOS平台，并提供单元测试。主要提供的接口可以分为工具类，并发类，容器类，io类四大类。
> 这些接口涵盖绝大多数项目中会需要的接口，在多个平台下提供统一的API。
> 
## 编译
> 该项目基于cmake构建，需要预先安装[__cmake__](https://cmake.org/download)环境，确保本地已经安装了cmake 3.10.2以上的环境  
>
> __WINDOWS:__  
>
> mkdir build  
> cd build  
> cmake -G "Visual Studio 16 2019" -A [win32/x64] .. -DCMAKE_INSTALL_PREFIX=/path/to/xcl  
> cmake --build . --target install  
>
> __Unix:__  
>
> mkdir build  
> cd build  
> cmake .. -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/path/to/xcl  
> make  
> make install  
>
> __Android:__  
>
> mkdir build  
> cd build  
> cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/path/to/xcl -DANDROID_NDK=/path/to/ndk -DCMAKE_TOOLCHAIN_FILE=/path/to/ndk/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-21 -DCMAKE_MAKE_PROGRAM=/path/to/ninja  
> cmake --build . --target install  
>
> 输入以上命令之后，可以在安装目录下看到安装的库和头文件  
> 其他平台以此类推  

## 相关配置参数
> xcl的编译过程中有几个配置参数需要额外注意
>
> - **CMAKE_BUILD_TYPE** (可选参数，默认是Debug)
> - **GTEST_PACKAGE** (可选参数，在Windows，Linux，macos下需要指定，用于编译单元测试，该参数受BUILD_TEST影响，如果BUILD_TEST为OFF，该参数无效)
> - **BUILD_TEST** (可选参数，指定是否需要编译单元测试，默认情况下，在Windows，Linux，macos下开启，移动端强制关闭，如果不需要编译单元测试，可以设置为OFF)
> - **VLD_PREFIX** (可选参数，用于Windows平台检测内存泄漏，需要本地安装visual leak detector，该参数指定vld的安装目录，在windows平台如果没有指定该参数，将尝试使用vs自带的asan检测内存越界问题)
>
> **内存分析中默认使用asan，内存泄漏需要额外指定参数（这是因为asan在macos和windows下无法检测内存泄漏），在Windows和macos平台，同一时刻只能进行一次内存分析（内存泄漏或者内存越界），该内存分析仅在Debug模式下生效**