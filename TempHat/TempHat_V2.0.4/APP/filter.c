#include "filter.h"

void smooth_filter_init(SmoothFilter_TypeDef *filter)
{
    filter->index = 0;
    filter->sum = 0;
    filter->is_full = false;
    memset(filter->buff, 0, sizeof(filter->buff));
}

float smooth_filter(SmoothFilter_TypeDef *filter, float value)
{
    float result;

    if(!filter->is_full && filter->index+1 == SMOOTH_FILT_BUFF_LEN)
        filter->is_full = true;

    filter->sum -= filter->buff[filter->index];
    filter->sum += value;
    filter->buff[filter->index] = value;

    if(filter->is_full)
        result = filter->sum / SMOOTH_FILT_BUFF_LEN;
    else
        result = filter->sum / (filter->index + 1);

    filter->index++;
    filter->index %= SMOOTH_FILT_BUFF_LEN;

    return result;
}