/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : batteryManage.c
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/11/20 17:06:26
*  @Last Modified  : 2018/07/19 10:34:19
********************************************************************************
*/

#include <string.h>
#include <math.h>
#include "acs712.h"
#include "sysconf.h"

typedef struct SmoothFilter_TypeDef
{
    float buff[SMOOTH_FILTER_CNT];
    float sum;
    int index;
    bool is_full;
} SmoothFilter_TypeDef;

SmoothFilter_TypeDef VoltageSmooth          = {{0}, 0, 0, false};
SmoothFilter_TypeDef ChargeSmooth           = {{0}, 0, 0, false};
SmoothFilter_TypeDef DischargeSmooth        = {{0}, 0, 0, false};
SmoothFilter_TypeDef FrontPushrodSmooth     = {{0}, 0, 0, false};
SmoothFilter_TypeDef RearPushrodSmooth      = {{0}, 0, 0, false};
SmoothFilter_TypeDef ProtectPushrodSmooth   = {{0}, 0, 0, false};

BatteryInfo_t g_batInfo = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//电池23度时按电压计算当前电量
static double battery_calculate_energe_p23(float v)
{
    double energy;
    if(v > 26018)
        //6.11747125146722E-13*Y4^5 - 8.22413660726901E-08*Y4^4 + 0.00442215495359303*Y4^3 - 118.881185686535*Y4^2 + 1597820.95878923*Y4 - 8589514920.71208
        energy = 6.11747125146722E-13*powl(v, 5) - 8.22413660726901E-08*powl(v, 4) + 0.00442215495359303*powl(v, 3) - 118.881185686535*powl(v, 2) + 1597820.95878923*v - 8589514920.71208;
    else if(v > 24972.0)
        //4.12319369098684E-13*Y119^6 - 6.30013681336513E-08*Y119^5 + 0.00401084607986381*Y119^4 - 136.176376667451*Y119^3 + 2600585.5188539*Y119^2 - 26486240583.3462*Y119 + 112393027992925
        energy = 4.12319369098684E-13*powl(v, 6) - 6.30013681336513E-08*powl(v, 5) + 0.00401084607986381*powl(v, 4) - 136.176376667451*powl(v, 3) + 2600585.5188539*powl(v, 2) - 26486240583.3462*v + 112393027992925;
    else if(v > 24914)
        //-2.67929711269911E-10*Y947^4 + 0.0000314262034498892*Y947^3 - 1.34313612127354*Y947^2 + 24984.4040048946*Y947 - 171522264.590447
        energy = -2.67929711269911E-10*powl(v, 4) + 0.0000314262034498892*powl(v, 3) - 1.34313612127354*powl(v, 2) + 24984.4040048946*v - 171522264.590447;
    else if(v > 19616)
        //6.02841070292631E-09*Y1080^3 - 0.000363803368832026*Y1080^2 + 7.35881345930714*Y1080 - 49872.8758369578
        energy = 6.02841070292631E-09*powl(v, 3) - 0.000363803368832026*powl(v, 2) + 7.35881345930714*v - 49872.8758369578;
    else
        energy = 0.0;
	
	if(energy < 0.0)
		energy = 0.0;
	else if(energy > g_batInfo.capacity)
		energy = g_batInfo.capacity;
	
    return energy;
}

//电池零下5度时按电压计算当前电量
static double battery_calculate_energe_n5(float v)
{
    double energy;
    if(v > 24750)
        energy = g_batInfo.capacity;
    else if(v > 23216)
        energy = 1.03485589645093E-08*powl(v, 4) - 0.000989862850392439*powl(v, 3) + 35.5030104915098*powl(v, 2) - 565894.079720844*v + 3382187580.86068;
    else if(v > 16554)
        energy = 5.07848991596295E-06*powl(v, 2) + 0.113817467189328*v - 3336.72163891065;
    else
        energy = 0.0;
	
	if(energy < 0.0)
		energy = 0.0;
	else if(energy > g_batInfo.capacity)
		energy = g_batInfo.capacity;
	
    return energy;
}

static double battery_calculate_energe_n20(float v)
{
    return 0;
}

//判断浮点数是否相等
#define fequal(a, b) (((a) - (b)) < 0.000001f)

//根据温度设置电池的容量
static void battery_setCapacity(void)
{
	if(g_batInfo.temperature < -40.0f || fequal(g_batInfo.temperature, -40.0f))
		g_batInfo.capacity = BATTERY_CAPACITY_N40 * 1000;
	else if(g_batInfo.temperature < -20.0f || fequal(g_batInfo.temperature, -20.0f))
		g_batInfo.capacity = BATTERY_CAPACITY_N20 * 1000;
	else if(g_batInfo.temperature < -5.0f || fequal(g_batInfo.temperature, -5.0f))
		g_batInfo.capacity = BATTERY_CAPACITY_N5 * 1000;
	else if(g_batInfo.temperature < 23.0f || fequal(g_batInfo.temperature, 23.0f))
		g_batInfo.capacity = BATTERY_CAPACITY_P23 * 1000;
	else
		g_batInfo.capacity = BATTERY_CAPACITY_P55 * 1000;
}

//根据电池温度和电压设置电池的剩余电量
void battery_energe_init(void)
{
    if(g_batInfo.temperature > 0)
        g_batInfo.energy = battery_calculate_energe_p23(g_batInfo.voltage);
    else if(g_batInfo.temperature < 0 && g_batInfo.temperature > -15.0f)
        g_batInfo.energy = battery_calculate_energe_n5(g_batInfo.voltage);
    else
        g_batInfo.energy = battery_calculate_energe_n20(g_batInfo.voltage);

    //计算当前剩余电量百分比，并4舍5入
    g_batInfo.energy_percent = g_batInfo.energy / g_batInfo.capacity * 100 + 0.5;
}

//ADC采样平滑滤波
static float Smooth_filter(SmoothFilter_TypeDef *obj, float value)
{
    float result;

    if(!obj->is_full && obj->index+1 == SMOOTH_FILTER_CNT)
        obj->is_full = true;

    obj->sum -= obj->buff[obj->index];
    obj->sum += value;
    obj->buff[obj->index] = value;

    if(obj->is_full)
        result = obj->sum / SMOOTH_FILTER_CNT;
    else
        result = obj->sum / (obj->index + 1);

    obj->index++;
    obj->index %= SMOOTH_FILTER_CNT;

    return result;
}

void battery_manage(void)
{
    static double protectPower = 0;
    float prev_charge = 0, prev_discharge = 0;
    const double time = (TASK_TICK_INTERVAL * TASK_PERIOD) / (3600 * 1000); //单位小时
    double charge_energy, discharge_energy, energy_change;
    BatteryInfo_t temp;

    prev_charge = g_batInfo.charge_current;
    prev_discharge = g_batInfo.discharge_current;

    ACS712_Capture(&temp);
    g_batInfo.voltage = Smooth_filter(&VoltageSmooth, temp.voltage);
    g_batInfo.charge_current = Smooth_filter(&ChargeSmooth, temp.charge_current);
    g_batInfo.discharge_current = Smooth_filter(&DischargeSmooth, temp.discharge_current);
    g_batInfo.front_pushrod_voltage = Smooth_filter(&FrontPushrodSmooth, temp.front_pushrod_voltage);
    g_batInfo.rear_pushrod_voltage = Smooth_filter(&RearPushrodSmooth, temp.rear_pushrod_voltage);
    g_batInfo.protect_pushrod_voltage = Smooth_filter(&ProtectPushrodSmooth, temp.protect_pushrod_voltage);
    
    //充电电流加上零点偏移
    g_batInfo.charge_current += g_batInfo.zeroset_value;
    if(g_batInfo.zeroset_flag && g_batInfo.charge_current < 80.0f)
    {
        g_batInfo.charge_current = 0.0f;
    }

    //放电电流加上零点偏移
    g_batInfo.discharge_current += g_batInfo.zeroset_value;
    if(g_batInfo.zeroset_flag && g_batInfo.discharge_current < 100.0f)
    {
        g_batInfo.discharge_current = 200.0f;
    }

    //计算一次采样周期内充电的电量(梯形面积)
    charge_energy = (prev_charge + temp.charge_current) * time / 2;
    //计算一次采样周期内放电的电量(梯形面积)
    discharge_energy = (prev_discharge + temp.discharge_current) * time / 2;
    //计算一次采样周期内电池电量的变化量
    energy_change = charge_energy - discharge_energy;

    //用充电电压和充电电流来判断电池充满：29.2V 0.2A
    if(g_batInfo.voltage > 28000.0f && g_batInfo.charge_current < 300.0f)
    {
        battery_setCapacity();
        g_batInfo.energy = g_batInfo.capacity;
    }
    //当放电大于充电时，或者当充电且没充到98%前计算电量的变化。
    else if(energy_change < 0 || g_batInfo.energy_percent < 98)
    {
        g_batInfo.energy += energy_change;
    }
    else
    {
        protectPower += energy_change;

        //当充电且充到98%以后继续充电1.5Ah后如果还没有充满(充满条件为充电电压
        //29.2V充电电流0.2A，但是由于阴天会出现电压无法达到29.2V但却一直有小电流充电的情况)，则将电量修改为99%
        if(g_batInfo.energy_percent >= 98 && protectPower >= 1.5*1000)
        {
            protectPower = 0;
            g_batInfo.energy = g_batInfo.capacity * 0.99f;
        }

        //当充电且充到99%以后继续充电1.5Ah后如果还没有充满(充满条件为充电电压
        //29.2V充电电流0.2A，但是由于阴天会出现电压无法达到29.2V但却一直有小电流充电的情况)，为了保护电池也认为电池已经充满。
        if(g_batInfo.energy_percent >= 99 && protectPower >= 1.5*1000)
        {
            protectPower = 0;
            battery_setCapacity();
            g_batInfo.energy = g_batInfo.capacity;
        }
    }

    if(g_batInfo.energy < 0)
    {
        g_batInfo.energy = 0;
    }
    else if(g_batInfo.energy > g_batInfo.capacity)
    {
        g_batInfo.energy = g_batInfo.capacity;
    }

    //计算剩余电量百分比，并4舍5入
    g_batInfo.energy_percent = g_batInfo.energy / g_batInfo.capacity * 100 + 0.5;
}

void battery_zerosetting(void)
{
    if(g_batInfo.zeroset_flag == false)
    {
        g_batInfo.zeroset_flag = true;
        g_batInfo.zeroset_value = -g_batInfo.charge_current;
    }
}

void battery_setTemperature(uint16_t temp)
{
    static bool firstSetTemp = true;
    g_batInfo.temperature = temp / 100.0f;

    //第一次设置温度时要根据电压判断电池容量和剩余电量
    if(firstSetTemp)
    {
        battery_setCapacity();	//根据温度设置电池容量
        battery_energe_init();	//根据温度和电压计算电池剩余电量
        firstSetTemp = false;
    }
}

void battery_getInfo(BatteryInfo_t *info)
{
    memcpy(info, &g_batInfo, sizeof(BatteryInfo_t));
}
