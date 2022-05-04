#include "tick.h"
#include "iwdg.h"

void iwdg_init(void)
{
#ifdef STM8S003
    if(RST_GetFlagStatus(RST_FLAG_IWDGF))
    {
        RST_ClearFlag(RST_FLAG_IWDGF);
    }
    IWDG_Enable();
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_256);
    IWDG_SetReload(0xFF);
    IWDG_ReloadCounter();
#else
    Sysctrl_SetPeripheralGate(SysctrlPeripheralWdt, TRUE);
    Wdt_Init(WdtResetEn, WdtT820ms);
    Wdt_Start();
#endif
}

void iwdg_feed(void)
{
    static uint32_t tick = 0;
    if(tick_value() - tick > 500)
    {
        tick = tick_value();
#ifdef STM8S003
        IWDG_ReloadCounter();
#else
        Wdt_Feed();
#endif
    }
}