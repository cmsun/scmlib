/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : GPS.h
*  @author         : scm 351721714@qq.com
*  @create         : 2018/11/29 15:40:08
********************************************************************************
*/

#ifndef GPS_H
#define GPS_H

#include "nmea/nmea.h"

void GPS_init(void);
void GPS_destroy(void);
void GPS_poll(void);
const nmeaINFO *GPS_getInfo(void);

#endif /* end of include guard: GPS_H */
