/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : mbascii.c
*  @Author         : scm 351721714@qq.com
*  @Create         : 2016/11/23 10:00:00
*  @Last Modified  : 2018/07/27 23:58:22
********************************************************************************
*/

#include <stdio.h>
#include <stdbool.h>
#include "mbconfig.h"
#include "modbus.h"
#include "mbchecksum.h"
#include "mberror.h"
#include "mbformat.h"

static bool g_mbRecAsciiFrame = false;

/*
********************************************************************************
*                               mb_ascii_receive 
*
* Description : 用户程序在串口中断函数中把接收到的数据通过mbSerialReceive函数传给
*               modbus模块进行数据处理。
*
* Arguments   : recvalue: 串口接收到的数据。
*
* Returns     : none
*
* Note(s)     : none
********************************************************************************
*/
void mb_ascii_receive(mbu8 data)
{
    if(g_mbRecAsciiFrame)
        return;

    mb_disable_timer();                       /* 关闭定时器 */

    if(mbRecCount != 0 || data == ':') 
        mbRecBuff[mbRecCount++] = data;

    /* 数据格式符合ascii格式 */
    if(mbRecCount >= 9 && mbRecBuff[mbRecCount-2] == '\r' && mbRecBuff[mbRecCount-1] == '\n')
        g_mbRecAsciiFrame = true;
    else if(mbRecCount == MB_RECEIVE_BUFFER_LENGTH-1)
        mberror(&mbObject, MBERR_CODE);
    else
        mb_enable_timer();
}

/*
********************************************************************************
*                               mb_ascii_timout 
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
void mb_ascii_timeout(void)
{
    mb_disable_timer();
    mb_clear_receive_buff();
}

/*
********************************************************************************
*                               mb_ascii_check_LRC 
*
* Description : 检查ascii帧的LRC校验和
*
* Arguments   : fram:ascii数据帧从字符转二进制后的数据首地址。length:数据的长度。
*
* Returns     : 返回true:校验成功。返回false校验失败。
*
* Note(s)     : none
********************************************************************************
*/
static bool mb_ascii_check_LRC(mbu8 *frame, mbu16 length)
{
    return mb_ascii_calculate_LRC(frame, length-1) == frame[length-1];
}

/*
********************************************************************************
*                               mb_ascii_analyze 
*
* Description : 初步分析一个modbus帧，查看功能码是否合法，并根据功能码调用相应的处理函数。
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)     : 在某些编译器和编译环境下如mingw，sscanf函数似乎只认识h，不认识hh。
*               于是即使用%2hhx声明了要写入的地址是一个char*类型的地址，但sscanf
*               只认识一个h，所以把地址看成了一个short*类型的地址，结果就写入越界了。
*               所以sscanf的安全的使用方法是即使读取的是一个char类型，也要往short*地
*               址里面写。就如同下面的devid和funcode都定义成了mbu16，而不是定义成mbu8。
********************************************************************************
*/
static void mb_ascii_analyze(void)
{
    mbu16 devid, funcode, temp, i; 

    /* 计算设备地址和功能码 */
    if(mbRecCount < 9)   /* modbus ASCII上位机的报文最小长度为9 */
        return;

    sscanf((char *)mbRecBuff+1, "%2hx", &devid);
    sscanf((char *)mbRecBuff+3, "%2hx", &funcode);
    /* 如果接收到的设备地址和本设备地址不相符，或者接收到的设备地址不是广播地址，则不做处理 */
    if(devid != MB_SLAVE_DEVID && devid != 0)
        return;

    if(funcode <= MB_FUNCTION_FIELD && mbFunArray[funcode] != NULL)
    {
        mbObject.frameLength = (mbRecCount-3) / 2;
        for(i = 0; i < mbObject.frameLength; ++i)
        {
            sscanf((char *)mbRecBuff + i*2 + 1, "%2hx", &temp);
            mbRecBuff[i] = temp;
        }

        /* 检查校验和 */
        if(!mb_ascii_check_LRC(mbRecBuff, mbObject.frameLength))
            mberror(&mbObject, MBERR_CHECKSUM);
        else
            mbFunArray[funcode](&mbObject);
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
void mb_ascii_poll(void)
{
    if(g_mbRecAsciiFrame)
    {
        mb_ascii_analyze();
        mb_clear_receive_buff();
        g_mbRecAsciiFrame = false;
    }
}

/*
********************************************************************************
*                               mb_ascii_append_LRC 
*
* Description : 计算buff[0]到buff[length-2]之间的lrc校验和，并把lrc放在buff[length-1]里
*
* Arguments   : buff: 要计算lrc校验和的数据
*               length: buff的长度
*
* Returns     : none
*
* Note(s)     : none
********************************************************************************
*/
void mb_ascii_append_LRC(mbu8 *buff, mbu16 length)
{
    mbu8 lrc = mb_ascii_calculate_LRC(buff, length-1);
    buff[length-1] = lrc;
}

/*
********************************************************************************
*                               mbAsciiSend 
*
* Description : modbus ascii的发送函数。
*
* Arguments   : data:要发送的使用lrc作为校验和的rtu数据帧首地址。data实际指向mbSendBuff。
*               length:数据长度。
*
* Returns     : none
*
* Note(s)     : 将以lrc作为校验和的rtu数据帧转换成ascii数据帧，并从串口发送出去。
********************************************************************************
*/
void mb_ascii_send(mbu8 *data, mbu16 length)
{
    char buff[3];
    mbu16 len = length;
    mbu16 index = MB_SEND_BUFFER_LENGTH - 1;

    mbSendBuff[index--] = '\n';
    mbSendBuff[index--] = '\r';
    while(len)
    {
        snprintf(buff, 3, "%02hx", data[--len]);
        mbSendBuff[index--] = buff[1];
        mbSendBuff[index--] = buff[0];
    }
    mbSendBuff[index] = ':';

    mb_raw_send(mbSendBuff + index, length*2 + 3);
}
