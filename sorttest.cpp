/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : sorttest.cpp
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/06/19 23:00:26
*  @Last Modified  : 2017/09/05 18:21:44
********************************************************************************
*/

#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <functional> //std::less<>
#include "linklist.h"
#include "dlinklist.h"

//编译指令：g++ dlinklist.c linklist.c sorttest.cpp -std=c++11 -O3

int less(const void *data1, const void *data2)
{
    return *(int *)data1 - *(int *)data2;
}

int __less(const void *data1, const void *data2)
{
    return *(int *)data1 < *(int *)data2;
}

#define SIZE 500000

int main(int argc, const char *argv[])
{
    (void)argc;
    (void)argv;

    std::list<int> lst;
    DLinkList dllist;
    LinkList llist;
    int *array = new int[SIZE];
    clock_t t = 0;

    dllist_init(&dllist, sizeof(int));
    llist_init(&llist, sizeof(int));
    srand(time(NULL));
    
    for(int i = 0; i < SIZE; ++i)
    {
        //std::list::sort()用的是二路归排序，即使数据已经有序也不会出现最坏情况，反倒会更快。
        array[i] = rand();  //当数据无序时，std::list::sort()和dllist_sort()基本一样快
        // array[i] = i;       //当数据有序时，std::list::sort()比dllist_sort()快很多
    }

    t = clock();
    for(int i = 0; i < SIZE; ++i)
        lst.push_front(array[i]);
    printf("list push time: %d\n", clock() - t);

    t = clock();
    dllist_assign(&dllist, array, SIZE);
    llist_assign(&llist, array, SIZE);
    /*
     * for(int i = 0; i < SIZE; ++i)
     * {
     *     dllist_push_back(&dllist, array+i);
     *     llist_push_front(&llist, array+i);
     *     llist_push_back(&llist, array+i);
     * }
     */
    printf("dllist push time: %d\n", clock() - t);

    t = clock();
    lst.sort(std::less<int>());
    printf("std::sort time: %d\n", clock() - t);

    // t = clock();
    // dllist_quick_sort(&dllist, [](const void *data1, const void *data2)->int{return *(int *)data1 - *(int *)data2;});
    // llist_quick_sort(&llist, [](const void *data1, const void *data2)->int{return *(int *)data1 < *(int *)data2;});
    // printf("dllist_quick_sort time: %d\n", clock() - t);

    t = clock();
    dllist_merge_sort(&dllist, [](const void *data1, const void *data2)->int{return *(int *)data1 < *(int *)data2;});
    // llist_merge_sort(&llist, [](const void *data1, const void *data2)->int{return *(int *)data1 < *(int *)data2;});
    printf("dllist_merge_sort time: %d\n", clock() - t);

    int i = 100;
    for(std::list<int>::iterator iter = lst.begin(); iter != lst.end(); ++iter)
    {
        if(i-- == 0) break;
        printf("%d ", *iter);
    }

    printf("\n------------------------------------------------------------\n");

    i = 100;
    for(DllIter iter = dllist_begin(&dllist); iter != dllist_end(); iter = dllist_next(iter))
    {
        if(i-- == 0) break;
        printf("%d ", *(int *)dllist_data(iter));
    }

    printf("\n------------------------------------------------------------\n");

    /*
     * i = 100;
     * for(LLIter iter = llist_begin(&llist); iter != llist_end(); iter = llist_next(iter))
     * {
     *     if(i-- == 0) break;
     *     printf("%d ", *(int *)llist_data(iter));
     * }
     */

    return 0;
}
