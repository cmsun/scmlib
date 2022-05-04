#include "pwm.h"

typedef struct {
    uint16_t period;        //PWM周期
    float duty_cycles;      //PWM占空比：0% - 100%
} PWM_TypeDef;

PWM_TypeDef pwm = {0};

#ifdef STM8S003

void pwm_init(uint16_t period, float duty_cycles)
{
    #define OPT2_ARF_ADDR 0x4803                                        //option byte 2,设置IO口功能重映射。
    uint8_t opt2 = FLASH_ReadOptionByte(OPT2_ARF_ADDR)>>8;
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    if((opt2 & 0x01) == 0)
        FLASH_ProgramOptionByte(OPT2_ARF_ADDR, (uint8_t)(opt2|(1<<8))); //OPT2 bit0 置1，设置 PC6 为 TIM1_CH1 PWM功能
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    TIM1_DeInit();
    TIM1_TimeBaseInit(16-1, TIM1_COUNTERMODE_UP, 1000-1, 0);            //初始化定时器时钟源为1MHZ，定时器定时周期为1KHZ
    TIM1_ARRPreloadConfig(ENABLE);                                      //使能自动重装
    TIM1_OC1Init(TIM1_OCMODE_PWM2,                                      //设置跳变前的电平（TIM1_OCMODE_PWM1为设置跳变后的电平）
                 TIM1_OUTPUTSTATE_ENABLE,                               //使能输出
                 TIM1_OUTPUTNSTATE_DISABLE,                             //不使能PWM1的互补通道
                 500-1,                                                 //比较值
                 TIM1_OCPOLARITY_LOW,                                   //跳变前为高电平（如果第一个参数为TIM1_OCMODE_PWM1，则跳变后电平为高电平）
                 TIM1_OCNPOLARITY_HIGH,                                 //不考虑互补通道
                 TIM1_OCIDLESTATE_SET,                                  //开启PWM前和停止PWM后电平为低电平
                 TIM1_OCNIDLESTATE_RESET);                              //不考虑互补通道
}

void pwm_set_period(uint16_t period)
{
    pwm.period = period;
    TIM1_SetAutoreload(period);
}

void pwm_set_duty_cycles(float duty_cycles)
{
    pwm.duty_cycles = duty_cycles;
    uint16_t ARR = (TIM1->ARRH<<8)|(TIM1->ARRL);
    TIM1_SetCompare1((ARR+1) * duty_cycles);
}

void pwm_enable(void)
{
    TIM1_CtrlPWMOutputs(ENABLE);    //使能PWM主输出
    TIM1_Cmd(ENABLE);               //开定时器
}

void pwm_disable(pwm_IDLE_STATE state)
{
    TIM1_CtrlPWMOutputs(DISABLE);   //关闭PWM主输出
    TIM1_Cmd(DISABLE);              //关定时器
}

void pwm_period_call(void){}

#else

void pwm_init(uint16_t period, float duty_cycles)
{
    stc_gpio_cfg_t              stcTIM4Port;
    stc_adt_basecnt_cfg_t       stcAdtBaseCntCfg;
    stc_adt_CHxX_port_cfg_t     stcAdtTIM4ACfg;

    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIM4ACfg);
    DDL_ZERO_STRUCT(stcTIM4Port);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);     //端口外设时钟使能
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE);   //ADT外设时钟使能
    
    stcTIM4Port.enDir  = GpioDirOut;
    Gpio_Init(GpioPort2, GpioPin6, &stcTIM4Port);
    Gpio_SetAfMode(GpioPort2, GpioPin6, GpioAf2);               //P26设置为TIM4_CHA
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;               //锯齿波模式
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0;
    Adt_Init(M0P_ADTIM4, &stcAdtBaseCntCfg);                    //ADT载波、计数模式、时钟配置
    
    stcAdtTIM4ACfg.enCap = AdtCHxCompareOutput;                 //比较输出
    stcAdtTIM4ACfg.bOutEn = TRUE;                               //CHA输出使能
    stcAdtTIM4ACfg.enPerc = AdtCHxPeriodHigh;                   //计数值与周期匹配时CHA电平为高电平
    stcAdtTIM4ACfg.enCmpc = AdtCHxCompareInv;                   //计数值与比较值A匹配时，CHA电平翻转
    stcAdtTIM4ACfg.enStaStp = AdtCHxStateSelSS;                 //CHA起始结束电平由STACA与STPCA控制
    stcAdtTIM4ACfg.enStaOut = AdtCHxPortOutLow;                 //CHA起始电平为低
    stcAdtTIM4ACfg.enStpOut = AdtCHxPortOutLow;                 //CHA结束电平为低
    Adt_CHxXPortCfg(M0P_ADTIM4, AdtCHxA, &stcAdtTIM4ACfg);      //端口CHA配置
    
    Adt_SetPeriod(M0P_ADTIM4, 0x1000);                          //周期设置
    Adt_SetCompareValue(M0P_ADTIM4, AdtCompareA, 0x1000/2);     //通用比较基准值寄存器A设置
}

void pwm_set_period(uint16_t period)
{
    pwm.period = period;
    Adt_SetPeriod(M0P_ADTIM4, period);
}

void pwm_set_duty_cycles(float duty_cycles)
{
    pwm.duty_cycles = duty_cycles;
    Adt_SetCompareValue(M0P_ADTIM4, AdtCompareA, M0P_ADTIM4->PERAR*duty_cycles);
}

void pwm_enable(void)
{
    Adt_StartCount(M0P_ADTIM4);
}

void pwm_disable(pwm_IDLE_STATE state)
{
    Adt_StopCount(M0P_ADTIM4);
}

void pwm_period_call(void){}

#endif