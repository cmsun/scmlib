/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : sysconf.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/11/23 13:40:28
*  @Last Modified  : 2018/05/14 08:52:47
********************************************************************************
*/

#ifndef SYSCONF_H
#define SYSCONF_H

#define TASK_PERIOD         1       //task period unit: tick
#define TASK_TICK_INTERVAL  5       //task tick time: 5ms
#define WDG_FEED_PERIOD     20      //watchdog period
#define RED_LED_PERIOD      250     //red led period
#define RED_LED_BLINK		10      //red led flicker time
#define SMOOTH_FILTER_CNT   12      //smooth filter buffer count

#define BATTERY_CAPACITY_N40    10.473		//-40度时电池容量，单位A*h(安时)
#define BATTERY_CAPACITY_N20    11.034		//-20度时电池容量，单位A*h(安时)
#define BATTERY_CAPACITY_N5		11.166		//-5度时电池容量，单位A*h(安时)
#define BATTERY_CAPACITY_P23    11.375		//23度时电池容量，单位A*h(安时)
#define BATTERY_CAPACITY_P55    11.378		//55度时电池容量，单位A*h(安时)

#endif /* end of include guard: SYSCONF_H */
