//
// Created by xuyan on 2022/6/20.
//

#pragma once

#include <stddef.h>

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

#ifdef __clang__
#define CLANG 1
#else
#define CLANG 0
#endif

#if defined(_MSC_VER) && !CLANG && !GNUC
#define MSVC 1
#else
#define MSVC 0
#endif

#if defined(__amd64__) || defined(_M_AMD64) || defined(__x86_64__)
#define X64 1
#define X86 0
#else
#define X86 1
#define X64 1
#endif

#ifdef _MSC_VER
#ifdef BUILD_LIBRARY
#ifdef DYNAMIC
#define XCL_EXPORT __declspec(dllexport)
#else
#define XCL_EXPORT
#endif
#elif defined(DYNAMIC)
#define XCL_EXPORT __declspec(dllimport)
#else
#define XCL_EXPORT
#endif
#elif CLANG || GNUC
#define XCL_EXPORT __attribute__((visibility("default")))
#else
#define XCL_EXPORT
#endif

#if GNUC || CLANG
#define XCL_LOCAL __attribute__((visibility("hidden")))
#else
#define XCL_LOCAL
#endif

#if WINDOWS
#define XCL_API __stdcall
#else
#define XCL_API
#endif

#ifdef _MSC_VER
#define ALIGNED(x) __declspec(align(x))
#else
#define ALIGNED(x) __attribute__((aligned(x)))
#endif
