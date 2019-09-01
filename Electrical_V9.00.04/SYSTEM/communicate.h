/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : communicate.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/11/13 17:21:56
*  @Last Modified  : 2018/07/30 10:56:28
********************************************************************************
*/

#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include <stdint.h>
#include <stdbool.h>
#include "acs712.h"

#define USART_BUFFER_LENGTH 64

#define COM_FRAME_START     0x10
#define COM_FRAME_TYPEQ     0xB1
#define COM_FRAME_TYPEA     0xB2
#define COM_FRAME_END       0x88

#pragma pack(push)
#pragma pack(1)
typedef struct
{
    uint8_t start_flag;
    uint8_t frame_type;
    uint8_t data_length;
    uint16_t voltage;
    uint16_t charge_current;
    uint16_t discharge_current;
    uint32_t energy;
    uint8_t energy_percent;
    uint16_t front_pushrod_voltage;
    uint16_t rear_pushrod_voltage;
    uint16_t protect_pushrod_voltage;
    uint8_t checksum;
    uint8_t end_flag;
} ACK_FRAME_FORMAT;
#pragma pack(pop)

uint16_t htons(uint16_t host);
uint32_t htonl(uint32_t host);
uint16_t ntohs(uint16_t net);
uint32_t ntohl(uint32_t net);
void COM_Init(void);
void COM_Cmd(bool state);
void COM_Poll(void);

#endif /* end of include guard: COMMUNICATE_H */
