/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : GPS.c
*  @author         : scm 351721714@qq.com
*  @create         : 2018/11/29 14:32:49
********************************************************************************
*/

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "GPS.h"

nmeaINFO g_info;
nmeaPARSER g_parser;

#define GPS_BUFF_LEN 128
char g_buff[GPS_BUFF_LEN];// = "$GNGGA,092600.40,2233.1234707,N,11356.1491492,E,1,16,1.0,64.083,M,-3.483,M,,*53\r\n";
int g_receive_index = 0;
bool g_receive_success = false;

void __aeabi_assert(const char *expr, const char *file, int line)
{
	while(1)
		;
}

unsigned char GPS_checksum(unsigned char *data, int len)
{
	unsigned char cksum = 0;
	for(int i = 0; i < len; ++i)
		cksum ^= data[i];
	return cksum;
}

void GNGGA_convert_to_GPGGA(char *buff)
{
	char hex[5] = {0};
	int len;
	unsigned char cksum;
	if(strncmp(buff, "$GNGGA", 6) == 0)
	{
		buff[2] = 'P';
		len = strlen(buff);
		cksum  = GPS_checksum((unsigned char *)buff + 1, len - 6);
		snprintf(hex, sizeof(hex), "%x", cksum);
		buff[len - 4] = hex[0];
		buff[len - 3] = hex[1];
	}
}

void GPS_init(void)
{
    nmea_zero_INFO(&g_info);
    nmea_parser_init(&g_parser);
}

void GPS_destroy(void)
{
    nmea_parser_destroy(&g_parser);
}

void GPS_poll(void)
{
    if(g_receive_success) 
    {
        g_receive_success = false;
		GNGGA_convert_to_GPGGA(g_buff);
        nmea_parse(&g_parser, g_buff, (int)strlen(g_buff), &g_info);
    }
}

const nmeaINFO *GPS_getInfo(void)
{
    return &g_info;
}

void USART1_Callback(unsigned short byte)
{
	if(g_receive_success)
		return;
	
    if(g_receive_index == 0)
    {
        if(byte == '$')
            g_buff[g_receive_index++] = byte;
    }
    else
    {
        g_buff[g_receive_index++] = byte;

        if(byte == '\n')
        {
            if(g_receive_index > 2 && g_buff[g_receive_index-2] == '\r')
                g_receive_success = true;
            g_receive_index = 0;
        }
        else if(g_receive_index == GPS_BUFF_LEN)
        {
            g_receive_index = 0;
        }
    }
}
