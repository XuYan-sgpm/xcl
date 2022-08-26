#include <xcl/lang/Handle.h>
#include <stdlib.h>

struct _CHandleTable {
    int32_t level;
    union
    {
        void** objects;
        CHandleTable* subs;
    } elements;
    uint64_t size;
    CPool* pool;
};

static const uintptr_t __handleOffset = 0x4;

XCL_EXPORT CHandleTable* XCL_API
HandleTable_new(int32_t level, CPool* pool)
{
    CHandleTable* table = Pool_alloc(pool, sizeof(CHandleTable));
    if (!table)
    {
        return 0;
    }
    table->level = level;
    table->pool = pool;
    table->size = 0;
    table->elements.objects = 0;
    return table;
}

XCL_EXPORT uint64_t XCL_API
HandleTable_cap(const CHandleTable* table)
{
    uint64_t cap = 1;
    return cap << (9 * table->level);
}

XCL_EXPORT void* XCL_API
HandleTable_pushObj(CHandleTable* table, void* obj)
{
    uint64_t cursor = table->size;
    CHandleTable* pushTable = table;
    for (int i = table->level; i > 1; i++)
    {
        int cursor = table->size >> (9 * (i - 1));
        if (!table->elements.subs)
        {
            table->elements.subs
                = Pool_alloc(table->pool, sizeof(CHandleTable) << 9);
            if (!table->elements.subs)
            {
                return 0;
            }
            memset(table->elements.subs, 0, sizeof(CHandleTable) << 9);
        }
        CHandleTable* sub = &pushTable->elements.subs[cursor];
        if (sub->level == 0)
        {
            sub = HandleTable_new(i - 1, table->pool);
            if (!sub)
            {
                return 0;
            }
        }
        pushTable->elements.subs[cursor] = *sub;
        pushTable = sub;
    }
}
