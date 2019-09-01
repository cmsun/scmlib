/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : main.c
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/11/11 08:37:02
*  @Last Modified  : 2018/07/30 11:01:41
********************************************************************************
*/
#include "stm32f10x_conf.h"
#include "system_task.h"
#include "includes.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    system_init();

    SysTick_Config(SystemCoreClock/200);    //给UCOSII提供5ms的心跳节拍。
    OSInit();
 	OSTaskCreate(sSuperTask, NULL, STK_ADDR(SUPER_TASK_STK), SUPER_TASK_PRIORITY);
	OSStart();
    
    /* return 1; */
}
