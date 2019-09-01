/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : adc.h
*  @author         : scm 351721714@qq.com
*  @create         : 2018/09/01 10:32:04
********************************************************************************
*/

#ifndef ADC_H
#define ADC_H

#include "stm32f10x_adc.h"

void adc_init(
        ADC_TypeDef *ADCx,
        uint8_t ADC_Channel_x,
        FunctionalState scanMode,
        FunctionalState continueMode);

void adc_regularChannelConfig(
        ADC_TypeDef *ADCx,
        uint8_t ADC_Channel_x,
        uint8_t Rank,
        uint8_t ADC_SampleTime_x);

void adc_regularDMAConfig(ADC_TypeDef *ADCx, uint16_t *buff, uint16_t size);

#endif /* end of include guard: ADC_H */
