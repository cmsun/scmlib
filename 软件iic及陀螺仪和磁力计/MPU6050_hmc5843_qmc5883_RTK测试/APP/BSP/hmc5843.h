/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : hmc5843.h
*  @author         : scm 351721714@qq.com
*  @create         : 2018/11/09 21:17:04
********************************************************************************
*/

#ifndef HMC5843_H
#define HMC5843_H

typedef struct CompassRawData
{
    short x;
    short y;
    short z;
} CompassRawData;

typedef struct CompassGaussData
{
	float x;
	float y;
	float z;
} CompassGaussData;

int hmc5843_init(void);
int hmc5843_get_data(CompassRawData *raw, CompassGaussData *gauss);
int hmc5843_get_direction(float *dir, float pitch, float roll);

#endif /* end of include guard: HMC5843_H */
