/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : acs712.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/11/16 14:09:50
*  @Last Modified  : 2018/01/22 15:54:13
********************************************************************************
*/

#ifndef ACS712_H
#define ACS712_H

#include <stdint.h>

#include "batteryManage.h"

extern uint16_t adc_dma_buff[];

enum {VoltageIndex = 0,
    ChargeIndex,
    DischargeIndex,
    FrontPushrodIndex,
    RearPushrodIndex,
    ProtectPushrodIndex
};

void ACS712_Init(void);
void ACS712_Capture(BatteryInfo_t *info);

#endif /* end of include guard: ACS712_H */
