#include <xcl/util/rb_tree.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <xcl/lang/handle_table.h>

typedef struct EntryContext {
    int32_t refs;
    void (*destructor)(void*);
    void* obj;
} EntryContext;

typedef struct TableEntry {
    union
    {
        EntryContext context;
        struct TableEntry* referenced;
    } entry_data;
    intptr_t info;
} TableEntry;

static bool
__TableEntry_isReferred(const TableEntry* entry)
{
    int off = sizeof(intptr_t) << 3;
    return (entry->info >> off - 1) & 1;
}

static void
__TableEntry_setReferred(TableEntry* entry)
{
    int off = sizeof(intptr_t) << 3;
    entry->info |= (1 << off - 1);
}

static intptr_t
__TableEntry_id(const TableEntry* entry)
{
    int off = sizeof(intptr_t) << 3;
    intptr_t mask = 1 << off - 1;
    mask = ~mask;
    return entry->info & mask;
}

static bool
__TableEntry_setId(TableEntry* entry, intptr_t id)
{
    if (id < 0)
    {
        return false;
    }
    intptr_t is_referred = __TableEntry_isReferred(entry);
    entry->info = (is_referred << (sizeof(intptr_t) << 3) - 1) | id;
    return true;
}

static void
__TableEntry_setData(TableEntry* entry,
                     int32_t refs,
                     void (*func)(void*),
                     void* obj,
                     intptr_t id)
{
    entry->entry_data.context.obj = obj;
    entry->entry_data.context.destructor = func;
    entry->entry_data.context.refs = refs;
    __TableEntry_setId(entry, id);
}

static void
__TableEntry_release(TableEntry* entry)
{
    if (__TableEntry_isReferred(entry))
    {
        TableEntry* referred = entry->entry_data.referenced;
        referred->entry_data.context.refs--;
    }
    else
    {
        int refs = entry->entry_data.context.refs--;
        if (refs == 1)
        {
            void (*func)(void*) = entry->entry_data.context.destructor;
            if (func)
            {
                void* obj = entry->entry_data.context.obj;
                func(obj);
            }
        }
    }
}

struct HandleTable {
    intptr_t last;
    RbTree* tree;
};

static TableEntry*
__HandleTable_addEntry(HandleTable* table, TableEntry entry)
{
    RbTree* tree = table->tree;
    RbNode* newNode = __RbTree_allocNode(tree, sizeof(entry));
    if (!newNode)
        return NULL;
    *(TableEntry*)(newNode + 1) = entry;
    RbNode* cur = __RbTree_root(tree);
    int64_t diff;
    RbNode* parent = &tree->header;
    bool left = true;
    while (cur)
    {
        parent = cur;
        diff
            = __TableEntry_id(&entry) - __TableEntry_id((TableEntry*)(cur + 1));
        left = diff < 0;
        if (diff < 0)
            cur = cur->left;
        else
            cur = cur->right;
    }
    __RbTree_addNode(tree, left, parent, newNode);
    return (TableEntry*)(newNode + 1);
}

static RbNode*
__HandleTable_find(HandleTable* table, void* handle)
{
    if (!handle)
    {
        return false;
    }
    intptr_t id = (intptr_t)handle >> 2;
    RbNode* cur = __RbTree_root(table->tree);
    if (!cur)
        return NULL;
    do
    {
        int64_t diff = __TableEntry_id((TableEntry*)(cur + 1)) - id;
        if (diff == 0)
            return cur;
        if (diff < 0)
            cur = cur->right;
        else
            cur = cur->left;
    } while (cur);
    return cur;
}

HandleTable*
HandleTable_new(struct Pool* pool)
{
    HandleTable* table = Pool_alloc(pool, sizeof(HandleTable));
    if (table)
    {
        table->last = 0;
        table->tree = __RbTree_new(pool);
        if (!table->tree)
        {
            Pool_dealloc(pool, table, sizeof(*table));
            table = NULL;
        }
    }
    return table;
}

void*
HandleTable_add(HandleTable* table, void* obj, void (*destructor)(void*))
{
    TableEntry entry;
    memset(&entry, 0, sizeof(entry));
    __TableEntry_setData(&entry, 1, destructor, obj, table->last + 1);
    __HandleTable_addEntry(table, entry);
    ++table->last;
    return (void*)(table->last << 2);
}

void*
HandleTable_ref(HandleTable* table, void* handle)
{
    RbNode* node = __HandleTable_find(table, handle);
    if (!node)
        return NULL;
    TableEntry* entry = (TableEntry*)(node + 1);
    TableEntry new_entry;
    __TableEntry_setId(&new_entry, table->last + 1);
    __TableEntry_setReferred(&new_entry);
    new_entry.entry_data.referenced = entry;
    if (!__HandleTable_addEntry(table, new_entry))
        return NULL;
    entry->entry_data.context.refs++;
    ++table->last;
    return (void*)(table->last << 2);
}

void
HandleTable_close(HandleTable* table, void* handle)
{
    RbNode* node = __HandleTable_find(table, handle);
    if (!node)
    {
        return;
    }
    TableEntry* entry = (TableEntry*)(node + 1);
    if (__TableEntry_isReferred(entry))
    {
        TableEntry* referred = entry->entry_data.referenced;
        referred->entry_data.context.refs--;
    }
    else
    {
        int refs = entry->entry_data.context.refs--;
        if (refs == 1)
        {
            __TableEntry_release(entry);
        }
    }
    __RbTree_removeNode(table->tree, node);
    __RbTree_deallocNode(table->tree, node, sizeof(TableEntry));
}

void*
HandleTable_get(HandleTable* table, void* handle)
{
    RbNode* node = __HandleTable_find(table, handle);
    if (!node)
    {
        return NULL;
    }
    TableEntry* entry = (TableEntry*)(node + 1);
    if (__TableEntry_isReferred(entry))
    {
        entry = entry->entry_data.referenced;
    }
    return entry->entry_data.context.obj;
}

void
HandleTable_delete(HandleTable* table)
{
    RbNode* cur = __RbTree_begin(table->tree);
    for (; cur != __RbTree_end(table->tree);)
    {
        RbNode* next = __RbTree_next(table->tree, cur);
        TableEntry* entry = (TableEntry*)(cur + 1);
        __TableEntry_release(entry);
        __RbTree_removeNode(table->tree, cur);
        __RbTree_deallocNode(table->tree, cur, sizeof(TableEntry));
        cur = next;
    }
    Pool_dealloc(table->tree->pool, table, sizeof(*table));
}

bool
HandleTable_valid(HandleTable* table, void* handle)
{
    return __HandleTable_find(table, handle);
}
