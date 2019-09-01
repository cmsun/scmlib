/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : bootloader.c
*  @author         : scm 351721714@qq.com
*  @create         : 2019/01/31 14:11:28
********************************************************************************
*/

#include <stdint.h>
#include "bootloader.h"
#include "stm32f4xx_hal.h"

__ASM void MSR_MSP(volatile uint32_t addr)                            //重新设置SP指针
{
    MSR MSP, r0
    BX r14
}

void bootloader_jump_to_app(void)
{
//    typedef void (*Func_Reset_Handler)(void);                       //stm32的Reset_Handler函数类型
//    Func_Reset_Handler app = *(Func_Reset_Handler *)(APP_ADDR + 4); //VectorTable的第一个32位存放栈地址的顶，后面是一个中断函数Reset_Handler
    void(*app)(void) = *(void(**)(void))(APP_ADDR + 4);               //VectorTable的第一个32位存放栈地址的顶，后面是一个中断函数Reset_Handler
    MSR_MSP(*(volatile uint32_t*)APP_ADDR);                           //复位SP指针为栈顶，否则在bootloader程序中消耗的栈空间无法再被app程序使用
    SCB->VTOR = APP_ADDR;                                             //重新设置中断向量表到app固件的向量表处
    app();                                                            //执行app程序的Reset_Handler函数
}
