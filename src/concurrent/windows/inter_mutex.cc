//
// Created by xuyan on 2022/6/24.
//

#include <windows.h>
#include "concurrent/Lock.h"

extern "C" bool
__waitHandle(HANDLE handle, DWORD timeout);

namespace
{
    class InternalMutex : public xcl::Lock
    {
    public:
        ~InternalMutex() override;

        InternalMutex();

    private:
        CRITICAL_SECTION critical_section_;

    public:
        void
        lock() override;

        void
        unlock() override;

        bool
        tryLock() override;
    };

    void
    InternalMutex::lock()
    {
        EnterCriticalSection(&critical_section_);
    }

    void
    InternalMutex::unlock()
    {
        LeaveCriticalSection(&critical_section_);
    }

    bool
    InternalMutex::tryLock()
    {
        return TryEnterCriticalSection(&critical_section_);
    }

    InternalMutex::~InternalMutex()
    {
        DeleteCriticalSection(&critical_section_);
    }

    InternalMutex::InternalMutex()
    {
        InitializeCriticalSection(&critical_section_);
    }

    class InternalTimedMutex : public xcl::TimedLock
    {
    public:
        ~InternalTimedMutex() override;

        InternalTimedMutex();

    public:
        void
        lock() override;

        void
        unlock() override;

        bool
        tryLock() override;

        bool
        tryLock(int32_t millis) override;

    private:
        HANDLE handle_;
    };

    void
    InternalTimedMutex::lock()
    {
        __waitHandle(handle_, INFINITE);
    }

    void
    InternalTimedMutex::unlock()
    {
        ::ReleaseMutex(handle_);
    }

    bool
    InternalTimedMutex::tryLock()
    {
        return __waitHandle(handle_, 0);
    }

    bool
    InternalTimedMutex::tryLock(int32_t millis)
    {
        return __waitHandle(handle_, millis);
    }

    InternalTimedMutex::InternalTimedMutex() : handle_(nullptr)
    {
        handle_ = ::CreateMutex(nullptr, false, nullptr);
    }

    InternalTimedMutex::~InternalTimedMutex()
    {
        if (handle_)
        {
            ::CloseHandle(handle_);
            handle_ = nullptr;
        }
    }
} // namespace

xcl::Lock*
xcl::Lock::NewLock()
{
    return new InternalMutex();
}

xcl::TimedLock*
xcl::TimedLock::NewLock()
{
    return new InternalTimedMutex();
}
