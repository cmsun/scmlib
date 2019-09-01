/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : debug.c
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/10/22 08:11:55
*  @Last Modified  : 2017/11/16 14:02:41
********************************************************************************
*/

#include <stdio.h>
#include <stdarg.h>

#include "stm32f10x_usart.h"
#include "debug.h"

#if defined(DEBUG) || defined(_DEBUG)
int __printf(const char *fmt, ...)
{
    char buff[DEBUG_BUFF_LENGTH];
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(buff, sizeof(buff), fmt, ap);
    va_end(ap);

    for(int i = 0; i < len; ++i)
    {
#if defined(DEBUG_WITH_SWO)
        /* print debug message with st-link */
        ITM_SendChar(buff[i]);
#endif

#if defined(DEBUG_WITH_USART)
        /* print debug message with usart */
        USART_SendData(DUSART, buff[i]);
        while(USART_GetFlagStatus(DUSART, USART_FLAG_TC) != SET)
            ;
#endif
    }

    return len+1;
}
#endif /* DEBUG or _DEBUG*/
