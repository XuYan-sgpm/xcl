// //
// // Created by 徐琰 on 2023/10/21.
// //

// #include <xcl/concurrent/util/spin_lock.h>
// #include <internal/concurrent/atomic.h>
// #include <stdio.h>
// #include <stdlib.h>

// static inline always_inline void __SpinLock_check(SpinLock* spin_lock)
// {
//     if (((uintptr_t)spin_lock) & 3)
//     {
//         fprintf(stderr, "spin lock address is not aligned with 4 bytes\n");
//         abort();
//     }
// }

// void SpinLock_init(SpinLock* spin_lock)
// {
//     __atomic_init_flag(&spin_lock->handle);
// }

// bool SpinLock_try(SpinLock* spin_lock)
// {
//     __SpinLock_check(spin_lock);
//     return !__atomic_tas(&spin_lock->handle, ACQ_REL);
// }

// void SpinLock_lock(SpinLock* spin_lock)
// {
//     __SpinLock_check(spin_lock);
//     while (__atomic_tas(&spin_lock->handle, ACQ_REL))
//     {
//     }
// }

// void SpinLock_unlock(SpinLock* spin_lock)
// {
//     __SpinLock_check(spin_lock);
//     __atomic_reset(&spin_lock->handle, RELEASE);
// }
