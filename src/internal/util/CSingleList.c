//
// Created by xuyan on 2022/6/23.
//

#include "xcl/util/CSingleList.h"
#include "xcl/pool/CPool.h"
#include <stdlib.h>
#include <string.h>

struct _CSingleList_st {
    CSingleNode* tail;
    CSingleNode header;
    CPool* pool;
};

XCL_PUBLIC(CSingleList*)
SingleList_new()
{
    CSingleList* list = Pool_alloc(NULL, sizeof(CSingleList));
    if (list)
    {
        list->pool = NULL;
        list->header.next = NULL;
        list->tail = &list->header;
    }
    return list;
}

XCL_PUBLIC(CSingleList*)
SingleList_new2(CPool* pool)
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

XCL_PUBLIC(bool)
SingleList_empty(const CSingleList* list)
{
    return list->header.next == NULL;
}

XCL_PUBLIC(int32_t)
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

XCL_PUBLIC(CSingleListIter)
SingleList_begin(CSingleList* list)
{
    CSingleListIter iter = {.tag = &list->header, .cur = list->header.next};
    return iter;
}

XCL_PUBLIC(CSingleListIter)
SingleList_end(CSingleList* list)
{
    CSingleListIter iter = {.tag = &list->header, .cur = &list->header};
    return iter;
}

XCL_PUBLIC(void)
SingleList_pushFront(CSingleList* list, CSingleNode* node)
{
    node->next = list->header.next;
    list->header.next = node;
    if (list->tail == &list->header)
    {
        list->tail = node;
    }
}

XCL_PUBLIC(void)
SingleList_pushBack(CSingleList* list, CSingleNode* node)
{
    node->next = NULL;
    list->tail->next = node;
    list->tail = node;
}

XCL_PUBLIC(CSingleNode*)
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

XCL_PUBLIC(CSingleNode*)
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

XCL_PUBLIC(CSingleListIter)
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

XCL_PUBLIC(void)
SingleList_sort(CSingleList* list, int (*cmp)(const void*, const void*))
{
    //
}

XCL_PUBLIC(void)
SingleList_delete(CSingleList* list)
{
    Pool_dealloc(list->pool, list, sizeof(CSingleList));
}
