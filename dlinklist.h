/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : dlinklist.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/06/17 21:19:46
*  @Last Modified  : 2017/07/11 09:48:46
********************************************************************************
*/

#ifndef DLINKLIST_H
#define DLINKLIST_H

#include <assert.h>

typedef struct double_link_node {
    void *data;
    struct double_link_node *prev;
    struct double_link_node *next;
} DLLNode;

typedef DLLNode *DllIter;
typedef DLLNode *RDllIter;

typedef struct double_link_list {
    int type_size;
    int length;
    DLLNode *head;
    DLLNode *tail;
} DLinkList;

typedef int (*Comp)(const void*, const void*);

void dllist_init(DLinkList *dllist, int type_size);
void dllist_clear(DLinkList *dllist);
int dllist_assign(DLinkList *dllist, const void *data, int count);
int dllist_push_front(DLinkList *dllist, const void *data);
int dllist_push_back(DLinkList *dllist, const void *data);
int dllist_pop_front(DLinkList *dllist);
int dllist_pop_back(DLinkList *dllist);
int dllist_index_insert(DLinkList *dllist, int index, const void *data);
int dllist_index_delete(DLinkList *dllist, int index);
int dllist_key_index(DLinkList *dllist, Comp equal, const void *key);
int dllist_key_delete(DLinkList *dllist, Comp equal, const void *key);
int dllist_iter_insert(DLinkList *dllist, DllIter iter, const void *data);
int dllist_iter_delete(DLinkList *dllist, DllIter *iter);
DllIter dllist_find_first(DLinkList *dllist, Comp equal, const void *key);
DllIter dllist_find_last(DLinkList *dllist, Comp equal, const void *key);
int dllist_count(DLinkList *dllist, Comp equal, const void *key);
void *dllist_at(DLinkList *dllist, int index);
//排序函数的用法和标准库的qsort一样，比较函数应该返回1、0、-1三个值。
void dllist_quick_sort(DLinkList *dllist, int (*compare)(const void*, const void*));
//并归排序比较函数应返回0和非0两个比较值。
void dllist_merge_sort(DLinkList *dllist, int (*compare)(const void*, const void*));
static DllIter dllist_begin(DLinkList *dllist);
static DllIter dllist_end(void);
static void *dllist_data(DllIter iter);
static DllIter dllist_next(DllIter iter);
static RDllIter dllist_rbegin(DLinkList *dllist);
static RDllIter dllist_rend(void);
static void *dllist_rdata(RDllIter riter);
static RDllIter dllist_rnext(RDllIter riter);
static void *dllist_front(DLinkList *dllist);
static void *dllist_back(DLinkList *dllist);
static int dllist_type_size(DLinkList *dllist);
static int dllist_length(DLinkList *dllist);
static int dllist_empty(DLinkList *dllist);

static inline DllIter dllist_begin(DLinkList *dllist)
{
    assert(dllist != NULL);
    return dllist->head;
}

static inline DllIter dllist_end(void)
{
    return NULL;
}

static inline void *dllist_data(DllIter iter)
{
    assert(iter != NULL);
    return iter->data;
}

static inline DllIter dllist_next(DllIter iter)
{
    return iter ? iter->next : NULL;
}

static inline RDllIter dllist_rbegin(DLinkList *dllist)
{
    assert(dllist != NULL);
    return dllist->tail;
}

static inline RDllIter dllist_rend(void)
{
    return NULL;
}

static inline void *dllist_rdata(RDllIter riter)
{
    assert(riter != NULL);
    return riter->data;
}

static inline RDllIter dllist_rnext(RDllIter riter)
{
    return riter ? riter->prev : NULL;
}

static inline void *dllist_front(DLinkList *dllist)
{
    assert(dllist != NULL);
    return dllist->head->data;
}

static inline void *dllist_back(DLinkList *dllist)
{
    assert(dllist != NULL);
    return dllist->tail->data;
}

static inline int dllist_type_size(DLinkList *dllist)
{
    assert(dllist != NULL);
    return dllist->type_size;
}

static inline int dllist_length(DLinkList *dllist)
{
    assert(dllist != NULL);
    return dllist->length;
}

static inline int dllist_empty(DLinkList *dllist)
{
    assert(dllist != NULL);
    return dllist->length == 0;
}

#endif /* end of include guard: DLINKLIST_H */
