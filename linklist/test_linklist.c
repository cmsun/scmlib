/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : main.cpp
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/05/12 21:35:55
*  @Last Modified  : 2017/06/21 11:59:33
********************************************************************************
*/

//编译指令：gcc linklist.c test_linklist.c -O3

#include <stdio.h>
#include <time.h>
#include "linklist.h"

int equal(const void *data1, const void *data2)
{
    return *(int *)data1 == *(int *)data2;
}

void show(void *data, void *param)
{
    printf("%d ", *(int *)data);
}

void print(LinkList *llist)
{
    /* llist_traversal(llist->head, show, NULL); */
    for(LLIter iter = llist_begin(llist); iter != llist_end(); iter = llist_next(iter))
        printf("%d ", *(int *)llist_data(iter));
    printf("\n");
}

int less(const void *data1, const void *data2)
{
    return *(int*)data1 < *(int *)data2;
}

int main(int argc, char *argv[])
{
    clock_t t = clock();

    int array[10];
    LinkList llist;
    llist_init(&llist, sizeof(int));

    printf("------------Test:llist_push_front llist_push_back-----------------\n");
    for(int i = 0; i < sizeof(array)/sizeof(array[0]); ++i)
    {
        array[i] = i;
        /* llist_push_front(&llist, &array[i]); */
        llist_push_back(&llist, &array[i]);
    } 
    print(&llist);

    printf("----------------Test:llist_front llist_back llist_at-------------\n");
    printf("front:%d\nback:%d\n", *(int *)llist_front(&llist), *(int *)llist_back(&llist));
    printf("at 0:%d\n", *(int *)llist_at(&llist, 0));
    printf("at 3:%d\n", *(int *)llist_at(&llist, 3));
    printf("at end:%d\n", *(int *)llist_at(&llist, llist_length(&llist)-1));

    printf("-------------Test:dllist_pop_front dllist_pop_back------------------\n");
    llist_pop_front(&llist);
    llist_pop_back(&llist);
    print(&llist);

    llist_clear(&llist);
    for(int i = 0; i < sizeof(array)/sizeof(array[0]); ++i)
        llist_push_back(&llist, &array[i]);

    printf("--------------------------Test:llist_key_index---------------------\n");
    print(&llist);
    int key_0 = 0, key_5 = 5, key_9 = 9, key_10 = 10;
    printf("index of 0: %d\n", llist_key_index(&llist, equal, &key_0));
    printf("index of 5: %d\n", llist_key_index(&llist, equal, &key_5));
    printf("index of 9: %d\n", llist_key_index(&llist, equal, &key_9));
    printf("index of 10: %d\n", llist_key_index(&llist, equal, &key_10));
    printf("-------------------------Test:llist_index_insert---------------------\n");
    int value_n1 = -1, value_n2 = -2, value_n5 = -5, value_n9 = -9;
    llist_index_insert(&llist, 0, &value_n1);
    llist_index_insert(&llist, 3, &value_n2);
    llist_index_insert(&llist, llist_length(&llist)-1, &value_n5);
    llist_index_insert(&llist, llist_length(&llist), &value_n9);
    print(&llist);
    printf("-------------------------Test:llist_index_delete---------------------\n");
    llist_index_delete(&llist, 3);
    llist_index_delete(&llist, 0);
    llist_index_delete(&llist, llist_length(&llist)-1);
    llist_index_delete(&llist, llist_length(&llist)-2);
    print(&llist);

    printf("-------------------------Test:llist_iter_insert---------------------\n");
    int value_0 = 0, value_2 = 2, value_5 = 5, value_9 = 9, value_10000 = 10000;
    for(LLIter iter = llist_begin(&llist); iter != llist_end(); iter = llist_next(iter))
    {
        if(equal(&value_0, llist_data(iter)))
            llist_iter_insert(&llist, &iter, &value_n1);
        if(equal(&value_2, llist_data(iter)))
            llist_iter_insert(&llist, &iter, &value_n2);
        if(equal(&value_5, llist_data(iter)))
            llist_iter_insert(&llist, &iter, &value_n5);
        if(equal(&value_9, llist_data(iter)))
            llist_iter_insert(&llist, &iter, &value_n9);
    }
    LLIter iter = llist_end();
    llist_iter_insert(&llist, &iter, &value_10000);
    print(&llist);

    printf("-------------------------Test:llist_iter_delete---------------------\n");
    for(LLIter iter = llist_begin(&llist); iter != llist_end(); iter = llist_next(iter))
    {
        if(equal(&value_n1, llist_data(iter)))
            llist_iter_delete(&llist, &iter);
        if(equal(&value_n2, llist_data(iter)))
            llist_iter_delete(&llist, &iter);
        if(equal(&value_n5, llist_data(iter)))
            llist_iter_delete(&llist, &iter);
        if(equal(&value_n9, llist_data(iter)))
            llist_iter_delete(&llist, &iter);
        if(equal(&value_10000, llist_data(iter)))
            llist_iter_delete(&llist, &iter);
    }
    print(&llist);

    printf("-------------Test:llist_find_first llist_find_last llist_count-------\n");
    llist_push_front(&llist, &value_5);
    llist_push_front(&llist, &value_5);
    llist_push_front(&llist, &value_5);
    llist_push_back(&llist, &value_5);
    llist_push_back(&llist, &value_5);
    iter = llist_find_first(&llist, equal, &value_5);
    llist_iter_insert(&llist, &iter, &value_10000);
    iter = llist_find_last(&llist, equal, &value_5);
    llist_iter_insert(&llist, &iter, &value_10000);
    int count = llist_count(&llist, equal, &value_5);
    printf("count of 5:%d\n", count);
    print(&llist);

    printf("-------------Test:llist_key_delete-------\n");
    for(int i = 0; i < count; ++i)
        llist_key_delete(&llist, equal, &value_5);
    llist_key_delete(&llist, equal, &value_10000);
    llist_key_delete(&llist, equal, &value_10000);
    print(&llist);

    printf("-------------Test:time-------\n");
    //当前数据量非常大时，比如10000个数据时，llist_push_back和llist_pop_back会
    //使单链表的速度变得非常慢。而双链表却依然很快
    printf("time:%ld\n", clock() - t);
    return 0;
}
