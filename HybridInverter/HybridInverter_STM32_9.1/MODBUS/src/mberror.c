/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : mbError.c
*  @Author         : scm 351721714@qq.com
*  @Create         : 2016/11/20 13:30:20
*  @Last Modified  : 2018/07/26 21:44:03
********************************************************************************
*/
#include "mberror.h"

void mberror(mbObject_t *object, mbu8 errcode) REENTRANT
{
    if(object->frame[0] == 0) /* 不回复广播 */
        return;

    object->sendBuff[0] = object->frame[0];
    object->sendBuff[1] = object->frame[1] | 0x80; /* 功能码最高位为1说明下位机回复的是一个异常码 */
    object->sendBuff[2] = errcode;
    object->mb_func_append_checksum(object->sendBuff, 5);
    object->mb_func_send(object->sendBuff, 5);
}

