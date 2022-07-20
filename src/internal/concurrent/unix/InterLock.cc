//
// Created by xuyan on 2022/6/25.
//

#include "xcl/concurrent/CMutex.h"
#include "xcl/lang/XclErr.h"
#include <pthread.h>
#include <xcl/concurrent/Lock.h>

namespace
{
    class __InternalUnixMutex : public xcl::Lock
    {
    public:
        explicit __InternalUnixMutex(bool recursive = true);

        ~__InternalUnixMutex() override;

    public:
        void lock() override;

        void unlock() override;

        bool tryLock() override;

    protected:
        pthread_mutex_t mutex_;
    };

    void __InternalUnixMutex::lock()
    {
        Mutex_lock((CMutex*)&mutex_);
    }

    void __InternalUnixMutex::unlock()
    {
        Mutex_unlock((CMutex*)&mutex_);
    }

    bool __InternalUnixMutex::tryLock()
    {
        return Mutex_tryLock((CMutex*)&mutex_);
    }

    __InternalUnixMutex::__InternalUnixMutex(bool recursive) : mutex_()
    {
        int ret;
        if (recursive)
        {
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
            ret = pthread_mutex_init(&mutex_, &attr);
        }
        else
        {
            ret = pthread_mutex_init(&mutex_, nullptr);
        }
        if (ret)
            Err_set(ret);
    }

    __InternalUnixMutex::~__InternalUnixMutex()
    {
        int ret = pthread_mutex_destroy(&mutex_);
        if (ret)
            Err_set(ret);
    }

    class __InternalUnixTimedMutex : public xcl::TimedLock,
                                     public __InternalUnixMutex
    {
    public:
        explicit __InternalUnixTimedMutex(bool recursive = true);

    public:
        bool tryLock(int32_t timeout) override;

        void lock() override;

        void unlock() override;

        bool tryLock() override;
    };

    bool __InternalUnixTimedMutex::tryLock(int32_t timeout)
    {
        return Mutex_tryLock2((CMutex*)&mutex_, timeout);
    }

    __InternalUnixTimedMutex::__InternalUnixTimedMutex(bool recursive)
        : __InternalUnixMutex(recursive)
    {}

    void __InternalUnixTimedMutex::lock()
    {
        __InternalUnixMutex::lock();
    }

    void __InternalUnixTimedMutex::unlock()
    {
        __InternalUnixMutex::unlock();
    }

    bool __InternalUnixTimedMutex::tryLock()
    {
        return __InternalUnixMutex::tryLock();
    }
}// namespace

xcl::Lock* xcl::Lock::NewLock()
{
    return new __InternalUnixMutex();
}

xcl::TimedLock* xcl::TimedLock::NewLock()
{
    return new __InternalUnixTimedMutex();
}