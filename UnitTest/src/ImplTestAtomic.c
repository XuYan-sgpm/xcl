//
// Created by xuyan on 2022/7/29.
//
#include <xcl/concurrent/CInternalAtomic.h>
#include <stdio.h>
#include <stdint.h>

typedef struct {
    void* p;
    int64_t tag;
} TaggedPtr;

void
__testAtomic()
{
    ATOMIC(TaggedPtr) x;
    TaggedPtr ptr1 = {0, 98};
    ATOMIC_STORE(&x, ptr1, memory_order_relaxed);
    TaggedPtr data;
    ATOMIC_LOAD(&x, data, memory_order_relaxed);
    printf("%lld\n", data.tag);
    TaggedPtr ptr = {0, 98};
    CAS(&x, ptr, ptr, memory_order_acq_rel);
}

int
main()
{
    __testAtomic();
    return 0;
}