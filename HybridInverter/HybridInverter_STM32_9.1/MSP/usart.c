/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : usart.c
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/10/22 09:08:41
*  @Last Modified  : 2017/11/22 09:42:31
********************************************************************************
*/

#include <stdio.h>
#include <stdarg.h>

#include "stm32f10x_conf.h"
#include "usart.h"
#include "debug.h"

#define USART_PRINTF_BUFF_LENGTH 128

void USART1_Init(uint32_t baudRate, uint16_t wordLength, uint16_t parity, uint16_t stopBits)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef init;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    //USART TX
    init.GPIO_Pin = GPIO_Pin_9;
    init.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &init);
    //USART RX
    init.GPIO_Pin = GPIO_Pin_10;
    init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &init);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    USART_InitTypeDef usart_init;
    usart_init.USART_BaudRate = baudRate;
    usart_init.USART_WordLength = wordLength;
    usart_init.USART_Parity = parity;
    usart_init.USART_StopBits = stopBits;
    usart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &usart_init);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启串口中断
    USART_Cmd(USART1, ENABLE); //使能串口

    while( !(USART1->SR & USART_FLAG_TC) )
        ;

    NVIC_InitTypeDef nvic_init;
    nvic_init.NVIC_IRQChannel = USART1_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 1;
    nvic_init.NVIC_IRQChannelSubPriority = 0;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);
}

void USART2_Init(uint32_t baudRate, uint16_t wordLength, uint16_t parity, uint16_t stopBits)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef init;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    //USART TX
    init.GPIO_Pin = GPIO_Pin_2;
    init.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &init);
    //USART RX
    init.GPIO_Pin = GPIO_Pin_3;
    init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &init);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    USART_InitTypeDef usart_init;
    usart_init.USART_BaudRate = baudRate;
    usart_init.USART_WordLength = wordLength;
    usart_init.USART_Parity = parity;
    usart_init.USART_StopBits = stopBits;
    usart_init.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &usart_init);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //开启串口中断
    USART_Cmd(USART2, ENABLE); //使能串口

    while( !(USART2->SR & USART_FLAG_TC) )
        ;

    NVIC_InitTypeDef nvic_init;
    nvic_init.NVIC_IRQChannel = USART2_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 1;
    nvic_init.NVIC_IRQChannelSubPriority = 1;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);
}

void USARTx_SendBuff(USART_TypeDef *USARTx, uint8_t buff[], int len)
{
    for(int i = 0; i < len; ++i)
    {
        USART_SendData(USARTx, buff[i]);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) != SET)
            ;
    }
}

int USARTx_Printf(USART_TypeDef *USARTx, const char *fmt, ...)
{
    char buff[USART_PRINTF_BUFF_LENGTH];
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(buff, sizeof(buff), fmt, ap);
    va_end(ap);
    USARTx_SendBuff(USARTx, (uint8_t *)buff, len + 1);
    return len + 1;
}

//定义成弱函数后可以在其它地方被同名函数重载。
__weak void USART1Callback(uint16_t data)
{
    (void)data;
}

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint16_t RecByte = USART_ReceiveData(USART1);
        USART1Callback(RecByte);
    }
}

//定义成弱函数后可以在其它地方被同名函数重载。
__weak void USART2Callback(uint16_t data)
{
    (void)data;
}

void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        uint16_t RecByte = USART_ReceiveData(USART2);
        USART2Callback(RecByte);
    }
}
