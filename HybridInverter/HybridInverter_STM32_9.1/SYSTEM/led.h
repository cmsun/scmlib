/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : led.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/11/13 10:56:42
*  @Last Modified  : 2017/11/21 22:21:04
********************************************************************************
*/

#ifndef LED_H
#define LED_H

#include "gpio.h"

#define led0_on()          (PBout(5) = 0)
#define led0_off()         (PBout(5) = 1)
#define led0_troggle()     (PBout(5) ^= 1)

#define led1_on()          (PEout(5) = 0)
#define led1_off()         (PEout(5) = 1)
#define led1_troggle()     (PEout(5) ^= 1)

#define led_init() \
{ \
    GPIO_Initialize(GPIOB, \
                    GPIO_Pin_5, \
                    GPIO_Mode_Out_PP, \
                    GPIO_Speed_2MHz); \
    GPIO_Initialize(GPIOE, \
                    GPIO_Pin_5, \
                    GPIO_Mode_Out_PP, \
                    GPIO_Speed_2MHz); \
    led0_off(); \
    led1_off(); \
}

#endif /* end of include guard: LED_H */
