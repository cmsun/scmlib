/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : mberror.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2016/11/20 13:32:03
*  @Last Modified  : 2016/11/24 11:38:05
********************************************************************************
*/
#ifndef MBERRORH 
#define MBERRORH 

#include "mbconfig.h"
#include "modbus.h"

#define MBMERR_NO       0x00U   /* no error 无任何错误 */
#define MBERR_CODE      0x01U   /* code error 不支持的功能码 */
#define MBERR_ADDR      0x02U   /* address error 地址超出范围 */
#define MBERR_FORMAT    0x03U   /* frame format error 错误的帧格式 */
#define MBERR_FAULT     0x04U   /* fault 下位机机故障。比如请求读取的内容太大，发送缓冲区不足，无法回复 */
#define MBERR_BUSY      0x07U   /* busy 设备忙*/
#define MBERR_CHECKSUM  0x08U   /* checksum error 校验和错误 */

#ifdef __cplusplus
extern "C" {
#endif

void mberror(mbObject_t *object, mbu8 errcode) REENTRANT;

#ifdef __cplusplus
    } //extern "C" {
#endif

#endif

