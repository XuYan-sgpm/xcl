//
// Created by xuyan on 2022/6/24.
//

#include "xcl/concurrent/Lock.h"
#include <WinBase.h>
#include <handleapi.h>
#include <synchapi.h>
#include <windef.h>
#include <windows.h>

extern "C" bool __Win32_wait(HANDLE handle, DWORD timeout);

namespace
{
    class __InternalWinMutex : public xcl::Lock
    {
    public:
        ~__InternalWinMutex() override;

        __InternalWinMutex();

    private:
        CRITICAL_SECTION criticalSection_;

    public:
        void lock() override;

        void unlock() override;

        bool tryLock() override;
    };

    void __InternalWinMutex::lock() { EnterCriticalSection(&criticalSection_); }

    void __InternalWinMutex::unlock()
    {
        LeaveCriticalSection(&criticalSection_);
    }

    bool __InternalWinMutex::tryLock()
    {
        return TryEnterCriticalSection(&criticalSection_);
    }

    __InternalWinMutex::~__InternalWinMutex()
    {
        DeleteCriticalSection(&criticalSection_);
    }

    __InternalWinMutex::__InternalWinMutex()
    {
        InitializeCriticalSection(&criticalSection_);
    }

    class __InternalWinTimedMutex : public xcl::TimedLock
    {
    public:
        ~__InternalWinTimedMutex() override;

        __InternalWinTimedMutex();

    public:
        void lock() override;

        void unlock() override;

        bool tryLock() override;

        bool tryLock(int32_t millis) override;

    private:
        HANDLE handle_;
    };

    void __InternalWinTimedMutex::lock() { __Win32_wait(handle_, INFINITE); }

    void __InternalWinTimedMutex::unlock() { ::ReleaseMutex(handle_); }

    bool __InternalWinTimedMutex::tryLock() { return __Win32_wait(handle_, 0); }

    bool __InternalWinTimedMutex::tryLock(int32_t millis)
    {
        return __Win32_wait(handle_, millis);
    }

    __InternalWinTimedMutex::__InternalWinTimedMutex() : handle_(nullptr)
    {
        handle_ = ::CreateMutex(nullptr, false, nullptr);
    }

    __InternalWinTimedMutex::~__InternalWinTimedMutex()
    {
        if (handle_)
        {
            ::CloseHandle(handle_);
            handle_ = nullptr;
        }
    }
}// namespace

xcl::Lock* xcl::Lock::NewLock() { return new __InternalWinMutex(); }

xcl::TimedLock* xcl::TimedLock::NewLock()
{
    return new __InternalWinTimedMutex();
}
