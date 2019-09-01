/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : debug.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/10/22 07:31:30
*  @Last Modified  : 2017/11/16 14:04:54
********************************************************************************
*/

#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG_BUFF_LENGTH 128
//#define DEBUG_WITH_SWO
#define DEBUG_WITH_USART
#define DUSART USART1

#if defined(DEBUG) || defined(_DEBUG)

int __printf(const char *fmt, ...);
#define _debug(fmt, args...) __printf(fmt, ##args)
#define _assert_failed(f, l) (_debug("assert failed: file:%s line:%d\n", f, l), (void)0)
#define _assert(expr) ((expr) ? (void)0 : _assert_failed(__FILE__, __LINE__))

#else

#define _debug(fmt, args...) (void)0
#define _assert(expr) (void)0

#endif /* DEBUG or _DEBUG*/

#endif /* end of include guard: DEBUG_H */
