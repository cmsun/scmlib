/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : linklist.c
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/05/12 14:35:54
*  @Last Modified  : 2017/07/18 16:04:42
********************************************************************************
*/

#include <stdlib.h>
#include <string.h>
#include "linklist.h"

void llist_init(LinkList *llist, int type_size)
{
    assert(llist != NULL);
    if(llist != NULL)
    {
        llist->type_size = type_size;
        llist->length = 0;
        llist->head = NULL;
    }
}

void llist_clear(LinkList *llist)
{
    assert(llist != NULL);
    if(llist != NULL)
    {
        while(llist->head != NULL)
        {
            LLNode *temp = llist->head;
            llist->head = temp->next;
            free(temp->data);
            free(temp);
        }
        llist->length = 0;
    }
}

int llist_assign(LinkList *llist, const void *data, int count)
{
    assert(llist != NULL && data != NULL && count > 0);
    if(llist == NULL || data == NULL || count <= 0)
        return 0;
    LLNode **tail = &llist->head;
    for(int i = 0; i < count; ++i)
    {
        LLNode *node = (LLNode *)malloc(sizeof(LLNode));
        if(node == NULL)
            goto ERROR;
        if((node->data = malloc(llist_type_size(llist))) == NULL)
        {
            free(node);
            goto ERROR;
        }
        memcpy(node->data, (char *)data + i * llist->type_size, llist->type_size);
        node->next = NULL;
        *tail = node;
        tail = &node->next;
    }
    llist->length = count;
    return 1;
ERROR:
    llist_clear(llist);
    return 0;
}

int llist_push_back(LinkList *llist, const void *data)
{
    assert(llist != NULL && data != NULL);
    if(llist != NULL && data != NULL)
    {
        LLNode *node = (LLNode *)malloc(sizeof(LLNode)); 
        if(node == NULL)
            return 0;
        if((node->data = malloc(llist->type_size)) == NULL)
        {
            free(node);
            return 0;
        }
        memcpy(node->data, data, llist->type_size);
        node->next = NULL;
        LLNode **tail = &llist->head;
        while(*tail != NULL)
            tail = &(*tail)->next;
        *tail = node;
        ++llist->length;
        return 1;
    }
    return 0;
}

int llist_push_front(LinkList *llist, const void *data)
{
    assert(llist != NULL && data != NULL);
    if(llist != NULL && data != NULL)
    {
        LLNode *node = (LLNode *)malloc(sizeof(LLNode)); 
        if(node == NULL)
            return 0;
        if((node->data = malloc(llist->type_size)) == NULL)
        {
            free(node);
            return 0;
        }
        memcpy(node->data, data, llist->type_size);
        node->next = llist->head;
        llist->head = node;
        ++llist->length;
        return 1;
    }
    return 0;
}

int llist_pop_back(LinkList *llist)
{
    assert(llist != NULL);
    if(llist != NULL && !llist_empty(llist))
    {
        LLNode **tail = &llist->head; 
        while((*tail)->next != NULL)
            tail = &((*tail)->next);
        free((*tail)->data);
        free(*tail);
        *tail = NULL;
        --llist->length;
        return 1;
    }
    return 0;
}

int llist_pop_front(LinkList *llist)
{
    assert(llist != NULL);
    if(llist != NULL && !llist_empty(llist))
    {
        LLNode *temp = llist->head;
        llist->head = temp->next;
        free(temp->data);
        free(temp);
        --llist->length;
        return 1;
    }
    return 0;
}

int llist_index_insert(LinkList *llist, int index, const void *data)
{
    assert(llist != NULL && data != NULL);

    if(llist == NULL || data == NULL)
        return 0;

    if(index < 0) index = 0;

    LLNode *node = (LLNode *)malloc(sizeof(LLNode)); 
    if(node == NULL)
        return 0;
    if((node->data = malloc(llist->type_size)) == NULL)
    {
        free(node);
        return 0;
    }
    memcpy(node->data, data, llist->type_size);
    LLNode **link = &llist->head;
    while(index-- && *link != NULL)
        link = &(*link)->next;
    node->next = *link;
    *link = node;
    ++llist->length;
    return 1;
}

int llist_index_delete(LinkList *llist, int index)
{
    assert(llist != NULL);
    if(llist != NULL && !llist_empty(llist))
    {
        if(index < 1)
            return llist_pop_front(llist);
        else if(index >= llist->length - 1)
            return llist_pop_back(llist);
        else
        {
            LLNode **link = &llist->head;
            while(index--)
                link = &(*link)->next;
            LLNode *delNode = *link;
            *link = delNode->next;
            free(delNode->data);
            free(delNode);
            --llist->length;
            return 1;
        }
    }
    return 0;
}

int llist_key_index(LinkList *llist, int (*equal)(const void *, const void*), const void *key)
{
    assert(llist != NULL && equal != NULL && key != NULL);
    int index = 0, found = 0;
    if(llist != NULL && equal != NULL && key != NULL)
    {
        for(LLNode *tail = llist->head; tail != NULL; tail = tail->next)
        {
            if(equal(key, tail->data))
            {
                found = 1;
                break;
            }
            ++index;
        }
    }
    return found ? index : -1;
}

int llist_key_delete(LinkList *llist, int (*equal)(const void*, const void*), const void *key)
{
    assert(llist != NULL && equal != NULL && key != NULL);
    if(llist != NULL && equal != NULL && key != NULL)
    {
        LLIter iter = llist_find_first(llist, equal, key);
        return llist_iter_delete(llist, &iter);
    }
    return 0;
}

static void inline llist_swap(LLNode *node1, LLNode *node2)
{
    void *temp = node1->data;
    node1->data = node2->data;
    node2->data = temp;
}

int llist_iter_insert(LinkList *llist, LLIter *iter, const void *data)
{
    assert(llist != NULL && data != NULL);
    if(llist != NULL && data != NULL)
    {
        if(llist_empty(llist) || *iter == llist_begin(llist))
            return llist_push_front(llist, data);
        else if(*iter == llist_end())
            return llist_push_back(llist, data);
        else
        {
            LLNode *node = (LLNode *)malloc(sizeof(LLNode)); 
            if(node == NULL)
                return 0;
            if((node->data = malloc(llist->type_size)) == NULL)
            {
                free(node);
                return 0;
            }
            memcpy(node->data, data, llist->type_size);
            //把新结点加在当前结点后面，然后交换当前节点和新节点的数据，
            //这就相当于把新节点加在当前结点前面。
            LLNode *currNode = *iter;
            node->next = currNode->next;
            currNode->next = node;
            llist_swap(currNode, node);
            //插入操作后迭代器应该保持不变，但是由于交换了前后结点的数据，
            //也就相当于迭代器指向了前一个结点。必须使它指向原来数据的结点。
            *iter = node;
            ++llist->length;
            return 1;
        }
    }
    return 0;
}

/*
********************************************************************************
* Note : *iter != llist_end() 时删除*iter对应的节点，否则不删除。
*        原因是llist_find_first()和llist_find_last()在找不到应对的数剧时会返回
*        llist_end()，如果这时候删除就会删除错误。 
*        删除成功后会使迭代器自动向后走一个节点，如果在循环中使用迭代进行删除，则
*        判断删除成功后要使用goto返回前面的操作，否则会错过下个节点的操作。
********************************************************************************
*/
int llist_iter_delete(LinkList *llist, LLIter *iter)
{
    assert(llist != NULL && iter != NULL);

    if(llist == NULL || iter == NULL || *iter == llist_end())
        return 0;

    int result = 0;
    if(*iter == llist_begin(llist))
    {
        if((result = llist_pop_front(llist)) != 0)
            *iter = llist_begin(llist);
        return result;
    }
    else if(llist_next(*iter) == llist_end())
    {
        if((result = llist_pop_back(llist)) != 0)
            *iter = llist_end();
        return result;
    }
    else
    {
        //把下个结点的数据和next指针复制到当前结点，然后删除下个结点。这相当于删除当前结点。
        //由于当前结点变成了下个结点，所以相当于iter自动后移了一个结点。
        LLNode *currNode = *iter;
        LLNode *nextNode = (*iter)->next;
        llist_swap(currNode, nextNode);
        currNode->next = nextNode->next;
        free(nextNode->data);
        free(nextNode);
        --llist->length; 
        return 1;
    }
}

LLIter llist_find_first(LinkList *llist, int (*equal)(const void *, const void*), const void *key)
{
    assert(llist != NULL && equal != NULL && key != NULL);
    if(llist != NULL && equal != NULL && key != NULL)
    {
        for(LLIter iter = llist_begin(llist);
            iter != llist_end();
            iter = llist_next(iter))
        {
            if(equal(key, llist_data(iter)))
                return iter;
        }
    }
    return llist_end();
}

LLIter llist_find_last(LinkList *llist, int (*equal)(const void *, const void*), const void *key)
{
    assert(llist != NULL && equal != NULL && key != NULL);
    LLIter last = llist_end();
    if(llist != NULL && equal != NULL && key != NULL)
    {
        for(LLIter iter = llist_begin(llist);
            iter != llist_end();
            iter = llist_next(iter))
        {
            if(equal(key, llist_data(iter)))
                last = iter;
        }
    }
    return last;
}

int llist_count(LinkList *llist, int (*equal)(const void*, const void*), const void *key)
{
    assert(llist != NULL && equal != NULL && key != NULL);
    int count = 0;
    if(llist != NULL && equal != NULL && key != NULL)
    {
        for(LLNode *tail = llist->head; tail != NULL; tail = tail->next)
        {
            if(equal(tail->data, key))
                ++count;
        }
    }
    return count;
}

void *llist_at(LinkList *llist, int index)
{
    assert(llist != NULL);
    if(llist != NULL && !llist_empty(llist) && 
        index >= 0 && index < llist->length)
    {
        LLNode *node = llist->head;
        while(index--)
            node = node->next;
        return node->data;
    }
    return NULL;
}

void *llist_back(LinkList *llist)
{
    assert(llist != NULL);
    if(llist != NULL && !llist_empty(llist))
    {
        LLNode *tail = llist->head;
        while(tail->next != NULL)
            tail = tail->next;
        return tail->data;
    }
    return NULL;
}

void llist_traversal(LLNode *node, void (*visit)(void *, void *), void *param)
{
    assert(node != NULL && visit != NULL);
    if(node != NULL && visit != NULL)
        for(LLNode *n = node; n != NULL; n = n->next)
            visit(n->data, param);
}

static void llist_rtraversal(LLNode *node, void (*visit)(void *, void *), void *param)
{
    llist_reverse_traversal(node->next, visit, param);
    visit(node->data, param);
}

void llist_reverse_traversal(LLNode *node, void (*visit)(void *, void *), void *param)
{
    assert(node != NULL && visit != NULL);
    if(node != NULL && visit != NULL)
        llist_rtraversal(node->next, visit, param);
}

/*
********************************************************************************
* Note : llist_reverse方法一。 
********************************************************************************
*/
void llist_reverse(LinkList *llist)
{
    if(llist != NULL)
    {
        LLNode *prev = NULL;
        LLNode *curr = llist->head;
        while(curr != NULL)
        {
            LLNode *next = curr->next;
            curr->next = prev;
            prev = curr;
            curr = next;
        }
        llist->head = prev;
    }
}

/*
********************************************************************************
*                               llist_reverse 
*
* Description : 反转链表
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)     : 方法二：从旧链表中不断取第一个节点插入新链表的0位置，旧链表头指
*                       向下一个节点。
********************************************************************************
*/
/*
 * void llist_reverse(LinkList *llist)
 * {
 *     if(llist != NULL)
 *     {
 *         LLNode *newlist, *temp;
 *         newlist = NULL;
 *         while(llist->head != NULL)
 *         {
 *             temp = llist->head;
 *             llist->head = llist->head->next;
 *             temp->next = newlist;
 *             newlist = temp;
 *         }
 *         llist->head = newlist;
 *     }
 * }
 */

void llist_selection_sort(LinkList *llist, int (*compare)(const void*, const void*))
{
    assert(llist != NULL && compare != NULL);
    if(llist != NULL && compare != NULL && llist->length > 1)
    {
        LLNode *p1 = llist->head;
        while(p1->next != NULL)
        {
            LLNode *temp = p1;
            LLNode *p2 = p1->next;
            while(p2 != NULL)
            {
                if(compare(temp->data, p2->data))
                {
                    temp = p2;
                }
                p2 = p2->next;
            }
            if(temp != p1)
                llist_swap(p1, temp);
            p1 = p1->next;
        }
    }
}

void llist_bubble_sort(LinkList *llist, int (*compare)(const void*, const void*))
{
    assert(llist != NULL && compare != NULL);
    if(llist != NULL && compare != NULL && llist->length > 1)
    {
        LLNode *p = llist->head;
        LLNode *end = NULL;
        int change = 1;
        while(p->next != end && change)
        {
            change = 0;
            while(p->next != end)
            {
                if(compare(p->data, p->next->data))
                {
                    llist_swap(p, p->next);
                    change = 1;
                }
                p = p->next;
            }
            end = p;
            p = llist->head;
        }
    }
}

static LLNode *llist_middle(LLNode *left, LLNode *right)
{
    LLNode *slow = left, *fast = left;

    /*
     * //如果有两个节点，则返回第1个节点
     * while(fast != right)
     * {
     *     if((fast = fast->next) != right)
     *         if((fast = fast->next) != right)
     *             slow = slow->next;
     * }
     * return slow;
     */

    //如果有两个节点，则返回第2个节点
    while(fast != right && fast->next != right)
    {
        fast = fast->next->next;
        slow = left->next;
    }
    return slow;
}

static LLNode *llist_partition(LLNode *left, LLNode *right, int (*compare)(const void*, const void*))
{
    LLNode *middle = llist_middle(left, right);
    llist_swap(left, middle);
    LLNode *pivot = left;
    LLNode *small = left;
    LLNode *index = small->next;
    while(index != right->next)
    {
        if(compare(index->data, pivot->data))
        {
            small = small->next;
            if(small != index)
                llist_swap(small, index);
        }
        index = index->next;
    }
    llist_swap(pivot, small);
    return small;
}

static void llist_qsort_recursive(LLNode *left, LLNode *right, int (*compare)(const void*, const void*))
{
    if(left != right)
    {
        LLNode *pivot = llist_partition(left, right, compare);
        if(left != pivot && left->next != pivot)
            llist_qsort_recursive(left, pivot, compare);
        if(pivot != right && pivot->next != right)
            llist_qsort_recursive(pivot->next, right, compare);
    }
}

void llist_quick_sort(LinkList *llist, int (*compare)(const void*, const void*))
{
    if(llist->length < 2)
        return;
    LLNode *head = llist->head, *tail = llist->head;
    while(tail->next != NULL)
        tail = tail->next;
    llist_qsort_recursive(head, tail, compare);
}

/*
 * static inline LLNode *llist_forward(LLNode *node, int steps)
 * {
 *     while(node != NULL && steps--)
 *         node = node->next;
 *     return node;
 * }
 * 
 * static inline void llist_node_append_node(LLNode **curr, LLNode **next)
 * {
 *     (*curr)->next = *next;
 *     *curr = *next;
 *     *next = (*next)->next;
 * }
 * 
 * static LLNode *llist_merge(LinkList *llist, int k, int (*compare)(const void*, const void*))
 * {
 *     LLNode mergeHead;
 *     LLNode *mergeTail = &mergeHead;
 *     LLNode *i = llist->head;
 *     while(i != NULL) 
 *     {
 *         LLNode *i_end = llist_forward(i, k);
 *         LLNode *j = i_end;
 *         LLNode *j_end = llist_forward(j, k);
 *         while(i != i_end && j != j_end)
 *         {
 *             if(compare(i->data, j->data))
 *                 llist_node_append_node(&mergeTail, &i);
 *             else
 *                 llist_node_append_node(&mergeTail, &j);
 *         }
 *         while(i != i_end)
 *             llist_node_append_node(&mergeTail, &i);
 *         while(j != j_end)
 *             llist_node_append_node(&mergeTail, &j);
 *         i = j_end;
 *     }
 *     mergeTail->next = NULL;
 *     return mergeHead.next;
 * }
 * 
 * void llist_merge_sort(LinkList *llist, int (*compare)(const void*, const void*))
 * {
 *     if(llist != NULL && compare != NULL)
 *     {
 *         for(int k = 1; k < llist_length(llist); k *= 2)
 *         {
 *             llist->head = llist_merge(llist, k, compare);
 *         }
 *     }
 * }
 */

//中间分割链表，并返回分割出的新链表
static LLNode *llist_middle_split(LLNode *head)
{
    if(head == NULL || head->next == NULL)
        return NULL;
    LLNode *slow = head, *fast = head;
    while(fast != NULL)
    {
        if((fast = fast->next) != NULL)
            if((fast = fast->next) != NULL)
                slow = slow->next;
    }
    LLNode *newlist = slow->next;
    slow->next = NULL;
    return newlist;
}

static LLNode *llist_merge_recursive(LLNode *node, int (*compare)(const void*, const void*))
{
    if(node == NULL) return NULL;
    if(node->next == NULL) return node;

    LLNode *list1 = node;
    LLNode *list2 = llist_middle_split(node);
    LLNode *left = llist_merge_recursive(list1, compare);
    LLNode *right = llist_merge_recursive(list2, compare);
    LLNode mergeHead, *mergeTail = &mergeHead;
    while(left != NULL && right != NULL)
    {
        if(compare(left->data, right->data))
        {
            mergeTail->next = left;
            mergeTail = left;
            left = left->next;
        }
        else
        {
            mergeTail->next = right;
            mergeTail = right;
            right = right->next;
        }
    }
    if(left != NULL)
        mergeTail->next = left;
    if(right != NULL)
        mergeTail->next = right;
    return mergeHead.next;
}

void llist_merge_sort(LinkList *llist, int (*compare)(const void*, const void*))
{
    assert(llist != NULL || compare != NULL);
    if(llist != NULL || compare != NULL)
    {
        llist->head = llist_merge_recursive(llist->head, compare);
    }
}
