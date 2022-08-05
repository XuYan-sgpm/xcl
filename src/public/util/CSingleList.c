//
// Created by xuyan on 2022/6/23.
//

#include "xcl/util/CSingleList.h"
#include "xcl/pool/CPool.h"
#include <stdlib.h>
#include <string.h>

struct _CSingleList_st {
    CSingleNode* tail;
    CPool* pool;
    CSingleNode header;
};

XCL_EXPORT CSingleList* XCL_API
SingleList_new()
{
    CPool* pool = Pool_def();
    CSingleList* list = Pool_alloc(pool, sizeof(CSingleList));
    if (list)
    {
        list->pool = pool;
        list->header.next = NULL;
        list->tail = &list->header;
    }
    return list;
}

XCL_EXPORT CSingleList* XCL_API
SingleList_newByPool(CPool* pool)
{
    CSingleList* list = Pool_alloc(pool, sizeof(CSingleList));
    if (list)
    {
        list->pool = pool;
        list->header.next = NULL;
        list->tail = &list->header;
    }
    return list;
}

XCL_EXPORT bool XCL_API
SingleList_empty(const CSingleList* list)
{
    return list->header.next == NULL;
}

XCL_EXPORT int32_t XCL_API
SingleList_size(const CSingleList* list)
{
    int32_t count = 0;
    CSingleNode* cur = list->header.next;
    while (cur)
    {
        cur = cur->next;
        ++count;
    }
    return count;
}

XCL_EXPORT CSingleListIter XCL_API
SingleList_begin(CSingleList* list)
{
    CSingleListIter iter = {.tag = &list->header, .cur = list->header.next};
    return iter;
}

XCL_EXPORT CSingleListIter XCL_API
SingleList_end(CSingleList* list)
{
    CSingleListIter iter = {.tag = &list->header, .cur = &list->header};
    return iter;
}

XCL_EXPORT void XCL_API
SingleList_pushFront(CSingleList* list, CSingleNode* node)
{
    node->next = list->header.next;
    list->header.next = node;
    if (list->tail == &list->header)
    {
        list->tail = node;
    }
}

XCL_EXPORT void XCL_API
SingleList_pushBack(CSingleList* list, CSingleNode* node)
{
    node->next = NULL;
    list->tail->next = node;
    list->tail = node;
}

XCL_EXPORT CSingleNode* XCL_API
SingleList_popFront(CSingleList* list)
{
    CSingleNode* node = list->header.next;
    if (node)
    {
        list->header.next = node->next;
        if (!list->header.next)
        {
            list->tail = &list->header;
        }
    }
    return node;
}

XCL_EXPORT CSingleNode* XCL_API
SingleList_popBack(CSingleList* list)
{
    CSingleNode* prev = &list->header;
    CSingleNode* node = prev->next;
    if (!node)
    {
        return NULL;
    }
    while (node->next)
    {
        prev = node;
        node = node->next;
    }
    prev->next = NULL;
    list->tail = prev;
    return node;
}

XCL_EXPORT CSingleListIter XCL_API
SingleList_next(CSingleList* list, CSingleListIter iter)
{
    if (!iter.cur || iter.cur == iter.tag)
    {
        return iter;
    }
    CSingleListIter next = iter;
    if (next.cur->next)
    {
        next.cur = next.cur->next;
    }
    else
    {
        next.cur = next.tag;
    }
    return next;
}

XCL_EXPORT void XCL_API
SingleList_sort(CSingleList* list, int (*cmp)(const void*, const void*))
{
    //
}

XCL_EXPORT bool XCL_API
SingleList_delete(CSingleList* list)
{
    CPool* pool = list->pool;
    if (SingleList_empty(list))
    {
        Pool_dealloc(pool, list, sizeof(CSingleList));
        return true;
    }
    else
    {
        return false;
    }
}
