/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : byteorder.c
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/12/08 22:01:31
*  @Last Modified  : 2017/12/08 22:03:26
********************************************************************************
*/

#include "byteorder.h"

uint16_t htons(uint16_t host)
{
    uint16_t net;
    uint8_t *p = (uint8_t *)&net;
    p[0] = (host & 0xff00) >> 8;
    p[1] = host & 0xff;
    return net;
}

uint32_t htonl(uint32_t host)
{
    uint32_t net;
    uint8_t *p = (uint8_t *)&net;
    p[0] = (host & 0xff000000) >> 24;
    p[1] = (host & 0xff0000) >> 16;
    p[2] = (host & 0xff00) >> 8;
    p[3] = host & 0xff;
    return net;
}

uint16_t ntohs(uint16_t net)
{
    uint8_t *p = (uint8_t *)&net;
    return p[0]<<8 | p[1];
}

uint32_t ntohl(uint32_t net)
{
    uint8_t *p = (uint8_t *)&net;
    return p[0]<<24 | p[1]<<16 | p[2]<<8 | p[3];
}
