#ifndef __ANALOG_H
#define __ANALOG_H

#include "configure.h"
#include "filter.h"

typedef struct {
    float Vcc;          //系统电压
    float Vbat;         //电池电压
    float semtemp;      //半导体温度
    float bodytemp;     //体温
    uint16_t adc_vbat;
    uint16_t adc_vcc;
    uint16_t adc_sem;
    uint16_t adc_body;
    SmoothFilter_TypeDef sf_sem;
    SmoothFilter_TypeDef sf_body;
} Analog_TypeDef;

void analog_init(void);
float analog_battery_voltage(void);
float analog_body_temp(void);
float analog_semiconductor_temp(void);
void analog_task_loop(void);

#endif