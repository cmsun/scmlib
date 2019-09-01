/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : mbrtu.c
*  @Author         : scm 351721714@qq.com
*  @Create         : 2016/11/23 09:51:51
*  @Last Modified  : 2018/07/27 23:57:54
********************************************************************************
*/

#include <stddef.h>
#include <stdbool.h>
#include "mbconfig.h"
#include "modbus.h"
#include "mbchecksum.h"
#include "mberror.h"
#include "mbformat.h"
#include "byteorder.h"

static bool g_mbTimeout = false;

/*
********************************************************************************
*                               mb_rtu_receive 
*
* Description : 用户程序在串口中断函数中把接收到的数据通过mb_rtu_receive函数传给
*               modbus模块进行数据处理。
*
* Arguments   : recvalue: 串口接收到的数据。
*
* Returns     : none
*
* Note(s)     : none
********************************************************************************
*/
void mb_rtu_receive(mbu8 data)
{
	if(g_mbTimeout)
		return;
	
    mb_disable_timer();

    if(mbRecCount < MB_RECEIVE_BUFFER_LENGTH-1)
    {
        mbRecBuff[mbRecCount++] = data;
        mb_enable_timer();
    }
    else
    {
        g_mbTimeout = true; //如果接收到的字符超过缓存buff则直接去处理
    }
}

/*
********************************************************************************
*                               mb_rtu_timeout 
*
* Description : 用户程序在定时器中断函数中中调用这个函数。 
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)     : modbus rtu在这个函数中判断一个帧的结束。
*               modbus ascii如果定时器中断，则认为是通信超时。
********************************************************************************
*/
void mb_rtu_timeout(void)
{
    mb_disable_timer();
    g_mbTimeout = true;
}

/*
********************************************************************************
*                               mb_rtu_check_CRC 
*
* Description : 检查rtu帧的CRC校验和
*
* Arguments   : fram:rtu数据帧首地址。length:rtu帧的长度。
*
* Returns     : 返回true:校验成功。返回false校验失败。
*
* Note(s)     : none
********************************************************************************
*/
static bool mb_rtu_check_CRC(mbu8 *frame, mbu16 length)
{
    return mb_rtu_calculate_CRC(frame, length) == 0;
}

/*
********************************************************************************
*                               void mb_rtu_analyze 
*
* Description : 初步分析一个modbus帧，查看功能码是否合法，并根据功能码调用相应的处理函数。
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)     : none
* 
********************************************************************************
*/
static void mb_rtu_analyze(void)
{
    mbu16 devid, funcode; 

    /* 计算设备地址和功能码 */
    if(mbRecCount < 4)    /* modbus RTU上位机的报文最小长度为4 */
        return;

    devid = mbRecBuff[0];
    funcode = mbRecBuff[1];
    /* 如果接收到的设备地址和本设备地址不相符，而且接收到的设备地址不是广播地址，则不做处理 */
    if(devid != MB_SLAVE_DEVID && devid != 0)
        return;

    if(funcode <= MB_FUNCTION_FIELD && mbFunArray[funcode] != NULL)
    {
        /* 检查校验和 */
        if(!mb_rtu_check_CRC(mbRecBuff, mbRecCount))
        {
            mberror(&mbObject, MBERR_CHECKSUM);
        }
        else
        {
            mbObject.frameLength = mbRecCount;
            mbFunArray[funcode](&mbObject);
        }
    }
    else
    {
        mberror(&mbObject, MBERR_CODE);
    }
}

/*
********************************************************************************
*                               mbPoll
*
* Description : modbus轮询处理函数。
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)     : none
* 
********************************************************************************
*/
void mb_rtu_poll(void)
{
    if(g_mbTimeout)
    {
        mb_rtu_analyze();
        mb_clear_receive_buff();
        g_mbTimeout = false;
    }
}

/*
********************************************************************************
*                               mb_rtu_append_CRC 
*
* Description : 计算buff[0]到buff[length-3]之间的CRC校验和
*               并把crc高字节放在buff[length-2]里，把crc低字节放在buff[length-1]里
*
* Arguments   : buff: 要计算CRC校验和的数据
*               length: buff的长度
*
* Returns     : none
*
* Note(s)     : 计算的CRC校验和以网络字节序（大端字节序）的方式加在buff尾部
********************************************************************************
*/
void mb_rtu_append_CRC(mbu8 *buff, mbu16 length)
{
    mbu16 crc = mb_rtu_calculate_CRC(buff, length-2);
    *(mbu16*)(buff+length-2) = htons(crc);
}
