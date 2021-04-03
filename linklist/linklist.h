/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : linklist.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/05/12 14:35:54
*  @Last Modified  : 2017/07/17 16:13:52
********************************************************************************
*/

#ifndef LINKLIST_H
#define LINKLIST_H

#include <assert.h>

typedef struct link_node{
    void *data;
    struct link_node *next;
} LLNode;

typedef LLNode *LLIter;

typedef struct link_list{
    int type_size;
    int length;
    LLNode *head;
} LinkList;

void llist_init(LinkList *llist, int type_size);
void llist_clear(LinkList *llist);
int llist_assign(LinkList *llist, const void *data, int count);
int llist_push_back(LinkList *llist, const void *data);
int llist_push_front(LinkList *llist, const void *data);
int llist_pop_back(LinkList *llist);
int llist_pop_front(LinkList *llist);
int llist_index_insert(LinkList *llist, int index, const void *data);
int llist_index_delete(LinkList *llist, int index);
int llist_key_index(LinkList *llist, int (*equal)(const void *, const void*), const void *key);
int llist_key_delete(LinkList *llist, int (*equal)(const void*, const void*), const void *key);
int llist_iter_insert(LinkList *llist, LLIter *iter, const void *data);
int llist_iter_delete(LinkList *llist, LLIter *iter);
LLIter llist_find_first(LinkList *llist, int (*equal)(const void *, const void*), const void *key);
LLIter llist_find_last(LinkList *llist, int (*equal)(const void *, const void*), const void *key);
int llist_count(LinkList *llist, int (*equal)(const void *, const void *), const void *key);
void *llist_at(LinkList *llist, int index);
void llist_traversal(LLNode *head, void (*visit)(void *, void *), void *param);
void llist_reverse_traversal(LLNode *head, void (*visit)(void *, void *), void *param);
void llist_reverse(LinkList *llist);
void llist_selection_sort(LinkList *llist, int (*compare)(const void*, const void*));
void llist_bubble_sort(LinkList *llist, int (*compare)(const void*, const void*));
void llist_quick_sort(LinkList *llist, int (*compare)(const void*, const void*));
void llist_merge_sort(LinkList *llist, int (*compare)(const void*, const void*));

static LLIter llist_begin(LinkList *llist);
static LLIter llist_end(void);
static LLIter llist_next(LLIter iter);
static void *llist_data(LLIter iter);
static int llist_type_size(LinkList *llist);
static int llist_length(LinkList *llist);
static void *llist_front(LinkList *llist);
void *llist_back(LinkList *llist);
static int llist_empty(LinkList *llist);

static inline LLIter llist_begin(LinkList *llist)
{
    assert(llist != NULL);
    return llist->head;
}

static inline LLIter llist_end(void)
{
    return NULL;
}

static inline LLIter llist_next(LLIter iter)
{
    return iter ? iter->next : NULL;
}

static inline void *llist_data(LLIter iter)
{
    assert(iter != NULL);
    return iter->data;
}

static inline int llist_type_size(LinkList *llist)
{
    assert(llist != NULL);
    return llist->type_size;
}

static inline int llist_length(LinkList *llist)
{
    assert(llist != NULL);
    return llist->length;
}

static inline void *llist_front(LinkList *llist)
{
    assert(llist != NULL && llist->head != NULL);
    return llist->head->data;
}

static inline int llist_empty(LinkList *llist)
{
    assert(llist != NULL);
    return llist->length == 0;
}

#endif /* end of include guard: LINKLIST_H */
