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

#ifndef QMC5883_H
#define QMC5883_H

#include "hmc5843.h"
/*typedef struct CompassRawData
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
} CompassGaussData;*/

int qmc5883_init(void);
int qmc5883_get_data(CompassRawData *raw, CompassGaussData *gauss);
int qmc5883_get_direction(float *dir, float pitch, float roll);

#endif /* end of include guard: QMC5883_H */
