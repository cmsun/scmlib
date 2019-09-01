/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : byteorder.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/12/08 22:02:13
*  @Last Modified  : 2018/07/25 21:38:55
********************************************************************************
*/
#ifndef BYTEORDER_H
#define BYTEORDER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t htons(uint16_t host);
uint32_t htonl(uint32_t host);
uint64_t htonll(uint64_t host);
uint16_t ntohs(uint16_t net);
uint32_t ntohl(uint32_t net);
uint64_t ntohll(uint64_t net);

#ifdef __cplusplus
    } //extern "C" {
#endif

#endif /* end of include guard: BYTEORDER_H */
