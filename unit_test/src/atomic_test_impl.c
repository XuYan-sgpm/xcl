#ifdef STATIC
#include <stdio.h>
#include <assert.h>
#include <xcl/concurrent/xcl_atomic.h>

void
__AtomicTest_func1()
{
    ALIGNED(8) ATOMIC(int64_t) x;
    __Atomic_store64(&x, 89l, memory_order_release);
    printf("%lld\n", __Atomic_load64(&x, memory_order_consume));
    int64_t val = 89;
    printf("%d\n", __Atomic_weakCas64(&x, &val, 876, memory_order_acq_rel));
    printf("%lld\n", __Atomic_load64(&x, memory_order_consume));
}
#endif
