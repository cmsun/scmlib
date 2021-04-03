/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : main.cpp
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/05/12 21:35:55
*  @Last Modified  : 2017/06/21 11:59:11
********************************************************************************
*/

//编译指令：gcc static_dlinklist.c test_static_dlinklist.c -O3

#include <stdio.h>
#include <time.h>
#include "static_dlinklist.h"

#define DataMaxSize sizeof(int)
struct
{
    DLLNodeBase base;
    uint8_t data[DataMaxSize-1];
} Nodes[10000];

DLLNodeBase *test_node_malloc(void)
{
    DLLNodeBase *node = NULL;
    for(int i = 0; i < sizeof(Nodes)/sizeof(Nodes[0]); ++i)
    {
        if(!Nodes[i].base.in_use)
        {
            node = &Nodes[i];
            Nodes[i].base.in_use = true;
            break;
        }
    }
    return node;
}

void test_node_free(DLLNodeBase *node)
{
    node->in_use = false;
}

int equal(const void *data1, const void *data2)
{
    return *(int *)data1 == *(int *)data2;
}

void show(void *data, void *param)
{
    printf("%d ", *(int *)data);
}

void print(DLinkList *dllist)
{
    for(DLLIter iter = dllist_begin(dllist); iter != dllist_end(); iter = dllist_next(iter))
        printf("%d ", *(int *)dllist_data(iter, NULL));
    printf("\n");
}

int less(const void *data1, const void *data2)
{
    return *(int*)data1 < *(int *)data2;
}

int main(int argc, char *argv[])
{
    clock_t t = clock();

    int array[20];
    DLinkList dllist;
    dllist_init(&dllist, DataMaxSize, test_node_malloc, test_node_free);

    printf("------------Test:dllist_push_front dllist_push_back-----------------\n");
    for(int i = 0; i < sizeof(array)/sizeof(array[0]); ++i)
    {
        array[i] = i;
        dllist_push_front(&dllist, &array[i], sizeof(array[0]));
        //dllist_push_back(&dllist, &array[i], sizeof(array[0]));
    } 
    print(&dllist);

    printf("----------------Test:dllist_front dllist_back dllist_at-------------\n");
    printf("front:%d\nback:%d\n", *(int *)dllist_front(&dllist, NULL), *(int *)dllist_back(&dllist, NULL));
    printf("at 0:%d\n", *(int *)dllist_at(&dllist, 0, NULL));
    printf("at 3:%d\n", *(int *)dllist_at(&dllist, 3, NULL));
    printf("at end:%d\n", *(int *)dllist_at(&dllist, dllist_length(&dllist)-1, NULL));

    printf("-------------Test:dllist_pop_front dllist_pop_back------------------\n");
    dllist_pop_front(&dllist);
    dllist_pop_back(&dllist);
    print(&dllist);
    dllist_pop_front(&dllist);
    dllist_pop_back(&dllist);
    print(&dllist);

    dllist_clear(&dllist);
    for(int i = 0; i < sizeof(array)/sizeof(array[0]); ++i)
        dllist_push_back(&dllist, &array[i], sizeof(array[0]));

    printf("--------------------------Test:dllist_key_index---------------------\n");
    print(&dllist);
    int key_0 = 0, key_5 = 5, key_9 = 9, key_10 = 10;
    printf("index of 0: %d\n", dllist_key_index(&dllist, equal, &key_0));
    printf("index of 5: %d\n", dllist_key_index(&dllist, equal, &key_5));
    printf("index of 9: %d\n", dllist_key_index(&dllist, equal, &key_9));
    printf("index of 10: %d\n", dllist_key_index(&dllist, equal, &key_10));
    printf("-------------------------Test:dllist_index_insert---------------------\n");
    int value_n1 = -1, value_n2 = -2, value_n5 = -5, value_n9 = -9;
    dllist_index_insert(&dllist, 0, &value_n1, sizeof(value_n1));
    dllist_index_insert(&dllist, 3, &value_n2, sizeof(value_n2));
    dllist_index_insert(&dllist, dllist_length(&dllist)-1, &value_n5, sizeof(value_n5));
    dllist_index_insert(&dllist, dllist_length(&dllist), &value_n9, sizeof(value_n9));
    print(&dllist);
    printf("-------------------------Test:dllist_index_delete---------------------\n");
    dllist_index_delete(&dllist, 3);
    dllist_index_delete(&dllist, 0);
    dllist_index_delete(&dllist, dllist_length(&dllist)-1);
    dllist_index_delete(&dllist, dllist_length(&dllist)-2);
    print(&dllist);

    printf("-------------------------Test:dllist_iter_insert---------------------\n");
    int value_0 = 0, value_2 = 2, value_5 = 5, value_9 = 9, value_10000 = 10000;
    for(DLLIter iter = dllist_begin(&dllist); iter != dllist_end(); iter = dllist_next(iter))
    {
        if(equal(&value_0, dllist_data(iter, NULL)))
            dllist_iter_insert(&dllist, iter, &value_n1, sizeof(value_n1));
        if(equal(&value_2, dllist_data(iter, NULL)))
            dllist_iter_insert(&dllist, iter, &value_n2, sizeof(value_n2));
        if(equal(&value_5, dllist_data(iter, NULL)))
            dllist_iter_insert(&dllist, iter, &value_n5, sizeof(value_n5));
        if(equal(&value_9, dllist_data(iter, NULL)))
            dllist_iter_insert(&dllist, iter, &value_n9, sizeof(value_n9));
    }
    DLLIter iter = dllist_end();
    dllist_iter_insert(&dllist, iter, &value_10000, sizeof(value_10000));
    print(&dllist);

    printf("-------------------------Test:dllist_iter_delete---------------------\n");
    for(DLLIter iter = dllist_begin(&dllist); iter != dllist_end(); iter = dllist_next(iter))
    {
        if(equal(&value_n1, dllist_data(iter, NULL)))
            dllist_iter_delete(&dllist, &iter);
        if(equal(&value_n2, dllist_data(iter, NULL)))
            dllist_iter_delete(&dllist, &iter);
        if(equal(&value_n5, dllist_data(iter, NULL)))
            dllist_iter_delete(&dllist, &iter);
        if(equal(&value_n9, dllist_data(iter, NULL)))
            dllist_iter_delete(&dllist, &iter);
        if(equal(&value_10000, dllist_data(iter, NULL)))
            dllist_iter_delete(&dllist, &iter);
    }
    print(&dllist);

    printf("-------------Test:dllist_find_first dllist_find_last dllist_count-------\n");
    dllist_push_front(&dllist, &value_5, sizeof(value_5));
    dllist_push_front(&dllist, &value_5, sizeof(value_5));
    dllist_push_front(&dllist, &value_5, sizeof(value_5));
    dllist_push_back(&dllist, &value_5, sizeof(value_5));
    dllist_push_back(&dllist, &value_5, sizeof(value_5));
    iter = dllist_find_first(&dllist, equal, &value_5);
    dllist_iter_insert(&dllist, iter, &value_10000, sizeof(value_10000));
    iter = dllist_find_last(&dllist, equal, &value_5);
    dllist_iter_insert(&dllist, iter, &value_10000, sizeof(value_10000));
    int count = dllist_count(&dllist, equal, &value_5);
    printf("count of 5:%d\n", count);
    print(&dllist);

    printf("-------------Test:dllist_key_delete-------\n");
    for(int i = 0; i < count; ++i)
        dllist_key_delete(&dllist, equal, &value_5);
    dllist_key_delete(&dllist, equal, &value_10000);
    dllist_key_delete(&dllist, equal, &value_10000);
    print(&dllist);

    printf("-------------Test:time-------\n");
    //当前数据量非常大时，比如10000个数据时，llist_push_back和llist_pop_back会
    //使单链表的速度变得非常慢。而双链表却依然很快
    printf("time:%ld\n", clock() - t);
    return 0;
}
