/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : static_dlinklist.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/06/17 21:19:46
*  @Last Modified  : 2021/03/28 09:48:46
********************************************************************************
*/

#ifndef STATIC_DLINKLIST_H
#define STATIC_DLINKLIST_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#pragma pack(1)
typedef struct double_link_list_node_base
{
    bool in_use;
    struct double_link_list_node_base *prev;
    struct double_link_list_node_base *next;
    int data_size;
    unsigned char data[1];
} DLLNodeBase;
#pragma pack()

typedef DLLNodeBase *DLLIter;

typedef struct double_link_list
{
    void *node_array;
    int node_size;
    int node_cnt;
    int node_data_max;
    DLLNodeBase *head;
    DLLNodeBase *tail;
    int length;
} DLinkList;

typedef bool (*EqualFun)(const void*, const void*);
typedef int (*CmpFun)(const void*, const void*);

void dllist_init(DLinkList *dllist, void *node_array, int array_size, int node_cnt);
void dllist_clear(DLinkList *dllist);
int dllist_assign(DLinkList *dllist, const void *array, int array_size, int element_cnt);
bool dllist_push_front(DLinkList *dllist, const void *data, int len);
bool dllist_push_back(DLinkList *dllist, const void *data, int len);
bool dllist_pop_front(DLinkList *dllist);
bool dllist_pop_back(DLinkList *dllist);
bool dllist_index_insert(DLinkList *dllist, int index, const void *data, int len);
bool dllist_index_delete(DLinkList *dllist, int index);
int dllist_key_index(DLinkList *dllist, EqualFun equal, const void *key);
bool dllist_key_delete(DLinkList *dllist, EqualFun equal, const void *key);
bool dllist_iter_insert_front(DLinkList *dllist, DLLIter iter, const void *data, int len);
bool dllist_iter_insert_back(DLinkList *dllist, DLLIter iter, const void *data, int len);
DLLIter dllist_iter_delete(DLinkList *dllist, DLLIter iter);
DLLIter dllist_iter_erase(DLinkList *dllist, DLLIter iter, int cnt);
DLLIter dllist_find_first(DLinkList *dllist, EqualFun equal, const void *key);
DLLIter dllist_find_last(DLinkList *dllist, EqualFun equal, const void *key);
int dllist_count(DLinkList *dllist, EqualFun equal, const void *key);
void *dllist_at(DLinkList *dllist, int index, int *len);
//使用快速排序时内存池中至少要有一个未分配的块。
//排序函数的用法和标准库的qsort一样，比较函数应该返回1、0、-1三个值。
void dllist_quick_sort(DLinkList *dllist, CmpFun compare);
//并归排序比较函数应返回0和非0两个比较值。
void dllist_merge_sort(DLinkList *dllist, CmpFun compare);

static inline DLLIter dllist_begin(DLinkList *dllist);
static inline DLLIter dllist_end(void);
static inline void *dllist_data(DLLIter iter, int *len);
static inline DLLIter dllist_next(DLLIter iter);
static inline DLLIter dllist_prev(DLLIter iter);
static inline DLLIter dllist_rbegin(DLinkList *dllist);
static inline void *dllist_front(DLinkList *dllist, int *len);
static inline void *dllist_back(DLinkList *dllist, int *len);
static inline int dllist_node_data_max(DLinkList *dllist);
static inline int dllist_length(DLinkList *dllist);
static inline bool dllist_empty(DLinkList *dllist);
static inline bool dllist_full(DLinkList *dllist);

static inline DLLIter dllist_begin(DLinkList *dllist)
{
    return dllist->head;
}

static inline DLLIter dllist_rbegin(DLinkList *dllist)
{
    return dllist->tail;
}

static inline DLLIter dllist_end(void)
{
    return NULL;
}

static inline void *dllist_data(DLLIter iter, int *len)
{
    if(len) *len = iter->data_size;
    return iter->data_size > 0 ? iter->data : NULL;
}

static inline DLLIter dllist_next(DLLIter iter)
{
    return iter ? iter->next : NULL;
}

static inline DLLIter dllist_prev(DLLIter iter)
{
    return iter ? iter->prev : NULL;
}

static inline void *dllist_front(DLinkList *dllist, int *len)
{
    if(len) *len = dllist->head->data_size;
    return dllist->head->data_size > 0 ? dllist->head->data : NULL;
}

static inline void *dllist_back(DLinkList *dllist, int *len)
{
    if(len) *len = dllist->tail->data_size;
    return dllist->head->data_size > 0 ? dllist->tail->data : NULL;
}

static inline int dllist_node_data_max(DLinkList *dllist)
{
    return dllist->node_data_max;
}

static inline int dllist_length(DLinkList *dllist)
{
    return dllist->length;
}

static inline bool dllist_empty(DLinkList *dllist)
{
    return dllist->length == 0;
}

static inline bool dllist_full(DLinkList *dllist)
{
    return dllist->length == dllist->node_cnt;
}

#endif /* end of include guard: STATIC_DLINKLIST_H */
