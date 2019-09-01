/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : debug.c
*  @author         : scm 351721714@qq.com
*  @create         : 2018/10/18 09:31:11
********************************************************************************
*/

#include <stdarg.h>
#include <stdio.h>
#include "debug.h"

int uprintf(UART_HandleTypeDef *huart, const char *fmt, ...)
{
    char buff[128] = {0};
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(buff, sizeof(buff), fmt, ap);
    HAL_StatusTypeDef result = HAL_UART_Transmit(huart, (uint8_t *)buff, len, 100);
    if(result != HAL_OK) return 0;
    return len; 
}

HAL_StatusTypeDef debug_send(uint8_t *buff, uint32_t len)
{
    HAL_GPIO_WritePin(USART2_RS485_WR_GPIO_Port, USART2_RS485_WR_Pin, GPIO_PIN_SET);
    HAL_StatusTypeDef result = HAL_UART_Transmit(&huart2, buff, len, 100);
    HAL_GPIO_WritePin(USART2_RS485_WR_GPIO_Port, USART2_RS485_WR_Pin, GPIO_PIN_RESET);
    return result;
}

HAL_StatusTypeDef debug_printf(const char *fmt, ...)
{
    char buff[128] = {0};
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(buff, sizeof(buff), fmt, ap);
    return debug_send((uint8_t *)buff, len);
}
