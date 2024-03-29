#include "util/list.h"
#include "pool/pool.h"

#include <stdlib.h>

/*
 * link prev and next
 */
static inline void
__List_link(ListNode* prev, ListNode* next)
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
static inline void
__List_unlink(ListNode* node)
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

struct List {
    ListNode header;
};

static void
__List_init(List* list)
{
    __List_link(&list->header, &list->header);
}

List* XCL_API
List_new()
{
    List* list = (List*)Pool_alloc(Pool_global(), sizeof(List));
    if (list)
    {
        __List_init(list);
    }
    return list;
}

bool XCL_API
List_delete(List* list)
{
    if (!List_empty(list))
    {
        return false;
    }
    Pool_dealloc(Pool_global(), list, sizeof(List));
    return true;
}

ListIter XCL_API
List_begin(List* list)
{
    ListIter it = {&list->header, list->header.next};
    return it;
}

ListIter XCL_API
List_end(List* list)
{
    ListIter it = {&list->header, &list->header};
    return it;
}

ListIter XCL_API
List_prev(ListIter it)
{
    ListIter prevIt = {it.terminator, it.cur->prev};
    return prevIt;
}

ListIter XCL_API
List_next(ListIter it)
{
    ListIter nextIt = {it.terminator, it.cur->next};
    return nextIt;
}

void XCL_API
List_push(List* list, ListNode* node)
{
    __List_link(list->header.prev, node);
    __List_link(node, &list->header);
}

bool XCL_API
List_iterEquals(ListIter it1, ListIter it2)
{
    return it1.cur == it2.cur;
}

bool XCL_API
List_add(List* list, ListIter pos, ListNode* node)
{
    if (!node || pos.terminator != &list->header)
    {
        return false;
    }
    __List_link(pos.cur->prev, pos.cur);
    __List_link(node, pos.cur);
    return true;
}

void XCL_API
List_pushFront(List* list, ListNode* node)
{
    if (!node)
    {
        return;
    }
    __List_link(node, list->header.next);
    __List_link(&list->header, node);
}

ListNode* XCL_API
List_pop(List* list)
{
    ListNode* node = list->header.next;
    if (node == &list->header)
    {
        return NULL;
    }
    __List_unlink(node);
    return node;
}

ListNode* XCL_API
List_popBack(List* list)
{
    ListNode* node = list->header.prev;
    if (node != &list->header)
    {
        __List_unlink(node);
        return node;
    }
    return NULL;
}

ListIter XCL_API
List_peekFirst(List* list)
{
    ListIter it = {&list->header, &list->header};
    ListNode* node = list->header.next;
    if (node != it.terminator)
    {
        it.cur = node;
    }
    return it;
}

ListIter XCL_API
List_peekLast(List* list)
{
    ListIter it = {&list->header, &list->header};
    ListNode* node = list->header.prev;
    if (node != it.terminator)
    {
        it.cur = node;
    }
    return it;
}

ListIter XCL_API
List_remove(List* list, ListIter iter)
{
    if (iter.terminator != &list->header || iter.cur == iter.terminator)
    {
        return List_end(list);
    }
    ListNode* node = iter.cur;
    ListNode* next = node->next;
    __List_unlink(iter.cur);
    return (ListIter){&list->header, next};
}

ListIter XCL_API
List_query(List* list, const void* value, int (*cmp)(const void*, const void*))
{
    ListNode* cur = list->header.next;
    ListIter it = {&list->header, &list->header};
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

ListIter XCL_API
List_query2(List* list,
            ListIter pos,
            const void* value,
            int (*cmp)(const void*, const void*))
{
    ListIter it = List_end(list);
    if (pos.terminator != &list->header)
    {
        return it;
    }
    ListNode* cur = pos.cur;
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

uint32_t XCL_API
List_size(List* list)
{
    int size = 0;
    ListNode* cur = list->header.next;
    while (cur != &list->header)
    {
        ++size;
    }
    return size;
}

bool XCL_API
List_empty(List* list)
{
    return list->header.next == &list->header;
}

bool XCL_API
List_spliceRange(List* list,
                 ListIter pos,
                 List* other,
                 ListIter first,
                 ListIter last)
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
    ListNode* last_removed = last.cur->prev;
    __List_link(first.cur->prev, last.cur);
    __List_link(pos.cur->prev, first.cur);
    __List_link(last_removed, pos.cur);
    return true;
}

bool XCL_API
List_splice(List* list, ListIter pos, List* other, ListIter it)
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

bool XCL_API
List_spliceAll(List* list, ListIter pos, List* other)
{
    if (pos.terminator != &list->header)
    {
        return false;
    }
    if (List_empty(other))
    {
        return true;
    }
    ListNode* first = other->header.next;
    ListNode* last_removed = other->header.prev;
    __List_link(first->prev, &other->header);
    __List_link(pos.cur->prev, first);
    __List_link(last_removed, pos.cur);
    return true;
}

/*
 * merge list2 into list1 with cmp
 */
static void
__List_merge(List* list1, List* list2, int (*cmp)(const void*, const void*))
{
    ListIter it1 = List_begin(list1);
    ListIter it2 = List_begin(list2);
    while (it1.cur != it1.terminator && it2.cur != it2.terminator)
    {
        int ret = cmp(it1.cur->data, it2.cur->data);
        if (ret <= 0)
        {
            it1.cur = it1.cur->next;
        }
        else
        {
            ListIter it3 = List_next(it2);
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

void XCL_API
List_sort(List* list, int (*cmp)(const void*, const void*))
{
    if (List_empty(list) || list->header.next->next == &list->header)
    {
        return;
    }
    List carry;
    char data[64 * sizeof(List)];
    List* tmp = (List*)data;
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
