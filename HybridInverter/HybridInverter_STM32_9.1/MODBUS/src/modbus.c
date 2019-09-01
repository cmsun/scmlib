/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : modbus.c
*  @author         : scm 351721714@qq.com
*  @create         : 2018/12/20 10:44:17
********************************************************************************
*/

#include "mbconfig.h"
#include "modbus.h"
#include "mbrtu.h"
#include "mbascii.h"

#include "usart.h"
#include "timer.h"
#include "led.h"

mbu8 mbRecBuff[MB_RECEIVE_BUFFER_LENGTH] = {0};
mbu8 mbSendBuff[MB_SEND_BUFFER_LENGTH] = {0};
volatile mbu16 mbRecCount = 0;

/*
 * modbus通信协议中规定：一个逻辑线圈或者一个继电器或者一个数字量输入占用一个位。
 * modbus通信协议中规定：所有的寄存器（保持型寄存器和模拟量输入寄存器）都是双字节的。
 */
#if MB_COIL_COUNT > 0
mbu8 mbCoil [(MB_COIL_COUNT+7)/8] = {0};            //线圈和继电器(一个位对应一个逻辑线圈)
#endif

#if MB_DISINPUT_COUNT > 0
mbu8 mbDisInput [(MB_DISINPUT_COUNT+7)/8] = {0};    //Discrete Input 离散输入量(一个位对应一个离散输入量)
#endif

#if MB_HOLDREG_COUNT > 0
mbu16 mbHoldReg [MB_HOLDREG_COUNT] = {0};           //保持型寄存器
#endif

#if MB_INPUTREG_COUNT > 0
mbu16 InputReg[MB_INPUTREG_COUNT] = {0};            //输入寄存器
#endif

mbObject_t mbObject;

void mb_init(MODBUS_TYPE type)
{
    //设定modbus的工作模式是 RTU 还是 ASCII
    if(type == MODBUS_RTU)
    {
        mbObject.frame =                 	mbRecBuff;
        mbObject.frameLength =           	0;
        mbObject.sendBuff =              	mbSendBuff;
        mbObject.buffCapacity =          	MB_SEND_BUFFER_LENGTH;
        mbObject.chksumLength =          	2;

        mbObject.mb_func_receive =			mb_rtu_receive;
        mbObject.mb_func_send =				mb_raw_send;
        mbObject.mb_func_timeout =			mb_rtu_timeout;
        mbObject.mb_func_poll =				mb_rtu_poll;
        mbObject.mb_func_append_checksum =	mb_rtu_append_CRC;
    }
    else if(type == MODBUS_ASCII)
    {
        mbObject.frame =                 	mbRecBuff;
        mbObject.frameLength =           	0;
        mbObject.sendBuff =              	mbSendBuff;
        mbObject.buffCapacity =          	(MB_SEND_BUFFER_LENGTH-3)/2;
        mbObject.chksumLength =          	1;

        mbObject.mb_func_receive =			mb_ascii_receive;
        mbObject.mb_func_send =				mb_ascii_send;
        mbObject.mb_func_timeout =			mb_ascii_timeout;
        mbObject.mb_func_poll =				mb_ascii_poll;
        mbObject.mb_func_append_checksum =	mb_ascii_append_LRC;         	
    }
    
    //初始化串口和超时定时器
    mb_hardware_init();
}

void mb_clear_receive_buff(void)
{
    mbRecCount = 0;
}

void mb_timeout(void)
{
    mbObject.mb_func_timeout();
}

void mb_receive(mbu8 data)
{
    mbObject.mb_func_receive(data);
}

void mb_poll(void)
{
    mbObject.mb_func_poll();
}

//移植时以上代码不需要修改
//--------------------------------------------------------------
//移植时以下代码需要修改，以适配不同硬件平台

void mb_hardware_init(void)
{
    USART1_Init(115200, USART_WordLength_8b, USART_Parity_No, USART_StopBits_1);
    TIM3_Init();
}

void mb_raw_send(mbu8 *data, mbu16 length)
{
    USARTx_SendBuff(USART1, data, length);
}
    
void mb_enable_timer(void)
{
    TIM_Cmd(TIM3, ENABLE);
}
    
void mb_disable_timer(void)
{
    TIM_Cmd(TIM3, DISABLE);
    TIM_SetCounter(TIM3, 0); //重新初始化定时器计数值为0
}

//串口1的中断函数调用USART1Callback弱函数，这里重载了这个弱函数
void USART1Callback(uint16_t data)
{
    mb_receive(data);
    led0_troggle();
}

//定时器3的中断函数调用TIM3Callback弱函数，这里重载了这个弱函数
void TIM3Callback(void)
{
    mb_timeout(); //接收完成一个 Modbus RTU 数据帧
    led0_off();
}


