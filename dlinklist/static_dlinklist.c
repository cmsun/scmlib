/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : static_dlinklist.c
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/06/18 01:35:31
*  @Last Modified  : 2021/03/28 20:26:32
********************************************************************************
*/

#include <string.h>
#include "utility.h"
#include "static_dlinklist.h"

static DLLNodeBase *dllist_node_malloc(DLinkList *dllist)
{
    DLLNodeBase *temp, *node = NULL;
    for(int i = 0; i < dllist->node_cnt; ++i)
    {
        temp = (DLLNodeBase *)((unsigned char *)dllist->node_array + (dllist->node_size * i));
        if(!temp->in_use)
        {
            temp->in_use = true;
            node = temp;
            break;
        }
    }
    return (DLLNodeBase *)node;
}

static inline void dllist_node_free(DLLNodeBase *node)
{
    node->in_use = false;
}

void dllist_init(DLinkList *dllist, void *node_array, int array_size, int node_cnt)
{
    if(dllist != NULL)
    {
        dllist->node_array = node_array;
        dllist->node_size = array_size/node_cnt;
        dllist->node_cnt = node_cnt;
        dllist->node_data_capacity = dllist->node_size - sizeof(DLLNodeBase) + 1;
        dllist->head = NULL;
        dllist->tail = NULL;
        dllist->length = 0;
    }
}

void dllist_clear(DLinkList *dllist)
{
    if(dllist != NULL)
    {
        while(dllist->head != NULL)
        {
            DLLNodeBase *temp = dllist->head;
            dllist->head = temp->next;
            dllist_node_free(temp);
        }
        dllist->length = 0;
        dllist->tail = NULL;
    }
}

bool dllist_assign(DLinkList *dllist, const void *data, int len, int count)
{
    if(dllist == NULL || data == NULL || count <= 0)
        return false;
    if(!dllist_empty(dllist))
        dllist_clear(dllist);
    for(int i = 0; i < count; ++i)
    {
        DLLNodeBase *node = dllist_node_malloc(dllist);
        if(node == NULL)
            goto ERROR;
        node->data_size = min(len, dllist->node_data_capacity);
        memcpy(node->data, (unsigned char *)data + i*node->data_size, node->data_size);
        node->prev = dllist->tail;
        node->next = NULL;
        if(dllist->tail == NULL)
            dllist->head = dllist->tail = node;
        else
            dllist->tail->next = node;
        dllist->tail = node;
    }
    dllist->length = count;
    return true;
    
ERROR:
    dllist_clear(dllist);
    return false;
}

bool dllist_push_front(DLinkList *dllist, const void *data, int len)
{
    if(dllist != NULL && data != NULL)
    {
        DLLNodeBase *node = dllist_node_malloc(dllist);
        if(node == NULL)
            return false;
        node->data_size = min(len, dllist->node_data_capacity);
        memcpy(node->data, data, node->data_size);
        node->prev = NULL;
        node->next = dllist->head;
        if(dllist->head != NULL)
            dllist->head->prev = node;
        else
            dllist->tail = node;
        dllist->head = node;
        dllist->length++;
        return true;
    }
    return false;
}

bool dllist_push_back(DLinkList *dllist, const void *data, int len)
{
    if(dllist != NULL && data != NULL)
    {
        DLLNodeBase *node = dllist_node_malloc(dllist);
        if(node == NULL)
            return false;
        node->data_size = min(len, dllist->node_data_capacity);
        memcpy(node->data, data, node->data_size);
        node->prev = dllist->tail;
        node->next = NULL;
        if(dllist->tail != NULL)
            dllist->tail->next = node;
        else
            dllist->head = node;
        dllist->tail = node;
        dllist->length++;
        return true;
    }
    return false;
}

bool dllist_pop_front(DLinkList *dllist)
{
    if(dllist != NULL && !dllist_empty(dllist))
    {
        DLLNodeBase *head = dllist->head;
        DLLNodeBase *next = head->next;
        dllist->head = next;
        if(next != NULL)
            next->prev = NULL;
        else
            dllist->tail = NULL;
        dllist_node_free(head);
        dllist->length--;
        return true;
    }
    return false;
}

bool dllist_pop_back(DLinkList *dllist)
{
    if(dllist != NULL && !dllist_empty(dllist))
    {
        DLLNodeBase *tail = dllist->tail;
        DLLNodeBase *prev = tail->prev;
        dllist->tail = prev;
        if(dllist->tail != NULL)
            dllist->tail->next = NULL;
        else
            dllist->head = NULL;
        dllist_node_free(tail);
        dllist->length--;
        return true;
    }
    return false;
}

bool dllist_index_insert(DLinkList *dllist, int index, const void *data, int len)
{
    if(dllist == NULL || data == NULL)
        return false;

    if(dllist_empty(dllist) || index < 1)
        return dllist_push_front(dllist, data, len);
    else if(index >= dllist->length)
        return dllist_push_back(dllist, data, len);
    else
    {
        DLLNodeBase *node = dllist_node_malloc(dllist);
        if(node == NULL)
            return false;
        node->data_size = min(len, dllist->node_data_capacity);
        memcpy(node->data, data, node->data_size);
        DLLNodeBase *prevNode = dllist->head;
        //新结点插入在prevNode的后面
        while(--index)
            prevNode = prevNode->next;
        node->prev = prevNode;
        node->next = prevNode->next;
        prevNode->next->prev = node;
        prevNode->next = node;
        dllist->length++;
        return true;
    }
}

bool dllist_index_delete(DLinkList *dllist, int index)
{
    if(dllist != NULL && !dllist_empty(dllist))
    {
        if(index < 1)
            return dllist_pop_front(dllist);
        else if(index >= dllist->length - 1)
            return dllist_pop_back(dllist);
        else 
        {
            DLLNodeBase *delNode = dllist->head;
            while(index--)
                delNode = delNode->next;
            delNode->prev->next = delNode->next;
            delNode->next->prev = delNode->prev;
            dllist_node_free(delNode);
            dllist->length--;
            return true;
        }
    }
    return false;
}

int dllist_key_index(DLinkList *dllist, Equal equal, const void *key)
{
    int index = 0, found = 0;
    if(dllist != NULL && equal != NULL && key != NULL)
    {
        for(DLLIter iter = dllist_begin(dllist);
            iter != dllist_end();
            iter = dllist_next(iter))
        {
            if(equal(key, iter->data))
            {
                found = 1;
                break;
            }
            index++;
        }
    }
    return found ? index : -1;
}

bool dllist_key_delete(DLinkList *dllist, Equal equal, const void *key)
{
    if(dllist != NULL && equal && key != NULL)
    {
        DLLIter iter = dllist_find_first(dllist, equal, key);
        return dllist_iter_delete(dllist, &iter);
    }
    return false;
}

/*
********************************************************************************
* Note : 若*iter == dllist_begin() 则插入最前面，相当于dllist_push_front()
*        若*iter == dllist_end() 则插入最后面，相当于dllist_push_back()
*        其它情况都是插入在*iter的前面
********************************************************************************
*/
bool dllist_iter_insert(DLinkList *dllist, DLLIter iter, const void *data, int len)
{
    if(dllist != NULL && data != NULL)
    {
        if(dllist_empty(dllist) || iter == dllist_begin(dllist))
            return dllist_push_front(dllist, data, len);
        else if(iter == dllist_end())
            return dllist_push_back(dllist, data, len);
        else
        {
            DLLNodeBase *node = dllist_node_malloc(dllist);
            if(node == NULL)
                return false;
            node->data_size = min(len, dllist->node_data_capacity);
            memcpy(node->data, data, node->data_size);
            node->prev = iter->prev;
            node->next = iter;
            iter->prev->next = node;
            iter->prev = node;
            dllist->length++;
            return true;
        }
    }
    return false;
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
bool dllist_iter_delete(DLinkList *dllist, DLLIter *iter)
{
    if(dllist != NULL && iter != NULL && *iter != dllist_end())
    {
        bool result = 0;
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
            DLLNodeBase *preNode = (*iter)->prev;
            DLLNodeBase *currNode = *iter;
            DLLNodeBase *nextNode = (*iter)->next;
            preNode->next = nextNode;
            nextNode->prev = preNode;
            dllist_node_free(currNode);
            *iter = nextNode;
            dllist->length--;
            return true;
        }
    }
    return false;
}

DLLIter dllist_find_first(DLinkList *dllist, Equal equal, const void *key)
{
    if(dllist != NULL && equal != NULL && key != NULL)
    {
        for(DLLIter iter = dllist_begin(dllist);
            iter != dllist_end();
            iter = dllist_next(iter))
        {
            if(equal(key, iter->data))
                return iter;
        }
    }
    return dllist_end();
}

DLLIter dllist_find_last(DLinkList *dllist, Equal equal, const void *key)
{
    if(dllist != NULL && equal != NULL && key != NULL)
    {
        for(RDLLIter riter = dllist_rbegin(dllist);
            riter != dllist_rend();
            riter = dllist_rnext(riter))
        {
            if(equal(key, riter->data))
                return riter;
        }
    }
    return dllist_rend();
}

int dllist_count(DLinkList *dllist, Equal equal, const void *key)
{
    int count = 0;
    if(dllist != NULL && equal != NULL && key != NULL)
    {
        for(DLLIter iter = dllist_begin(dllist);
            iter != dllist_end();
            iter = dllist_next(iter))
        {
            if(equal(key, iter->data))
                count++;
        }
    }
    return count;
}

void *dllist_at(DLinkList *dllist, int index, int *len)
{
    unsigned char *data = NULL;

    if(dllist != NULL && index >= 0 && index < dllist->length)
    {
        DLLIter iter = dllist_begin(dllist);
        while(index--)
            iter = dllist_next(iter);
        data = dllist_data(iter, len);
    }
    return data;
}

//快速找寻两个结点的中间结点
static DLLNodeBase *dllist_middle(DLLNodeBase *low, DLLNodeBase *high)
{
    DLLNodeBase *slow = low, *fast = low;

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

static DLLNodeBase *dllist_partition(DLinkList *dllist, DLLNodeBase *low, DLLNodeBase *high, Equal compare)
{
    //如果每次都选第一个点作为基准点，当链表已经有序时就会出现最坏情况。所以最好选择中间点作为基准点。
    //void *pivot = low->data; //不能总是选第一个结点作为基准点。
    DLLNodeBase *middleNode = dllist_middle(low, high);
    void *pivot = &(middleNode->data);
    memcpy(middleNode->data, low->data, dllist->node_data_capacity);
    while(low != high)
    {
        while(low != high && compare(&(high->data), pivot) >= 0)
            high = high->prev;
        if(low != high)
        {
            memcpy(low->data, high->data, dllist->node_data_capacity);
            low = low->next;
        }

        while(low != high && compare(&(low->data), pivot) <= 0)
            low = low->next;
        if(low != high)
        {
            memcpy(high->data, low->data, dllist->node_data_capacity);
            high = high->prev;
        }
    }
    memcpy(low->data, pivot, dllist->node_data_capacity);
    return low;
}

static void dllist_qsort_recursive(DLinkList *dllist, DLLNodeBase *low, DLLNodeBase *high, Equal compare)
{
    if(low != high)
    {
        DLLNodeBase *pivot = dllist_partition(dllist, low, high, compare);
        if(low != pivot && low != pivot->prev)
            dllist_qsort_recursive(dllist, low, pivot->prev, compare);
        if(high != pivot && high != pivot->next)
            dllist_qsort_recursive(dllist, pivot->next, high, compare);
    }
}

void dllist_quick_sort(DLinkList *dllist, int (*compare)(const void*, const void*))
{
    if(dllist != NULL && compare != NULL && dllist->length > 1)
        dllist_qsort_recursive(dllist, dllist->head, dllist->tail, compare);
}

static inline void dllist_append_node(DLLNodeBase **curr, DLLNodeBase **next)
{
    (*next)->prev = *curr;
    (*curr)->next = *next;
    *curr = *next;
    *next = (*next)->next;
}

//中间分割链表，并返回分割出的新链表
static DLLNodeBase *dllist_middle_split(DLLNodeBase *head)
{
    if(head == NULL) return NULL;
    DLLNodeBase *slow = head, *fast = head;
    while(fast != NULL && fast->next != NULL)
    {
        fast = fast->next->next;
        //当链表只有两个节点时，最终slow应该指向第一个节点。
        //这样就能将只有两个节点的链表拆分成两个分别有一个节点的链表。
        if(fast != NULL)
            slow = slow->next;
    }
    DLLNodeBase *newlist = slow->next;
    newlist->prev = NULL;
    slow->next = NULL;
    return newlist;
}

static DLLNodeBase *dllist_merge_recursive(DLLNodeBase *head, int (*compare)(const void*, const void*))
{
    if(head == NULL) return NULL;
    if(head->next == NULL) return head;

    DLLNodeBase *list1 = head;
    DLLNodeBase *list2 = dllist_middle_split(head);
    DLLNodeBase *left = dllist_merge_recursive(list1, compare);
    DLLNodeBase *right = dllist_merge_recursive(list2, compare);

    if(left == NULL)
        return right;
    else if(right == NULL)
        return left;

    DLLNodeBase headNode, *tailNode = &headNode;
    while(left != NULL && right != NULL)
    {
        if(compare(&(left->data), &(right->data)))
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
    if(dllist != NULL && compare != NULL)
    {
        dllist->head = dllist_merge_recursive(dllist->head, compare);
        DLLNodeBase *tail = dllist->head;
        for(; tail != NULL && tail->next != NULL; tail = tail->next)
            ;
        dllist->tail = tail;
    }
}
