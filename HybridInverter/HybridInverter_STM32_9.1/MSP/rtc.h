#ifndef RTC_H
#define RTC_H

#include <time.h>
#include <stdint.h>
#include "stm32f10x_conf.h"

int RTC_Initialize(void);
void RTC_SetTimestamp(time_t time, uint32_t prescale);
void RTC_SetDateTime(int16_t year, int8_t month, int8_t day,
                            int8_t hour, int8_t minute, int8_t second);
void RTC_SetAlarmTimestamp(time_t time);
void RTC_SetAlarmTime(int16_t year, int8_t month, int8_t day,
                             int8_t hour, int8_t minute, int8_t second);
time_t RTC_GetTimestamp(void);
void RTC_SetNVICPriority(uint8_t PreemptionPriority, uint8_t SubPriority);
void RTC_SecondITConfig(FunctionalState state);
void RTC_AlarmITConfig(FunctionalState state);

#endif
