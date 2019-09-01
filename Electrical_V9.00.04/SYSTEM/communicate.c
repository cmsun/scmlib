/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : communication.c
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/11/13 17:20:07
*  @Last Modified  : 2018/07/30 10:57:03
********************************************************************************
*/

#include <stdio.h>
#include "communicate.h"
#include "usart.h"
#include "gpio.h"
#include "led.h"
#include "acs712.h"
#include "debug.h"

uint8_t usart_buff[USART_BUFFER_LENGTH] = {0};
static bool g_ReceiveCMD = false;
static volatile int receive_count = 0;
static volatile bool receive_success = false;
static volatile int receive_timeout = 0;

uint16_t htons(uint16_t host)
{
    uint16_t net;
    uint8_t *p = (uint8_t *)&net;
    p[0] = (host & 0xff00) >> 8;
    p[1] = host & 0xff;
    return net;
}

uint32_t htonl(uint32_t host)
{
    uint32_t net;
    uint8_t *p = (uint8_t *)&net;
    p[0] = (host & 0xff000000) >> 24;
    p[1] = (host & 0xff0000) >> 16;
    p[2] = (host & 0xff00) >> 8;
    p[3] = host & 0xff;
    return net;
}

uint16_t ntohs(uint16_t net)
{
    uint8_t *p = (uint8_t *)&net;
    return p[0]<<8 | p[1];
}

uint32_t ntohl(uint32_t net)
{
    uint8_t *p = (uint8_t *)&net;
    return p[0]<<24 | p[1]<<16 | p[2]<<8 | p[3];
}

//calculate checksum
static uint8_t checksum(uint8_t buff[], int len)
{
    if(buff == NULL || len < 1)
    {
        return 0xff;
    }

    uint8_t chksm = buff[0];
    for(int i = 1; i < len; ++i)
        chksm ^= buff[i];
    return chksm;
}

//usart receive function
void USART2_Callback(uint16_t data)
{
    int expect_len;

    led_green_troggle();
    receive_timeout = 0;

    if(receive_success)
	{
		//USART_ClearFlag(USART1, USART_FLAG_RXNE);
        return; //success receive a frame, and wait to process it.
	}

    switch(receive_count)
    {
        case 0:
            if(data == COM_FRAME_START) //check start flag
                usart_buff[receive_count++] = data;
            break;

        case 1:
            if(data != COM_FRAME_TYPEQ) //check frame type.
                receive_count = 0;
            else
                usart_buff[receive_count++] = data;
            break;

        default:
            usart_buff[receive_count++] = data;
            expect_len = usart_buff[2] + 5;
            if(expect_len > USART_BUFFER_LENGTH) //检查是否会溢出
            {
                receive_count = 0;
                led_green_off();
            }
            else if(expect_len == receive_count)  //check length.
            {
                if(data == COM_FRAME_END) //check end flag.
                {
                    receive_success = true;
                    led_green1_on();
                }
                else
                {
                    receive_count = 0;
                }
                led_green_off();
            }
    }
}

#define ADM3485_init() GPIO_Initialize(GPIOA, GPIO_Pin_0, GPIO_Mode_Out_PP, GPIO_Speed_2MHz)
#define ADM3485_setReceiveMode() (PAout(0) = 0)
#define ADM3485_setSendMode() (PAout(0) = 1)

//initialize usart
void COM_Init(void)
{
    USART2_Init(115200, USART_WordLength_8b, USART_Parity_No, USART_StopBits_1);
    ADM3485_init();
    ADM3485_setReceiveMode();
}

static void COM_Acknowledge(BatteryInfo_t *info)
{
	if(g_ReceiveCMD == false)
		return;

    ACK_FRAME_FORMAT msg;
    msg.start_flag = COM_FRAME_START;
    msg.frame_type = COM_FRAME_TYPEA;
    msg.data_length = (uint8_t)sizeof(ACK_FRAME_FORMAT) - 5;
    msg.voltage = htons((int16_t)info->voltage);
    msg.charge_current = htons((int16_t)info->charge_current);
    msg.discharge_current = htons((int16_t)info->discharge_current);
    msg.energy = htonl((uint32_t)info->energy);
    msg.energy_percent = info->energy_percent;
    msg.front_pushrod_voltage = htons(info->front_pushrod_voltage * 1000);
    msg.rear_pushrod_voltage = htons(info->rear_pushrod_voltage * 1000);
    msg.protect_pushrod_voltage = htons(info->protect_pushrod_voltage * 1000);
    msg.end_flag = COM_FRAME_END;
    msg.checksum = checksum((uint8_t *)&msg, sizeof(msg) - 2);

    ADM3485_setSendMode();
    USARTx_SendBuff(USART2, (uint8_t *)&msg, sizeof(msg));
    ADM3485_setReceiveMode();
}

void COM_Cmd(bool state)
{
	g_ReceiveCMD = state;
}

void COM_Poll(void)
{
    BatteryInfo_t info;
    
    if(receive_count != 0 && receive_timeout++ > 50)
    {
        receive_count = 0;
        receive_timeout = 0;
    }
    
    if(receive_success && checksum(usart_buff, receive_count - 1) == 0)
    {
        uint16_t temper = (usart_buff[3]<<8) | usart_buff[4];
        battery_setTemperature(temper);
        battery_getInfo(&info);
        COM_Acknowledge(&info);
        receive_count = 0;
        receive_success = false;
        led_green1_off();
    }
}
