/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : mbformat.c
*  @Author         : scm 351721714@qq.com
*  @Create         : 2016/11/20 13:29:07
*  @Last Modified  : 2017/01/25 16:26:50
********************************************************************************
*/
#include <stddef.h>
#include "mbconfig.h"
#include "mbchecksum.h"
#include "mberror.h"
#include "mbprocess.h"
#include "mbformat.h"

const mbFormatHandle_t mbFunArray[MB_FUNCTION_FIELD+1] = {
    NULL,

#if MB_COIL_COUNT > 0
    mb_format01,
#else
    NULL,
#endif

#if MB_DISINPUT_COUNT > 0
    mb_format02,
#else
    NULL,
#endif

#if MB_HOLDREG_COUNT > 0
    mb_format03,
#else
    NULL,
#endif

#if MB_INPUTREG_COUNT > 0
    mb_format04,
#else
    NULL,
#endif

#if MB_COIL_COUNT > 0
    mb_format05,
#else
    NULL,
#endif

#if MB_HOLDREG_COUNT > 0
    mb_format06,
#else
    NULL,
#endif

    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,

#if MB_HOLDREG_COUNT > 0
    mb_format16,
#else
    NULL,
#endif
};

#if MB_COIL_COUNT > 0
void mb_format01(mbObject_t *object)
{
    uint8_t *frame, *sendbuff, chksumlen;
    mbu16 framelen, addr, count, sendlen;

    frame = object->frame;
    sendbuff = object->sendBuff;
    framelen = object->frameLength;
    chksumlen = object->chksumLength;

    if(framelen != 6U + chksumlen)
    {
        mberror(object, MBERR_FORMAT);
        return;
    }

    /* 检查modbus地址是否超出范围 */
    addr = frame[2]<<8 | frame[3];
    count = frame[4]<<8 | frame[5];
    if(addr >= MB_COIL_COUNT || (addr+count) > MB_COIL_COUNT)
    {
        mberror(object, MBERR_ADDR);
        return;
    }
    
    /* 检查是否有足够的回复空间 */
    sendlen = 3 + (count%8 ? count/8+1 : count/8) + chksumlen;
    if(sendlen > object->buffCapacity)
    {
        mberror(object, MBERR_FAULT);
        return;
    }

    /* 如果接收的数据报文格式无误，则进行数据处理 */
    mb_process01(frame, sendbuff);
    /* 添加校验码 */
    object->mb_func_append_checksum(sendbuff, sendlen);
    if(frame[0] != 0) object->mb_func_send(sendbuff, sendlen);
}

void mb_format05(mbObject_t *object)
{
    uint8_t *frame, *sendbuff, chksumlen;
    mbu16 framelen, addr;

    frame = object->frame;
    sendbuff = object->sendBuff;
    framelen = object->frameLength;
    chksumlen = object->chksumLength;

    if(framelen != 6U + chksumlen)
    {
        mberror(object, MBERR_FORMAT);
        return;
    }

    /* 检查modbus地址是否超出范围 */
    addr = frame[2]<<8 | frame[3];
    if(addr >= MB_COIL_COUNT)
    {
        mberror(object, MBERR_ADDR);
        return;
    }

    /* 如果接收的数据报文格式无误，则进行数据处理 */
    mb_process05(frame, sendbuff);
    object->mb_func_append_checksum(sendbuff, 6 + chksumlen);
    if(frame[0] != 0) object->mb_func_send(sendbuff, 6 + chksumlen);   
}
#endif

#if MB_HOLDREG_COUNT > 0
void mb_format03(mbObject_t *object)
{
    uint8_t *frame, *sendbuff, chksumlen;
    mbu16 framelen, addr, count, sendlen;

    frame = object->frame;
    sendbuff = object->sendBuff;
    framelen = object->frameLength;
    chksumlen = object->chksumLength;

    if(framelen != 6U + chksumlen)
    {
        mberror(object, MBERR_FORMAT);
        return;
    }

    /* 检查modbus地址是否超出范围 */
    addr = frame[2]<<8 | frame[3];
    count = frame[4]<<8 | frame[5];
    if(addr >= MB_HOLDREG_COUNT || (addr+count) > MB_HOLDREG_COUNT)
    {
        mberror(object, MBERR_ADDR);
        return;
    }
    
    /* 检查是否有足够的回复空间 */
    sendlen = 3 + count*2 + chksumlen;
    if(sendlen > object->buffCapacity)
    {
        mberror(object, MBERR_FAULT);
        return;
    }

    /* 如果接收的数据报文格式无误，则进行数据处理 */
    mb_process03(frame, sendbuff);
    object->mb_func_append_checksum(sendbuff, sendlen);
    if(frame[0] != 0) object->mb_func_send(sendbuff, sendlen);
}

void mb_format06(mbObject_t *object)
{
    uint8_t *frame, *sendbuff, chksumlen;
    mbu16 framelen, addr;

    frame = object->frame;
    sendbuff = object->sendBuff;
    framelen = object->frameLength;
    chksumlen = object->chksumLength;

    if(framelen != 6U + chksumlen)
    {
        mberror(object, MBERR_FORMAT);
        return;
    }

    /* 检查modbus地址是否超出范围 */
    addr = frame[2]<<8 | frame[3];
    if(addr >= MB_HOLDREG_COUNT)
    {
        mberror(object, MBERR_ADDR);
        return;
    }

    /* 如果接收的数据报文格式无误，则进行数据处理 */
    mb_process06(frame, sendbuff);
    object->mb_func_append_checksum(sendbuff, 6 + chksumlen);
    if(frame[0] != 0) object->mb_func_send(sendbuff, 6 + chksumlen);
}

void mb_format16(mbObject_t *object)
{
    uint8_t *frame, *sendbuff, chksumlen;
    mbu16 framelen, addr;

    frame = object->frame;
    sendbuff = object->sendBuff;
    framelen = object->frameLength;
    chksumlen = object->chksumLength;

    /* 检查报文格式 */
    if(frame[5]*2 != frame[6] || 
       framelen != frame[6]+7U + chksumlen)
    {
        mberror(object, MBERR_FORMAT);
        return;
    }   

    /* 检查modbus地址是否超出范围 */
    addr = frame[2]<<8 | frame[3];
    if(addr >= MB_HOLDREG_COUNT || (addr+frame[5]) > MB_HOLDREG_COUNT)
    {
        mberror(object, MBERR_ADDR);
        return;
    }

    /* 如果接收的数据报文格式无误，则进行数据处理 */
    mb_process16(frame, sendbuff);
    object->mb_func_append_checksum(sendbuff, 6 + chksumlen);
    if(frame[0] != 0) object->mb_func_send(sendbuff, 6 + chksumlen);
}
#endif

#if MB_DISINPUT_COUNT > 0
void mb_format02(mbObject_t *object) 
{
    uint8_t *frame, *sendbuff, chksumlen;
    mbu16 framelen, addr, count, sendlen;

    frame = object->frame;
    sendbuff = object->sendBuff;
    framelen = object->frameLength;
    chksumlen = object->chksumLength;

    if(framelen != 6U + chksumlen)
    {
        mberror(object, MBERR_FORMAT);
        return;
    }

    /* 检查modbus地址是否超出范围 */
    addr = frame[2]<<8 | frame[3];
    count = frame[4]<<8 | frame[5];
    if(addr >= MB_DISINPUT_COUNT || (addr+count) > MB_DISINPUT_COUNT)
    {
        mberror(object, MBERR_ADDR);
        return;
    }

    /* 检查是否有足够的回复空间 */
    sendlen = 3 + (count%8 ? count/8+1 : count/8) + chksumlen;
    if(sendlen > object->buffCapacity)
    {
        mberror(object, MBERR_FAULT);
        return;
    }

    /* 如果接收的数据报文格式无误，则进行数据处理 */
    mb_process02(frame, sendbuff);
    object->mb_func_append_checksum(sendbuff, sendlen);
    if(frame[0] != 0) object->mb_func_send(sendbuff, sendlen);
}
#endif

#if MB_INPUTREG_COUNT > 0
void mb_format04(mbObject_t *object)
{
    uint8_t *frame, *sendbuff, chksumlen;
    mbu16 framelen, addr, count, sendlen;

    frame = object->frame;
    sendbuff = object->sendBuff;
    framelen = object->frameLength;
    chksumlen = object->chksumLength;

    if(framelen != 6U + chksumlen)
    {
        mberror(object, MBERR_FORMAT);
        return;
    }

    /* 检查modbus地址是否超出范围 */
    addr = frame[2]<<8 | frame[3];
    count = frame[4]<<8 | frame[5];
    if(addr >= MB_INPUTREG_COUNT*4 || (addr+count) > MB_INPUTREG_COUNT*4)
    {
        mberror(object, MBERR_ADDR);
        return;
    }
    
    /* 检查是否有足够的回复空间 */
    sendlen = 3 + count*2 + chksumlen;
    if(sendlen > object->buffCapacity)
    {
        mberror(object, MBERR_FAULT);
        return;
    }

    /* 如果接收的数据报文格式无误，则进行数据处理 */
    mb_process04(frame, sendbuff);
    object->mb_func_append_checksum(sendbuff, sendlen);
    if(frame[0] != 0) object->mb_func_send(sendbuff, sendlen);
}
#endif
