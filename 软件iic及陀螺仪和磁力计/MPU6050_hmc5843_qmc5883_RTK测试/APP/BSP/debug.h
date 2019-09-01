/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : debug.h
*  @author         : scm 351721714@qq.com
*  @create         : 2018/10/18 09:32:55
********************************************************************************
*/

#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>
#include "usart.h"

int uprintf(UART_HandleTypeDef *huart, const char *fmt, ...);
HAL_StatusTypeDef debug_send(uint8_t *buff, uint32_t len);
HAL_StatusTypeDef debug_printf(const char *fmt, ...);

#endif /* end of include guard: DEBUG_H */
