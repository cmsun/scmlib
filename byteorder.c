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

uint64_t htonll(uint64_t host)
{
    uint64_t net;
    uint8_t *pNet = (uint8_t *)&net;
    uint8_t *pHost = (uint8_t *)&host;
    pNet[0] = pHost[7];
    pNet[1] = pHost[6];
    pNet[2] = pHost[5];
    pNet[3] = pHost[4];
    pNet[4] = pHost[3];
    pNet[5] = pHost[2];
    pNet[6] = pHost[1];
    pNet[7] = pHost[0];
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

uint64_t ntohll(uint64_t net)
{
    uint64_t temp;
    uint32_t *pTemp = (uint32_t *)&temp;
    uint32_t *pNet = (uint32_t *)&net;
    pTemp[0] = ntohl(pNet[1]);
    pTemp[1] = ntohl(pNet[0]);
    return temp;
}
