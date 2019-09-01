#ifndef __MIP_SYSTICK_H
#define __MIP_SYSTICK_H

#include "stm32f10x_rtc.h"

#ifdef __cplusplus
    extern "C" {
#endif

typedef struct {
    void (*pfun)(void *);
    void *arg;
} SYSTICK_CALLBACK;

void systick_init(void);
void systick_setCLKSource(uint32_t SysTick_CLKSource);
void systick_setTimeOut_ms(uint16_t nms, void (*pfun)(void *), void *arg);
void systick_setTimeOut_us(uint16_t nus, void (*pfun)(void *), void *arg);
void systick_mDelay(uint32_t nms);
void systick_uDelay(uint32_t nus);
void systick_ITConfig(FunctionalState NewState);
void systick_setReload(uint32_t reloadVal);
uint32_t systick_getValue(void);
void systick_enable(void);
void systick_disable(void);

#ifdef __cplusplus
    }
#endif

#endif
