#include "xcl/util/CList.h"
#include "xcl/pool/CPool.h"

#include <stdlib.h>

/*
 * link prev and next
 */
static inline void __List_link(CListNode* prev, CListNode* next)
{
    if (prev)
    {
        prev->next = next;
    }
    if (next)
    {
        next->prev = prev;
    }
}

/*
 * unlink node from node's prev and its next
 * but not free
 */
static inline void __List_unlink(CListNode* node)
{
    if (node)
    {
        if (node->prev)
        {
            node->prev->next = node->next;
        }
        if (node->next)
        {
            node->next->prev = node->prev;
        }
        node->next = node->prev = NULL;
    }
}

struct _CList {
    CListNode header;
};

static void __List_init(CList* list)
{
    __List_link(&list->header, &list->header);
}

XCL_PUBLIC(CList*)
List_new()
{
    CList* list = (CList*)Pool_alloc(Pool_def(), sizeof(CList));
    if (list)
    {
        __List_init(list);
    }
    return list;
}

XCL_PUBLIC(bool)
List_delete(CList* list)
{
    if (!List_empty(list))
    {
        return false;
    }
    Pool_dealloc(Pool_def(), list, sizeof(CList));
    return true;
}

XCL_PUBLIC(CListIter)
List_begin(CList* list)
{
    CListIter it = {&list->header, list->header.next};
    return it;
}

XCL_PUBLIC(CListIter)
List_end(CList* list)
{
    CListIter it = {&list->header, &list->header};
    return it;
}

XCL_PUBLIC(CListIter)
List_prev(CListIter it)
{
    CListIter prevIt = {it.terminator, it.cur->prev};
    return prevIt;
}

XCL_PUBLIC(CListIter)
List_next(CListIter it)
{
    CListIter nextIt = {it.terminator, it.cur->next};
    return nextIt;
}

XCL_PUBLIC(void)
List_push(CList* list, CListNode* node)
{
    __List_link(list->header.prev, node);
    __List_link(node, &list->header);
}

XCL_PUBLIC(bool)
List_iterEquals(CListIter it1, CListIter it2)
{
    return it1.cur == it2.cur;
}

XCL_PUBLIC(bool)
List_add(CList* list, CListIter pos, CListNode* node)
{
    if (!node || pos.terminator != &list->header)
    {
        return false;
    }
    __List_link(pos.cur->prev, pos.cur);
    __List_link(node, pos.cur);
    return true;
}

XCL_PUBLIC(void)
List_pushFront(CList* list, CListNode* node)
{
    if (!node)
    {
        return;
    }
    __List_link(node, list->header.next);
    __List_link(&list->header, node);
}

XCL_PUBLIC(CListNode*)
List_pop(CList* list)
{
    CListNode* node = list->header.next;
    if (node == &list->header)
    {
        return NULL;
    }
    __List_unlink(node);
    return node;
}

XCL_PUBLIC(CListNode*)
List_popBack(CList* list)
{
    CListNode* node = list->header.prev;
    if (node != &list->header)
    {
        __List_unlink(node);
        return node;
    }
    return NULL;
}

XCL_PUBLIC(CListIter)
List_peekFirst(CList* list)
{
    CListIter it = {&list->header, &list->header};
    CListNode* node = list->header.next;
    if (node != it.terminator)
    {
        it.cur = node;
    }
    return it;
}

XCL_PUBLIC(CListIter)
List_peekLast(CList* list)
{
    CListIter it = {&list->header, &list->header};
    CListNode* node = list->header.prev;
    if (node != it.terminator)
    {
        it.cur = node;
    }
    return it;
}

XCL_PUBLIC(CListIter)
List_remove(CList* list, CListIter iter)
{
    if (iter.terminator != &list->header || iter.cur == iter.terminator)
    {
        return List_end(list);
    }
    CListNode* node = iter.cur;
    CListNode* next = node->next;
    __List_unlink(iter.cur);
    return (CListIter){&list->header, next};
}

XCL_PUBLIC(CListIter)
List_query(CList* list, const void* value, int (*cmp)(const void*, const void*))
{
    CListNode* cur = list->header.next;
    CListIter it = {&list->header, &list->header};
    while (cur != &list->header)
    {
        if (cmp(cur->data, value) == 0)
        {
            it.cur = cur;
            break;
        }
        cur = cur->next;
    }
    return it;
}

XCL_PUBLIC(CListIter)
List_query2(CList* list,
            CListIter pos,
            const void* value,
            int (*cmp)(const void*, const void*))
{
    CListIter it = List_end(list);
    if (pos.terminator != &list->header)
    {
        return it;
    }
    CListNode* cur = pos.cur;
    while (cur != &list->header)
    {
        if (cmp(cur->data, value) == 0)
        {
            it.cur = cur;
            break;
        }
        cur = cur->next;
    }
    return it;
}

XCL_PUBLIC(uint32_t)
List_size(CList* list)
{
    int size = 0;
    CListNode* cur = list->header.next;
    while (cur != &list->header)
    {
        ++size;
    }
    return size;
}

XCL_PUBLIC(bool)
List_empty(CList* list)
{
    return list->header.next == &list->header;
}

XCL_PUBLIC(bool)
List_spliceRange(CList* list,
                 CListIter pos,
                 CList* other,
                 CListIter first,
                 CListIter last)
{
    if (pos.terminator != &list->header || first.terminator != &other->header ||
        last.terminator != &other->header)
    {
        return false;
    }
    if (last.cur == first.cur)
    {
        return true;
    }
    CListNode* lastRemoved = last.cur->prev;
    __List_link(first.cur->prev, last.cur);
    __List_link(pos.cur->prev, first.cur);
    __List_link(lastRemoved, pos.cur);
    return true;
}

XCL_PUBLIC(bool)
List_splice(CList* list, CListIter pos, CList* other, CListIter it)
{
    if (it.terminator != &other->header || pos.terminator != &list->header ||
        it.cur == it.terminator)
    {
        return false;
    }
    __List_link(it.cur->prev, it.cur->next);
    __List_link(pos.cur->prev, it.cur);
    __List_link(it.cur, pos.cur);
    return true;
}

XCL_PUBLIC(bool)
List_spliceAll(CList* list, CListIter pos, CList* other)
{
    if (pos.terminator != &list->header)
    {
        return false;
    }
    if (List_empty(other))
    {
        return true;
    }
    CListNode* first = other->header.next;
    CListNode* lastRemoved = other->header.prev;
    __List_link(first->prev, &other->header);
    __List_link(pos.cur->prev, first);
    __List_link(lastRemoved, pos.cur);
    return true;
}

/*
 * merge list2 into list1 with cmp
 */
static void
__List_merge(CList* list1, CList* list2, int (*cmp)(const void*, const void*))
{
    CListIter it1 = List_begin(list1);
    CListIter it2 = List_begin(list2);
    while (it1.cur != it1.terminator && it2.cur != it2.terminator)
    {
        int ret = cmp(it1.cur->data, it2.cur->data);
        if (ret <= 0)
        {
            it1.cur = it1.cur->next;
        }
        else
        {
            CListIter it3 = List_next(it2);
            while (it3.cur != it3.terminator)
            {
                if (cmp(it1.cur->data, it3.cur->data) < 0)
                {
                    break;
                }
                it3.cur = it3.cur->next;
            }
            List_spliceRange(list1, it1, list2, it2, it3);
            it2 = it3;
        }
    }
    if (it2.cur != it2.terminator)
    {
        List_spliceAll(list1, it1, list2);
    }
}

XCL_PUBLIC(void)
List_sort(CList* list, int (*cmp)(const void*, const void*))
{
    if (List_empty(list) || list->header.next->next == &list->header)
    {
        return;
    }
    CList carry;
    char data[64 * sizeof(CList)];
    CList* tmp = (CList*)data;
    for (int i = 0; i < 64; i++)
    {
        __List_init(&tmp[i]);
    }
    __List_init(&carry);
    int fill = 0;
    while (!List_empty(list))
    {
        List_splice(&carry, List_end(&carry), list, List_begin(list));
        int i = 0;
        while (!List_empty(&tmp[i]))
        {
            __List_merge(&tmp[i], &carry, cmp);
            List_spliceAll(&carry, List_begin(&carry), &tmp[i++]);
        }
        List_spliceAll(&tmp[i], List_begin(&tmp[i]), &carry);
        if (i > fill)
        {
            fill = i;
        }
    }
    for (int i = 1; i <= fill; i++)
    {
        if (!List_empty(&tmp[i]))
        {
            __List_merge(&tmp[0], &tmp[i], cmp);
        }
    }
    List_spliceAll(list, List_end(list), &tmp[0]);
}
