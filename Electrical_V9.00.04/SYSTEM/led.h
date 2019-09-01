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

#define led_red_on()            (PBout(7) = 0)
#define led_red_off()           (PBout(7) = 1)
#define led_red_troggle()       (PBout(7) ^= 1)

#define led_green_on()          (PBout(9) = 0)
#define led_green_off()         (PBout(9) = 1)
#define led_green_troggle()     (PBout(9) ^= 1)

#define led_green1_on()         (PBout(8) = 0)
#define led_green1_off()        (PBout(8) = 1)
#define led_green1_troggle()    (PBout(8) ^= 1)

#define led_init() \
{ \
    GPIO_Initialize(GPIOB, \
                    GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9, \
                    GPIO_Mode_Out_PP, \
                    GPIO_Speed_2MHz); \
    led_red_off(); \
    led_green_off(); \
    led_green1_off(); \
}

#endif /* end of include guard: LED_H */
