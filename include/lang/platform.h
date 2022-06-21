//
// Created by xuyan on 2022/6/20.
//

#pragma once

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

#ifdef __GNUC__
#define GNUC 1
#else
#define GNUC 0
#endif

#ifdef _MSC_VER
#define MSVC 1
#else
#define MSVC 0
#endif

#if defined(__amd64__) || defined(_M_AMD64) || defined(__x86_64__)
#define X64
#else
#define X86
#endif

#ifdef __clang__
#define CLANG 1
#else
#define CLANG 0
#endif
