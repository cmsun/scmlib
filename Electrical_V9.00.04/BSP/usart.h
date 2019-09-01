/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : usart.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/10/22 21:39:00
*  @Last Modified  : 2017/11/22 09:36:51
********************************************************************************
*/

#ifndef USART_H
#define USART_H

#include <stdint.h>
#include "stm32f10x_conf.h"

typedef void (*USART_CALLBACK_FUNCTION)(uint16_t, void *);

void USARTx_SendBuff(USART_TypeDef *USARTx, uint8_t buff[], int len);
int USARTx_Printf(USART_TypeDef *USARTx, const char *fmt, ...);

void USART2_Init(uint32_t baudRate, uint16_t wordLength, uint16_t parity, uint16_t stopBits);
void USART2_Callback(uint16_t data);

#endif /* end of include guard: USART_H */
