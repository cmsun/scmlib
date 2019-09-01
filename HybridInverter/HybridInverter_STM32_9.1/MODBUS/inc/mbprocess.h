/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : mbprocess.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2016/11/20 13:32:23
*  @Last Modified  : 2016/11/21 15:58:06
********************************************************************************
*/

#ifndef MBPROCESS_H
#define MBPROCESS_H

#include "mbconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

void mb_process01(mbu8 *frame, mbu8 *sendbuff);
void mb_process02(mbu8 *frame, mbu8 *sendbuff);
void mb_process03(mbu8 *frame, mbu8 *sendbuff);
void mb_process04(mbu8 *frame, mbu8 *sendbuff);
void mb_process05(mbu8 *frame, mbu8 *sendbuff);
void mb_process06(mbu8 *frame, mbu8 *sendbuff);
void mb_process16(mbu8 *frame, mbu8 *sendbuff);

#ifdef __cplusplus
    } //extern "C" {
#endif

#endif /* end of include guard: MBPROCESS_H */
