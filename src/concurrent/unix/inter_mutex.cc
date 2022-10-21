//
// Created by xuyan on 2022/6/25.
//

#include "concurrent/mutex.h"
#include "lang/xcl_err.h"
#include <pthread.h>
#include <concurrent/Lock.h>

namespace
{
    class InternalMutex : public xcl::Lock
    {
    public:
        explicit InternalMutex(bool recursive = true);

        ~InternalMutex() override;

    public:
        void
        lock() override;

        void
        unlock() override;

        bool
        tryLock() override;

    protected:
        pthread_mutex_t mutex_;
    };

    void
    InternalMutex::lock()
    {
        Mutex_lock((Mutex*)&mutex_);
    }

    void
    InternalMutex::unlock()
    {
        Mutex_unlock((Mutex*)&mutex_);
    }

    bool
    InternalMutex::tryLock()
    {
        return Mutex_tryLock((Mutex*)&mutex_);
    }

    InternalMutex::InternalMutex(bool recursive) : mutex_()
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

    InternalMutex::~InternalMutex()
    {
        int ret = pthread_mutex_destroy(&mutex_);
        if (ret)
            Err_set(ret);
    }

    class InternalTimedMutex : public xcl::TimedLock, public InternalMutex
    {
    public:
        explicit InternalTimedMutex(bool recursive = true);

    public:
        bool
        tryLock(int32_t timeout) override;

        void
        lock() override;

        void
        unlock() override;

        bool
        tryLock() override;
    };

    bool
    InternalTimedMutex::tryLock(int32_t timeout)
    {
        return Mutex_tryLock2((Mutex*)&mutex_, timeout);
    }

    InternalTimedMutex::InternalTimedMutex(bool recursive)
        : InternalMutex(recursive)
    {}

    void
    InternalTimedMutex::lock()
    {
        InternalMutex::lock();
    }

    void
    InternalTimedMutex::unlock()
    {
        InternalMutex::unlock();
    }

    bool
    InternalTimedMutex::tryLock()
    {
        return InternalMutex::tryLock();
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