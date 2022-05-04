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
#include <stdio.h>
#include "utility.h"
#include "static_dlinklist.h"

static DLLNodeBase *dllist_node_malloc(DLinkList *dllist)
{
    DLLNodeBase *node = NULL, *temp;
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
    return node;
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
        dllist->node_data_max = dllist->node_size - sizeof(DLLNodeBase) + 1;
        dllist->head = NULL;
        dllist->tail = NULL;
        dllist->length = 0;
    }
}

void dllist_clear(DLinkList *dllist)
{
    DLLNodeBase *node;
    if(dllist != NULL)
    {
        while(dllist->head != NULL)
        {
            node = dllist->head;
            dllist->head = node->next;
            dllist_node_free(node);
        }
        dllist->length = 0;
        dllist->tail = NULL;
    }
}

//data：数组首原素地址
//data_size: 数组长度
//element_cnt：数组中元素的数量
int dllist_assign(DLinkList *dllist, const void *array, int array_size, int element_cnt)
{   
    DLLNodeBase *node;
    int element_size;

    if(dllist == NULL || array == NULL)
    {
        return 0;
    }
    
    element_size = array_size / element_cnt;
    for(int i = 0; i < element_cnt; ++i)
    {
        node = dllist_node_malloc(dllist);
        if(node == NULL)
        {
            dllist->length += i;
            return i;
        }
        node->data_size = min(element_size, dllist->node_data_max);
        memcpy(node->data, (unsigned char *)array + i*node->data_size, node->data_size);
        node->prev = dllist->tail;
        node->next = NULL;
        if(dllist->tail == NULL)
        {
            dllist->head = dllist->tail = node;
        }
        else
        {
            dllist->tail->next = node;
        }
        dllist->tail = node;
    }
    dllist->length += element_cnt;
    return element_cnt;
}

bool dllist_push_front(DLinkList *dllist, const void *data, int len)
{
    DLLNodeBase *node;
    if(dllist != NULL && data != NULL)
    {
        node = dllist_node_malloc(dllist);
        if(node == NULL)
        {
            return false;
        }
        node->data_size = min(len, dllist->node_data_max);
        memcpy(node->data, data, node->data_size);
        node->prev = NULL;
        node->next = dllist->head;
        if(dllist->head != NULL)
        {
            dllist->head->prev = node;
        }
        else
        {
            dllist->tail = node;
        }
        dllist->head = node;
        dllist->length++;
        return true;
    }
    return false;
}

bool dllist_push_back(DLinkList *dllist, const void *data, int len)
{
    DLLNodeBase *node;
    if(dllist != NULL && data != NULL)
    {
        node = dllist_node_malloc(dllist);
        if(node == NULL)
        {
            return false;
        }
        node->data_size = min(len, dllist->node_data_max);
        memcpy(node->data, data, node->data_size);
        node->prev = dllist->tail;
        node->next = NULL;
        if(dllist->tail != NULL)
        {
            dllist->tail->next = node;
        }
        else
        {
            dllist->head = node;
        }
        dllist->tail = node;
        dllist->length++;
        return true;
    }
    return false;
}

bool dllist_pop_front(DLinkList *dllist)
{
    DLLNodeBase *head, *next;
    if(dllist != NULL && !dllist_empty(dllist))
    {
        head = dllist->head;
        next = head->next;
        dllist->head = next;
        if(next != NULL)
        {
            next->prev = NULL;
        }
        else
        {
            dllist->tail = NULL;
        }
        dllist_node_free(head);
        dllist->length--;
        return true;
    }
    return false;
}

bool dllist_pop_back(DLinkList *dllist)
{
    DLLNodeBase *tail, *prev;
    if(dllist != NULL && !dllist_empty(dllist))
    {
        tail = dllist->tail;
        prev = tail->prev;
        dllist->tail = prev;
        if(dllist->tail != NULL)
        {
            dllist->tail->next = NULL;
        }
        else
        {
            dllist->head = NULL;
        }
        dllist_node_free(tail);
        dllist->length--;
        return true;
    }
    return false;
}

bool dllist_index_insert(DLinkList *dllist, int index, const void *data, int len)
{
    DLLNodeBase *prevNode, *node;

    if(dllist == NULL || data == NULL)
    {
        return false;
    }

    if(dllist_empty(dllist) || index <= 0)
    {
        return dllist_push_front(dllist, data, len);
    }
    else if(index >= dllist->length - 1)
    {
        return dllist_push_back(dllist, data, len);
    }
    else
    {
        prevNode = dllist->head;
        node = dllist_node_malloc(dllist);
        if(node == NULL)
        {
            return false;
        }
        node->data_size = min(len, dllist->node_data_max);
        memcpy(node->data, data, node->data_size);
        //新结点插入在prevNode的后面
        while(--index)
        {
            prevNode = prevNode->next;
        }
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
    DLLNodeBase *node;

    if(dllist != NULL && !dllist_empty(dllist))
    {
        if(index <= 0)
        {
            return dllist_pop_front(dllist);
        }
        else if(index >= dllist->length - 1)
        {
            return dllist_pop_back(dllist);
        }
        else 
        {
            node = dllist->head;
            while(index--)
            {
                node = node->next;
            }
            node->prev->next = node->next;
            node->next->prev = node->prev;
            dllist_node_free(node);
            dllist->length--;
            return true;
        }
    }
    return false;
}

int dllist_key_index(DLinkList *dllist, EqualFun equal, const void *key)
{
    int index = 0;
    bool found = false;
    if(dllist != NULL && equal != NULL && key != NULL)
    {
        for(DLLIter iter = dllist_begin(dllist);
            iter != dllist_end();
            iter = dllist_next(iter))
        {
            if(equal(key, iter->data))
            {
                found = true;
                break;
            }
            index++;
        }
    }
    return found ? index : -1;
}

bool dllist_key_delete(DLinkList *dllist, EqualFun equal, const void *key)
{
    if(dllist != NULL && equal && key != NULL)
    {
        DLLIter iter = dllist_find_first(dllist, equal, key);
        return dllist_iter_delete(dllist, iter);
    }
    return false;
}

bool dllist_iter_insert_front(DLinkList *dllist, DLLIter iter, const void *data, int len)
{
    DLLNodeBase *node;

    if(dllist != NULL && data != NULL)
    {
        if(iter == NULL || iter == dllist->head)
        {
            return dllist_push_front(dllist, data, len);
        }
        else
        {
            node = dllist_node_malloc(dllist);
            if(node == NULL)
            {
                return false;
            }
            node->data_size = min(len, dllist->node_data_max);
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

bool dllist_iter_insert_back(DLinkList *dllist, DLLIter iter, const void *data, int len)
{
    DLLNodeBase *node;

    if(dllist != NULL && data != NULL)
    {
        if(iter == NULL || iter == dllist->tail)
        {
            return dllist_push_back(dllist, data, len);
        }
        else
        {
            node = dllist_node_malloc(dllist);
            if(node == NULL)
            {
                return false;
            }
            node->data_size = min(len, dllist->node_data_max);
            memcpy(node->data, data, node->data_size);
            node->prev = iter;
            node->next = iter->next;
            iter->next->prev = node;
            iter->next = node;
            dllist->length++;
            return true;
        }
    }
    return false;
}

/*
********************************************************************************
* Note : 删除成功后会使返回前一个迭代器, 使用示例：
    for(DLLIter iter; iter != dllist_end(); iter = dllist_next(iter))
    {
        if(*(int *)dllist_data(iter, NULL) == 5)
        {
            iter = dllist_delete(iter);
        }
    }
********************************************************************************
*/
DLLIter dllist_iter_delete(DLinkList *dllist, DLLIter iter)
{
    DLLIter prev = NULL;
    DLLNodeBase *prevNode, *currNode, *nextNode;
    
    if(dllist != NULL && iter != NULL)
    {
        prev = iter->prev;
        
        if(iter == dllist_begin(dllist))
        {
            if(dllist_pop_front(dllist) != 0)
            {
                iter = dllist_begin(dllist);
            }
        }
        else if(dllist_next(iter) == dllist_end())
        {
            if(dllist_pop_back(dllist) != 0)
            {
                iter = dllist_end();
            }
        }
        else
        {
            prevNode = iter->prev;
            currNode = iter;
            nextNode = iter->next;
            
            prevNode->next = nextNode;
            nextNode->prev = prevNode;
            dllist_node_free(currNode);
            dllist->length--;
        }
    }
    return prev;
}

/*
********************************************************************************
* Note : 删除成功后会使返回前一个迭代器。
         cnt < 0 则删除iter及后面所有的节点
         cnt == 0 不删除任何节点，返回iter
         cnt > 0 删除n个节点
********************************************************************************
*/
DLLIter dllist_iter_erase(DLinkList *dllist, DLLIter iter, int cnt)
{
    DLLIter prev = iter;
    
    if(dllist != NULL && iter != NULL)
    {
        if(cnt < 0)
        {
            prev = iter->prev;
            for(; iter != dllist_end(); iter = dllist_next(iter))
            {
                iter = dllist_iter_delete(dllist, iter);
            }
        }
        else if(cnt > 0)
        {
            prev = iter->prev;
            for(int i = 0;
                i < cnt && iter != dllist_end();
                ++i, iter = dllist_next(iter))
            {
                iter = dllist_iter_delete(dllist, iter);
            }
        }
    }
    
    return prev;
}

DLLIter dllist_find_first(DLinkList *dllist, EqualFun equal, const void *key)
{
    if(dllist != NULL && equal != NULL && key != NULL)
    {
        for(DLLIter iter = dllist_begin(dllist);
            iter != dllist_end();
            iter = dllist_next(iter))
        {
            if(equal(key, iter->data))
            {
                return iter;
            }
        }
    }
    return dllist_end();
}

DLLIter dllist_find_last(DLinkList *dllist, EqualFun equal, const void *key)
{
    if(dllist != NULL && equal != NULL && key != NULL)
    {
        for(DLLIter riter = dllist_rbegin(dllist);
            riter != dllist_end();
            riter = dllist_prev(riter))
        {
            if(equal(key, riter->data))
            {
                return riter;
            }
        }
    }
    return dllist_end();
}

int dllist_count(DLinkList *dllist, EqualFun equal, const void *key)
{
    int count = 0;
    if(dllist != NULL && equal != NULL && key != NULL)
    {
        for(DLLIter iter = dllist_begin(dllist);
            iter != dllist_end();
            iter = dllist_next(iter))
        {
            if(equal(key, iter->data))
            {
                count++;
            }
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
        {
            iter = dllist_next(iter);
        }
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

static DLLNodeBase *dllist_partition(DLinkList *dllist, DLLNodeBase *low, DLLNodeBase *high, CmpFun compare)
{
    //如果每次都选第一个点作为基准点，当链表已经有序时就会出现最坏情况。所以最好选择中间点作为基准点。
    //void *pivot = low->data; //不能总是选第一个结点作为基准点。
    DLLNodeBase *pivot, *middle;

    pivot = dllist_node_malloc(dllist);
    if(pivot == NULL)
    {
        return low;
    }

    middle = dllist_middle(low, high);
    memcpy(&pivot->data, &middle->data, middle->data_size);
    memcpy(&middle->data, &low->data, low->data_size);
    
    while(low != high)
    {
        while(low != high && compare(&high->data, &pivot->data) >= 0)
        {
            high = high->prev;
        }
        
        if(low != high)
        {
            memcpy(&low->data, &high->data, high->data_size);
            low = low->next;
        }

        while(low != high && compare(&low->data, &pivot->data) <= 0)
        {
            low = low->next;
        }
        
        if(low != high)
        {
            memcpy(&high->data, &low->data, low->data_size);
            high = high->prev;
        }
    }
    
    memcpy(&low->data, &pivot->data, pivot->data_size);
    dllist_node_free(pivot);

    return low;
}

static void dllist_qsort_recursive(DLinkList *dllist, DLLNodeBase *low, DLLNodeBase *high, CmpFun compare)
{
    DLLNodeBase *pivot;

    if(low != high)
    {
        pivot = dllist_partition(dllist, low, high, compare);
        
        if(low != pivot && low != pivot->prev)
        {
            dllist_qsort_recursive(dllist, low, pivot->prev, compare);
        }
        
        if(high != pivot && high != pivot->next)
        {
            dllist_qsort_recursive(dllist, pivot->next, high, compare);
        }
    }
}

void dllist_quick_sort(DLinkList *dllist, CmpFun compare)
{
    if(dllist != NULL && compare != NULL && dllist->length > 1)
    {
        dllist_qsort_recursive(dllist, dllist->head, dllist->tail, compare);
    }
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
    DLLNodeBase *slow = head, *fast = head, *newlist;

    if(head == NULL) 
    {
        return NULL;
    }
    
    while(fast != NULL && fast->next != NULL)
    {
        fast = fast->next->next;
        //当链表只有两个节点时，最终slow应该指向第一个节点。
        //这样就能将只有两个节点的链表拆分成两个分别有一个节点的链表。
        if(fast != NULL)
        {
            slow = slow->next;
        }
    }
    newlist = slow->next;
    newlist->prev = NULL;
    slow->next = NULL;
    return newlist;
}

static DLLNodeBase *dllist_merge_recursive(DLLNodeBase *head, int (*compare)(const void*, const void*))
{
    if(head == NULL)
    {
        return NULL;
    }
    
    if(head->next == NULL)
    {
        return head;
    }

    DLLNodeBase *list1 = head;
    DLLNodeBase *list2 = dllist_middle_split(head);
    DLLNodeBase *left = dllist_merge_recursive(list1, compare);
    DLLNodeBase *right = dllist_merge_recursive(list2, compare);

    if(left == NULL)
    {
        return right;
    }
    else if(right == NULL)
    {
        return left;
    }

    DLLNodeBase headNode, *tailNode = &headNode;
    while(left != NULL && right != NULL)
    {
        if(compare(&left->data, &right->data) <= 0)
        {
            dllist_append_node(&tailNode, &left);
        }
        else
        {
            dllist_append_node(&tailNode, &right);
        }
    }
    
    if(left != NULL && tailNode != NULL)
    {
        dllist_append_node(&tailNode, &left);
    }
    
    if(right != NULL && tailNode != NULL)
    {
        dllist_append_node(&tailNode, &right);
    }
    headNode.next->prev = NULL;
    return headNode.next;
}

void dllist_merge_sort(DLinkList *dllist, int (*compare)(const void*, const void*))
{
    DLLNodeBase *tail;
    if(dllist != NULL && compare != NULL)
    {
        dllist->head = dllist_merge_recursive(dllist->head, compare);
        tail = dllist->head;
        for(; tail != NULL && tail->next != NULL; tail = tail->next)
        {
            ;
        }
        dllist->tail = tail;
    }
}
