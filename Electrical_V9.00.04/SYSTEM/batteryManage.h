/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : batteryManage.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/11/20 17:11:42
*  @Last Modified  : 2018/07/18 18:57:24
********************************************************************************
*/

#ifndef BATTERYMANAGE_H
#define BATTERYMANAGE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    //电池电压
    float voltage;
    //电池充电电流
    float charge_current;
    //电池放电电流
    float discharge_current;
    //前电推杆反馈电压
    float front_pushrod_voltage;
    //后电推杆反馈电压
    float rear_pushrod_voltage;
    //保护电推杆反馈电压
    float protect_pushrod_voltage;
    //为了减少计算电流积分时的精度损失，energy必须定义为双精度浮点数。
    //电池剩余电量
    double energy;
    //电池剩余电量百分比
    uint8_t energy_percent;
    //电池温度
    float temperature;
    //电池在当前温度下的容量
	float capacity;
    //充电和放电通道的调零标志
    bool zeroset_flag;
    //充电和放电通信的调零值
    float zeroset_value;
} BatteryInfo_t;

void battery_init(void);
void battery_energe_init(void);
void battery_zerosetting(void);
void battery_manage(void);
void battery_getInfo(BatteryInfo_t *info);
void battery_setTemperature(uint16_t temp);

#endif /* end of include guard: BATTERYMANAGE_H */
