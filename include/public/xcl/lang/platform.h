//
// Created by 徐琰 on 2021/10/11.
//

#ifndef SCL_PLATFORM_H
#define SCL_PLATFORM_H

#ifdef __APPLE__
#define MACOSX 1
#else
#define MACOSX 0
#endif

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS 1
#else
#define WINDOWS 0
#endif

#ifdef __ANDROID__
#define ANDROID 1
#else
#define ANDROID 0
#endif

#ifdef __linux__
#define LINUX 1
#else
#define LINUX 0
#endif

#endif// SCL_PLATFORM_H
