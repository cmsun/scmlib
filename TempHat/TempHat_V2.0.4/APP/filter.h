#ifndef __FILTER_H
#define __FILTER_H

#include <stdbool.h>

#define SMOOTH_FILT_BUFF_LEN 15

typedef struct SmoothFilter_TypeDef
{
    float buff[SMOOTH_FILT_BUFF_LEN];
    int index;
    float sum;
    bool is_full;
} SmoothFilter_TypeDef;

void smooth_filter_init(SmoothFilter_TypeDef *filter);
float smooth_filter(SmoothFilter_TypeDef *filter, float value);

#endif