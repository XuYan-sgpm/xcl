// //
// // Created by 14049 on 2024/5/9.
// //

// #include <xcl/concurrent/util/blocker.h>
// #include <stdlib.h>
// #include <xcl/concurrent/mutex.h>
// #include <xcl/concurrent/condition.h>
// #include <xcl/lang/system.h>
// #include <internal/concurrent/atomic.h>
// #include <xcl/lang/alloc.h>

// struct Blocker {
//     volatile int flag;
//     volatile int wait;
//     void* mut;
//     void* cond;
// };

// struct Blocker* Blocker_new(void* mut, void* cond)
// {
//     struct Blocker* blocker = palign(sizeof(struct Blocker), PTR_SIZE);
//     if (blocker)
//     {
//         if ((blocker->mut = mut ? mut : Mutex_new())
//             && (blocker->cond = cond ? cond : Cond_new()))
//         {
//             blocker->flag = 0;
//             blocker->wait = 0;
//             return blocker;
//         }
//         if (blocker->mut)
//             Mutex_delete(blocker->mut);
//         pfreea(blocker);
//     }
//     return NULL;
// }

// void Blocker_free(struct Blocker* blocker)
// {
//     if (blocker)
//     {
//         Mutex_delete(blocker->mut);
//         Cond_delete(blocker->cond);
//         pfreea(blocker);
//     }
// }

// void Blocker_wait(struct Blocker* blocker)
// {
//     if (!__atomic_load32(&blocker->flag, ACQUIRE))
//     {
//         Mutex_lock(blocker->mut);
//         while (!__atomic_load32(&blocker->flag, ACQUIRE))
//         {
//             __atomic_fetch_add32(&blocker->wait, 1, ACQ_REL);
//             Cond_wait(blocker->mut, blocker->cond);
//             __atomic_fetch_sub32(&blocker->wait, 1, ACQ_REL);
//         }
//         Mutex_unlock(blocker->mut);
//     }
// }

// bool Blocker_wait2(struct Blocker* blocker, int millis)
// {
//     if (__atomic_load32(&blocker->flag, ACQUIRE))
//         return true;
//     if (!millis)
//         return false;
//     bool ret;
//     Mutex_lock(blocker->mut);
//     if (millis < 0)
//     {
//         while (!(ret = __atomic_load32(&blocker->flag, ACQUIRE)))
//         {
//             __atomic_fetch_add32(&blocker->wait, 1, ACQ_REL);
//             Cond_wait(blocker->mut, blocker->cond);
//             __atomic_fetch_sub32(&blocker->wait, 1, ACQ_REL);
//         }
//     }
//     else
//     {
//         int64_t st = nanos();
//         int cost = 0;
//         while (!(ret = __atomic_load32(&blocker->flag, ACQUIRE))
//                && cost < millis)
//         {
//             __atomic_fetch_add32(&blocker->wait, 1, ACQ_REL);
//             Cond_waitFor(blocker->mut, blocker->cond, millis - cost);
//             cost = (int)(nanos() - st) / 1000000;
//             __atomic_fetch_sub32(&blocker->wait, 1, ACQ_REL);
//         }
//     }
//     Mutex_unlock(blocker->mut);
//     return ret;
// }

// void Blocker_signal(struct Blocker* blocker)
// {
//     __atomic_exchange32(&blocker->flag, 1, ACQ_REL);
//     Mutex_lock(blocker->mut);
//     if (__atomic_load32(&blocker->wait, ACQUIRE))
//         Cond_signal(blocker->cond);
//     Mutex_unlock(blocker->mut);
// }

// void Blocker_broadcast(struct Blocker* blocker)
// {
//     __atomic_exchange32(&blocker->flag, 1, ACQ_REL);
//     Mutex_lock(blocker->mut);
//     if (__atomic_load32(&blocker->wait, ACQUIRE))
//         Cond_broadcast(blocker->cond);
//     Mutex_unlock(blocker->mut);
// }
