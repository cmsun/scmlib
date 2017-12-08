/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : net.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/12/08 22:02:13
*  @Last Modified  : 2017/12/08 22:03:15
********************************************************************************
*/

#ifndef NET_H
#define NET_H

#include <stdint.h>

uint16_t htons(uint16_t host);
uint32_t htonl(uint32_t host);
uint16_t ntohs(uint16_t net);
uint32_t ntohl(uint32_t net);

#endif /* end of include guard: NET_H */
