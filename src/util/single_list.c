//
// Created by xuyan on 2022/6/23.
//

#include "util/single_list.h"
#include "pool/pool.h"
#include <stdlib.h>
#include <string.h>

struct SingleList {
    SingleNode* tail;
    struct Pool* pool;
    SingleNode header;
};

SingleList* XCL_API
SingleList_new()
{
    struct Pool* pool = Pool_global();
    SingleList* list = Pool_alloc(pool, sizeof(SingleList));
    if (list)
    {
        list->pool = pool;
        list->header.next = NULL;
        list->tail = &list->header;
    }
    return list;
}

SingleList* XCL_API
SingleList_newByPool(struct Pool* pool)
{
    SingleList* list = Pool_alloc(pool, sizeof(SingleList));
    if (list)
    {
        list->pool = pool;
        list->header.next = NULL;
        list->tail = &list->header;
    }
    return list;
}

bool XCL_API
SingleList_empty(const SingleList* list)
{
    return list->header.next == NULL;
}

int32_t XCL_API
SingleList_size(const SingleList* list)
{
    int32_t count = 0;
    SingleNode* cur = list->header.next;
    while (cur)
    {
        cur = cur->next;
        ++count;
    }
    return count;
}

SingleListIter XCL_API
SingleList_begin(SingleList* list)
{
    SingleListIter iter = {.tag = &list->header, .cur = list->header.next};
    return iter;
}

SingleListIter XCL_API
SingleList_end(SingleList* list)
{
    SingleListIter iter = {.tag = &list->header, .cur = &list->header};
    return iter;
}

void XCL_API
SingleList_pushFront(SingleList* list, SingleNode* node)
{
    node->next = list->header.next;
    list->header.next = node;
    if (list->tail == &list->header)
    {
        list->tail = node;
    }
}

void XCL_API
SingleList_pushBack(SingleList* list, SingleNode* node)
{
    node->next = NULL;
    list->tail->next = node;
    list->tail = node;
}

SingleNode* XCL_API
SingleList_popFront(SingleList* list)
{
    SingleNode* node = list->header.next;
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

SingleNode* XCL_API
SingleList_popBack(SingleList* list)
{
    SingleNode* prev = &list->header;
    SingleNode* node = prev->next;
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

SingleListIter XCL_API
SingleList_next(SingleList* list, SingleListIter iter)
{
    if (!iter.cur || iter.cur == iter.tag)
    {
        return iter;
    }
    SingleListIter next = iter;
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

void XCL_API
SingleList_sort(SingleList* list, int (*cmp)(const void*, const void*))
{
    //
}

bool XCL_API
SingleList_delete(SingleList* list)
{
    struct Pool* pool = list->pool;
    if (SingleList_empty(list))
    {
        Pool_dealloc(pool, list, sizeof(SingleList));
        return true;
    }
    else
    {
        return false;
    }
}
