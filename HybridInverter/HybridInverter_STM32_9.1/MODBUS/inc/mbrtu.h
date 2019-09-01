/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : mbrtu.h
*  @author         : scm 351721714@qq.com
*  @create         : 2018/12/20 11:25:51
********************************************************************************
*/

#ifndef MBRTU_H
#define MBRTU_H

#include "mbconfig.h"

void mb_rtu_receive(mbu8 data);
void mb_rtu_timeout(void);
void mb_rtu_poll(void);
void mb_rtu_append_CRC(mbu8 *buff, mbu16 length);

#endif /* end of include guard: MBRTU_H */
