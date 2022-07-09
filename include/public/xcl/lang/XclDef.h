//
// Created by xuyan on 2022/6/20.
//

#pragma once

#ifdef __APPLE__
#  define MACOSX 1
#else
#  define MACOSX 0
#endif

#if defined(_WIN32) || defined(_WIN64)
#  define WINDOWS 1
#else
#  define WINDOWS 0
#endif

#ifdef __ANDROID__
#  define ANDROID 1
#else
#  define ANDROID 0
#endif

#ifdef __linux__
#  define LINUX 1
#else
#  define LINUX 0
#endif

#ifdef __GNUC__
#  define GNUC 1
#else
#  define GNUC 0
#endif

#ifdef __clang__
#  define CLANG 1
#else
#  define CLANG 0
#endif

#if defined(_MSC_VER) && !CLANG && !GNUC
#  define MSVC 1
#else
#  define MSVC 0
#endif

#if defined(__amd64__) || defined(_M_AMD64) || defined(__x86_64__)
#  define X64
#else
#  define X86
#endif

#if WINDOWS
#  if defined(_MSC_VER) && defined(DYNAMIC)
#    define XCL_PUBLIC __declspec(dllexport)
#  elif GNUC || CLANG
#    define XCL_PUBLIC __attribute__((visibility("default")))
#  else
#    define XCL_PUBLIC
#  endif
#  if GNUC || CLANG
#    define XCL_HIDDEN __attribute__((visibility("hidden")))
#  else
#    define XCL_HIDDEN
#  endif
#elif GNUC
#  define XCL_HIDDEN __attribute__((visibility("hidden")))
#  define XCL_PUBLIC __attribute__((visibility("default")))
#else
#  define XCL_HIDDEN
#  define XCL_PUBLIC
#endif

#define XCL_API __stdcall
