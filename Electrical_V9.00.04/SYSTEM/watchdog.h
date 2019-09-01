/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : watchdog.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/11/20 13:50:11
*  @Last Modified  : 2017/11/20 13:59:04
********************************************************************************
*/

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include "gpio.h"

#define WDG_Init() GPIO_Initialize(GPIOB, GPIO_Pin_15, GPIO_Mode_Out_PP, GPIO_Speed_2MHz)
#define WDG_Feed() (PBout(15) ^= 1)

#endif /* end of include guard: WATCHDOG_H */
