/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : mbformat.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2016/11/20 13:28:40
*  @Last Modified  : 2018/07/26 21:44:20
********************************************************************************
*/
#ifndef MBFORMAT_H
#define MBFORMAT_H

#include "mbconfig.h"
#include "modbus.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (* mbFormatHandle_t)(mbObject_t *);
extern const mbFormatHandle_t mbFunArray[];

void mb_format01(mbObject_t *object);
void mb_format02(mbObject_t *object);
void mb_format03(mbObject_t *object);
void mb_format04(mbObject_t *object);
void mb_format05(mbObject_t *object);
void mb_format06(mbObject_t *object);
void mb_format16(mbObject_t *object);

#ifdef __cplusplus
    } //extern "C" {
#endif

#endif /* end of include guard: MBFORMAT_H */
