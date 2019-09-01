/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : mbascii.h
*  @author         : scm 351721714@qq.com
*  @create         : 2018/12/20 10:54:33
********************************************************************************
*/

#ifndef MBASCII_H
#define MBASCII_H

#include <stdbool.h>
#include "mbconfig.h"

void mb_ascii_receive(mbu8 data);
void mb_ascii_timeout(void);
void mb_ascii_poll(void);
void mb_ascii_append_LRC(mbu8 *buff, mbu16 length);
void mb_ascii_send(mbu8 *data, mbu16 length);

#endif /* end of include guard: MBASCII_H */
