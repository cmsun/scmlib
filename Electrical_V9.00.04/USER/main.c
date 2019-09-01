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

#include "led.h"
#include "communicate.h"
#include "acs712.h"
#include "watchdog.h"
#include "systick.h"
#include "sysconf.h"
#include "debug.h"

static volatile int task_tick = TASK_PERIOD;

static void systick_callback(void *arg)
{
    (void)arg;
    if(task_tick > 0)
    {
        --task_tick;
    }
}

int main(int argc, char *argv[])
{
    (void)argc, (void)argv; //prevent compiler complain
	
    int watchdogPeriod = WDG_FEED_PERIOD;
    int redLedPeriod = RED_LED_PERIOD;
    int redLedBlink = RED_LED_BLINK;
	
	COM_Cmd(false);		//没采样到稳定的电压电流前不允许通信

    _debug("system running...\n");

    COM_Init();		//初始化串口
    led_init();		//初始化LED
    ACS712_Init();	//初始化电压电流检测
    WDG_Init();		//初始化看门狗

    //task initialize
	systick_init();
    systick_setTimeOut_ms(TASK_TICK_INTERVAL, systick_callback, 0);
    systick_enable();

    //延时一秒钟，等待电压电流稳定
    for(int i = 0; i < 200; ++i)
    {
        while(task_tick > 0)
            ;
        task_tick = TASK_PERIOD;
        WDG_Feed();
    }

    //对电压进行一次完整的平滑滤波
    for(int i = 0; i < SMOOTH_FILTER_CNT; ++i)
    {
        while(task_tick > 0)
            ;
        task_tick = TASK_PERIOD;
        battery_manage();   //capture voltage
        WDG_Feed();
    }
    battery_zerosetting();  //充电和放电调零
	
	COM_Cmd(true);	//采样到稳定的电压和电流并且调零以及初始化电池电量后才可以通信
    
    while(1)
    {
		//5ms一个任务周期
        while(task_tick > 0)
            ;
        task_tick = TASK_PERIOD;

		//电池电量管理
        battery_manage();

		//串口通信轮询
        COM_Poll();

		//喂看门狗
        if(watchdogPeriod-- <= 0)
        {
            watchdogPeriod = WDG_FEED_PERIOD;
            WDG_Feed(); //feeding watchdog
        }

		//系统指示灯闪烁
        if(redLedPeriod-- <= 0)
        {
            redLedPeriod = RED_LED_PERIOD;
            redLedBlink = RED_LED_BLINK;
            led_red_on();
        }
        else
        {
            if(redLedBlink > 0)
                redLedBlink--;
            else
                led_red_off();
        }
    }
    /* return 1; */
}
