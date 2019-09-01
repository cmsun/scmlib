/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : update.h
*  @author         : scm 351721714@qq.com
*  @create         : 2019/01/31 23:26:29
********************************************************************************
*/
#ifndef UPDATE_H
#define UPDATE_H

#include <stdint.h>

#define UPDATE_FRAME_HEAD 0x10  //帧头
#define UPDATE_FRAME_TAIL 0x88  //帧尾
#define UPDATE_FRAME_CMD  0x41  //命令帧类型
#define UPDATE_FRAME_DATA 0x4A  //数据帧类型

#pragma pack(1)

typedef struct Update_Cmd_Req
{
    uint8_t head;
    uint8_t type;
    uint8_t length;
    uint8_t ID[9];
    uint8_t command;
    uint32_t firmwareSize;
    uint32_t reserve;
    uint8_t checkSum;
    uint8_t tail;
} Update_Cmd_Req;

typedef struct Update_Cmd_Ack
{
    uint8_t head;
    uint8_t type;
    uint8_t length;
    uint8_t ID[9];
    uint8_t acknowledge;
    uint32_t reserve;
    uint8_t checkSum;
    uint8_t tail;
} Update_Cmd_Ack;

typedef struct Update_Transmit_Req
{
    uint8_t head;
    uint8_t type;
    uint8_t length;
    uint8_t ID[9];
    uint8_t dataType;
    uint8_t sendCnt;
    uint32_t firmwareIndex;
    uint8_t data;
} Update_Transmit_Req;

typedef struct Update_Transmit_Ack
{
    uint8_t head;
    uint8_t type;
    uint8_t length;
    uint8_t ID[9];
    uint8_t acknowledge;
    uint32_t nextOffset;
    uint32_t reserve;
    uint8_t checkSum;
    uint8_t tail;
} Update_Transmit_Ack;

#pragma pack()

uint8_t update_checksum(uint8_t *data, uint16_t len);
void update_Cmd_process(uint8_t *recData, uint16_t len);
void update_Transmit_process(uint8_t *recData, uint16_t len);
void update_timeout_check(void);

#endif /* end of include guard: UPDATE_H */
