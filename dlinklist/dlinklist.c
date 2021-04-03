/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : dlinklist.c
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/06/18 01:35:31
*  @Last Modified  : 2017/09/05 20:26:32
********************************************************************************
*/

#include <malloc.h>
#include <string.h>
#include "dlinklist.h"

void dllist_init(DLinkList *dllist, int type_size)
{
    assert(dllist != NULL);
    if(dllist != NULL)
    {
        dllist->type_size = type_size;
        dllist->length = 0;
        dllist->head = NULL;
        dllist->tail = NULL;
    }
}

void dllist_clear(DLinkList *dllist)
{
    assert(dllist != NULL);
    if(dllist != NULL)
    {
        while(dllist->head != NULL)
        {
            DLLNode *temp = dllist->head;
            dllist->head = temp->next;
            free(temp->data);
            free(temp);
        }
        dllist->length = 0;
        dllist->tail = NULL;
    }
}

int dllist_assign(DLinkList *dllist, const void *data, int count)
{
    assert(dllist != NULL && data != NULL && count > 0);
    if(dllist == NULL || data == NULL || count <= 0)
        return 0;
    if(!dllist_empty(dllist))
        dllist_clear(dllist);
    for(int i = 0; i < count; ++i)
    {
        DLLNode *node = (DLLNode *)malloc(sizeof(DLLNode));
        if(node == NULL)
            goto ERROR;
        if((node->data = malloc(dllist->type_size)) == NULL)
        {
            free(node);
            goto ERROR;
        }
        memcpy(node->data, (char *)data + i * dllist->type_size, dllist->type_size);
        node->prev = dllist->tail;
        node->next = NULL;
        if(dllist->tail == NULL)
            dllist->head = dllist->tail = node;
        else
            dllist->tail->next = node;
        dllist->tail = node;
    }
    dllist->length = count;
    return 1;
ERROR:
    dllist_clear(dllist);
    return 0;
}

int dllist_push_front(DLinkList *dllist, const void *data)
{
    assert(dllist != NULL && data != NULL);
    if(dllist != NULL && data != NULL)
    {
        DLLNode *node = (DLLNode *)malloc(sizeof(DLLNode));
        if(node == NULL)
            return 0;
        if((node->data = malloc(dllist->type_size)) == NULL)
        {
            free(node);
            return 0;
        }
        memcpy(node->data, data, dllist->type_size);
        node->prev = NULL;
        node->next = dllist->head;
        if(dllist->head != NULL)
            dllist->head->prev = node;
        else
            dllist->tail = node;
        dllist->head = node;
        ++dllist->length;
        return 1;
    }
    return 0;
}

int dllist_push_back(DLinkList *dllist, const void *data)
{
    assert(dllist != NULL && data != NULL);
    if(dllist != NULL && data != NULL)
    {
        DLLNode *node = (DLLNode *)malloc(sizeof(DLLNode));
        if(node == NULL)
            return 0;
        if((node->data = malloc(dllist->type_size)) == NULL)
        {
            free(node);
            return 0;
        }
        memcpy(node->data, data, dllist->type_size);
        node->prev = dllist->tail;
        node->next = NULL;
        if(dllist->tail != NULL)
            dllist->tail->next = node;
        else
            dllist->head = node;
        dllist->tail = node;
        ++dllist->length;
        return 1;
    }
    return 0;
}

int dllist_pop_front(DLinkList *dllist)
{
    assert(dllist != NULL);
    if(dllist != NULL && !dllist_empty(dllist))
    {
        DLLNode *head = dllist->head;
        DLLNode *next = head->next;
        dllist->head = next;
        if(next != NULL)
            next->prev = NULL;
        else
            dllist->tail = NULL;
        free(head->data);
        free(head);
        --dllist->length;
        return 1;
    }
    return 0;
}

int dllist_pop_back(DLinkList *dllist)
{
    assert(dllist != NULL);
    if(dllist != NULL && !dllist_empty(dllist))
    {
        DLLNode *tail = dllist->tail;
        DLLNode *prev = tail->prev;
        dllist->tail = prev;
        if(dllist->tail != NULL)
            dllist->tail->next = NULL;
        else
            dllist->head = NULL;
        free(tail->data);
        free(tail);
        --dllist->length;
        return 1;
    }
    return 0;
}

int dllist_index_insert(DLinkList *dllist, int index, const void *data)
{
    assert(dllist != NULL && data != NULL);

    if(dllist == NULL || data == NULL)
        return 0;

    if(dllist_empty(dllist) || index < 1)
        return dllist_push_front(dllist, data);
    else if(index >= dllist->length)
        return dllist_push_back(dllist, data);
    else
    {
        DLLNode *node = (DLLNode *)malloc(sizeof(DLLNode));
        if(node == NULL)
            return 0;
        if((node->data = malloc(dllist->type_size)) == NULL)
        {
            free(node);
            return 0;
        }
        memcpy(node->data, data, dllist->type_size);
        DLLNode *prevNode = dllist->head;
        //新结点插入在prevNode的后面
        while(--index)
            prevNode = prevNode->next;
        node->prev = prevNode;
        node->next = prevNode->next;
        prevNode->next->prev = node;
        prevNode->next = node;
        ++dllist->length;
        return 1;
    }
}

int dllist_index_delete(DLinkList *dllist, int index)
{
    assert(dllist != NULL);

    if(dllist != NULL && !dllist_empty(dllist))
    {
        if(index < 1)
            return dllist_pop_front(dllist);
        else if(index >= dllist->length - 1)
            return dllist_pop_back(dllist);
        else 
        {
            DLLNode *delNode = dllist->head;
            while(index--)
                delNode = delNode->next;
            delNode->prev->next = delNode->next;
            delNode->next->prev = delNode->prev;
            free(delNode->data);
            free(delNode);
            --dllist->length;
            return 1;
        }
    }
    return 0;
}

int dllist_key_index(DLinkList *dllist, Comp equal, const void *key)
{
    assert(dllist != NULL && equal != NULL && key != NULL);
    int index = 0, found = 0;
    if(dllist != NULL && equal != NULL && key != NULL)
    {
        for(DllIter iter = dllist_begin(dllist);
            iter != dllist_end();
            iter = dllist_next(iter))
        {
            if(equal(key, dllist_data(iter)))
            {
                found = 1;
                break;
            }
            ++index;
        }
    }
    return found ? index : -1;
}

int dllist_key_delete(DLinkList *dllist, Comp equal, const void *key)
{
    assert(dllist != NULL && equal && key != NULL);
    if(dllist != NULL && equal && key != NULL)
    {
        DllIter iter = dllist_find_first(dllist, equal, key);
        return dllist_iter_delete(dllist, &iter);
    }
    return 0;
}

/*
********************************************************************************
* Note : 若*iter == dllist_begin() 则插入最前面，相当于dllist_push_front()
*        若*iter == dllist_end() 则插入最后面，相当于dllist_push_back()
*        其它情况都是插入在*iter的前面
********************************************************************************
*/
int dllist_iter_insert(DLinkList *dllist, DllIter iter, const void *data)
{
    assert(dllist != NULL && data != NULL);
    if(dllist != NULL && data != NULL)
    {
        if(dllist_empty(dllist) || iter == dllist_begin(dllist))
            return dllist_push_front(dllist, data);
        else if(iter == dllist_end())
            return dllist_push_back(dllist, data);
        else
        {
            DLLNode *node = (DLLNode *)malloc(sizeof(DLLNode));
            if(node == NULL)
                return 0;
            if((node->data = malloc(dllist->type_size)) == NULL)
            {
                free(node);
                return 0;
            }
            memcpy(node->data, data, dllist->type_size);
            node->prev = iter->prev;
            node->next = iter;
            iter->prev->next = node;
            iter->prev = node;
            ++dllist->length;
            return 1;
        }
    }
    return 0;
}

/*
********************************************************************************
* Note : *iter != dllist_end() 时删除*iter对应的节点，否则不删除。
*        原因是dllist_find_first()和dllist_find_last()在找不到应对的数剧时会返回
*        dllist_end()，如果这时候删除就会删除错误。 
*        删除成功后会使迭代器自动向后走一个节点，如果在循环中使用迭代进行删除，则
*        判断删除成功后要使用goto返回前面的操作，否则会错过下个节点的操作。
********************************************************************************
*/
int dllist_iter_delete(DLinkList *dllist, DllIter *iter)
{
    assert(dllist != NULL && iter != NULL);
    if(dllist != NULL && iter != NULL && *iter != dllist_end())
    {
        int result = 0;
        if(*iter == dllist_begin(dllist))
        {
            if((result = dllist_pop_front(dllist)) != 0)
                *iter = dllist_begin(dllist);
            return result;
        }
        else if(dllist_next(*iter) == dllist_end())
        {
            if((result = dllist_pop_back(dllist)) != 0)
                *iter = dllist_end();
            return result;
        }
        else
        {
            DLLNode *preNode = (*iter)->prev;
            DLLNode *currNode = *iter;
            DLLNode *nextNode = (*iter)->next;
            preNode->next = nextNode;
            nextNode->prev = preNode;
            free(currNode->data);
            free(currNode);
            *iter = nextNode;
            --dllist->length;
            return 1;
        }
    }
    return 0;
}

DllIter dllist_find_first(DLinkList *dllist, Comp equal, const void *key)
{
    assert(dllist != NULL && equal != NULL && key != NULL);
    if(dllist != NULL && equal != NULL && key != NULL)
    {
        for(DllIter iter = dllist_begin(dllist);
            iter != dllist_end();
            iter = dllist_next(iter))
        {
            if(equal(key, dllist_data(iter)))
                return iter;
        }
    }
    return dllist_end();
}

DllIter dllist_find_last(DLinkList *dllist, Comp equal, const void *key)
{
    assert(dllist != NULL && equal != NULL && key != NULL);
    if(dllist != NULL && equal != NULL && key != NULL)
    {
        for(RDllIter riter = dllist_rbegin(dllist);
            riter != dllist_rend();
            riter = dllist_rnext(riter))
        {
            if(equal(key, dllist_rdata(riter)))
                return riter;
        }
    }
    return dllist_rend();
}

int dllist_count(DLinkList *dllist, Comp equal, const void *key)
{
    assert(dllist != NULL && equal != NULL && key != NULL);
    int count = 0;
    if(dllist != NULL && equal != NULL && key != NULL)
    {
        for(DllIter iter = dllist_begin(dllist);
            iter != dllist_end();
            iter = dllist_next(iter))
        {
            if(equal(key, dllist_data(iter)))
                ++count;
        }
    }
    return count;
}

void *dllist_at(DLinkList *dllist, int index)
{
    assert(dllist != NULL && index >= 0 && index < dllist->length);
    void *data = NULL;
    if(dllist != NULL && index >= 0 && index < dllist->length)
    {
        DllIter iter = dllist_begin(dllist);
        while(index--)
            iter = dllist_next(iter);
        data = dllist_data(iter);
    }
    return data;
}

//快速找寻两个结点的中间结点
static DLLNode *dllist_middle(DLLNode *low, DLLNode *high)
{
    DLLNode *slow = low, *fast = low;

    //如果有两个节点则返回第一个节点
    /*
     * while(fast != high && fast->next != high)
     * {
     *     fast = fast->next->next;
     *     if(fast != NULL)
     *         slow = slow->next;
     * }
     * return slow;
     */

    //如果有两个节点，则返回第2个节点
    while(fast != high && fast->next != high)
    {
        fast = fast->next->next;
        slow = slow->next;
    }
    return slow;
}

static DLLNode *dllist_partition(DLLNode *low, DLLNode *high, Comp compare)
{
    //如果每次都选第一个点作为基准点，当链表已经有序时就会出现最坏情况。所以最好选择中间点作为基准点。
    //void *pivot = low->data; //不能总是选第一个结点作为基准点。
    DLLNode *middleNode = dllist_middle(low, high);
    void *pivot = middleNode->data;
    middleNode->data = low->data;
    while(low != high)
    {
        while(low != high && compare(high->data, pivot) >= 0)
            high = high->prev;
        if(low != high)
        {
            low->data = high->data;
            low = low->next;
        }

        while(low != high && compare(low->data, pivot) <= 0)
            low = low->next;
        if(low != high)
        {
            high->data = low->data;
            high = high->prev;
        }
    }
    low->data = pivot;
    return low;
}

static void dllist_qsort_recursive(DLLNode *low, DLLNode *high, Comp compare)
{
    if(low != high)
    {
        DLLNode *pivot = dllist_partition(low, high, compare);
        if(low != pivot && low != pivot->prev)
            dllist_qsort_recursive(low, pivot->prev, compare);
        if(high != pivot && high != pivot->next)
            dllist_qsort_recursive(pivot->next, high, compare);
    }
}

void dllist_quick_sort(DLinkList *dllist, int (*compare)(const void*, const void*))
{
    assert(dllist != NULL && compare != NULL);
    if(dllist != NULL && compare != NULL && dllist->length > 1)
        dllist_qsort_recursive(dllist->head, dllist->tail, compare);
}

static inline void dllist_append_node(DLLNode **curr, DLLNode **next)
{
    (*next)->prev = *curr;
    (*curr)->next = *next;
    *curr = *next;
    *next = (*next)->next;
}

//中间分割链表，并返回分割出的新链表
static DLLNode *dllist_middle_split(DLLNode *head)
{
    if(head == NULL) return NULL;
    DLLNode *slow = head, *fast = head;
    while(fast != NULL && fast->next != NULL)
    {
        fast = fast->next->next;
        //当链表只有两个节点时，最终slow应该指向第一个节点。
        //这样就能将只有两个节点的链表拆分成两个分别有一个节点的链表。
        if(fast != NULL)
            slow = slow->next;
    }
    DLLNode *newlist = slow->next;
    newlist->prev = NULL;
    slow->next = NULL;
    return newlist;
}

static DLLNode *dllist_merge_recursive(DLLNode *head, int (*compare)(const void*, const void*))
{
    if(head == NULL) return NULL;
    if(head->next == NULL) return head;

    DLLNode *list1 = head;
    DLLNode *list2 = dllist_middle_split(head);
    DLLNode *left = dllist_merge_recursive(list1, compare);
    DLLNode *right = dllist_merge_recursive(list2, compare);

    if(left == NULL)
        return right;
    else if(right == NULL)
        return left;

    DLLNode headNode, *tailNode = &headNode;
    while(left != NULL && right != NULL)
    {
        if(compare(left->data, right->data))
            dllist_append_node(&tailNode, &left);
        else
            dllist_append_node(&tailNode, &right);
    }
    if(left != NULL && tailNode != NULL)
        dllist_append_node(&tailNode, &left);
    if(right != NULL && tailNode != NULL)
        dllist_append_node(&tailNode, &right);
    headNode.next->prev = NULL;
    return headNode.next;
}

void dllist_merge_sort(DLinkList *dllist, int (*compare)(const void*, const void*))
{
    assert(dllist != NULL && compare != NULL);
    if(dllist != NULL && compare != NULL)
    {
        dllist->head = dllist_merge_recursive(dllist->head, compare);
        DLLNode *tail = dllist->head;
        for(; tail != NULL && tail->next != NULL; tail = tail->next)
            ;
        dllist->tail = tail;
    }
}
