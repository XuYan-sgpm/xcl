#include <stdio.h>
#include <assert.h>
#include <xcl/concurrent/CAtomic.h>

void
__AtomicTest_func1()
{
    ALIGNED(8) ATOMIC(int64_t) x;
    __Atomic_store64(&x, 89l, memory_order_release);
    printf("%ld\n", __Atomic_load64(&x, memory_order_consume));
}