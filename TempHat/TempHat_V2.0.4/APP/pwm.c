#include "pwm.h"

typedef struct {
    uint16_t period;        //PWM����
    float duty_cycles;      //PWMռ�ձȣ�0% - 100%
} PWM_TypeDef;

PWM_TypeDef pwm = {0};

#ifdef STM8S003

void pwm_init(uint16_t period, float duty_cycles)
{
    #define OPT2_ARF_ADDR 0x4803                                        //option byte 2,����IO�ڹ�����ӳ�䡣
    uint8_t opt2 = FLASH_ReadOptionByte(OPT2_ARF_ADDR)>>8;
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    if((opt2 & 0x01) == 0)
        FLASH_ProgramOptionByte(OPT2_ARF_ADDR, (uint8_t)(opt2|(1<<8))); //OPT2 bit0 ��1������ PC6 Ϊ TIM1_CH1 PWM����
    FLASH_Lock(FLASH_MEMTYPE_DATA);
    
    TIM1_DeInit();
    TIM1_TimeBaseInit(16-1, TIM1_COUNTERMODE_UP, 1000-1, 0);            //��ʼ����ʱ��ʱ��ԴΪ1MHZ����ʱ����ʱ����Ϊ1KHZ
    TIM1_ARRPreloadConfig(ENABLE);                                      //ʹ���Զ���װ
    TIM1_OC1Init(TIM1_OCMODE_PWM2,                                      //��������ǰ�ĵ�ƽ��TIM1_OCMODE_PWM1Ϊ���������ĵ�ƽ��
                 TIM1_OUTPUTSTATE_ENABLE,                               //ʹ�����
                 TIM1_OUTPUTNSTATE_DISABLE,                             //��ʹ��PWM1�Ļ���ͨ��
                 500-1,                                                 //�Ƚ�ֵ
                 TIM1_OCPOLARITY_LOW,                                   //����ǰΪ�ߵ�ƽ�������һ������ΪTIM1_OCMODE_PWM1����������ƽΪ�ߵ�ƽ��
                 TIM1_OCNPOLARITY_HIGH,                                 //�����ǻ���ͨ��
                 TIM1_OCIDLESTATE_SET,                                  //����PWMǰ��ֹͣPWM���ƽΪ�͵�ƽ
                 TIM1_OCNIDLESTATE_RESET);                              //�����ǻ���ͨ��
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
    TIM1_CtrlPWMOutputs(ENABLE);    //ʹ��PWM�����
    TIM1_Cmd(ENABLE);               //����ʱ��
}

void pwm_disable(pwm_IDLE_STATE state)
{
    TIM1_CtrlPWMOutputs(DISABLE);   //�ر�PWM�����
    TIM1_Cmd(DISABLE);              //�ض�ʱ��
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
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);     //�˿�����ʱ��ʹ��
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE);   //ADT����ʱ��ʹ��
    
    stcTIM4Port.enDir  = GpioDirOut;
    Gpio_Init(GpioPort2, GpioPin6, &stcTIM4Port);
    Gpio_SetAfMode(GpioPort2, GpioPin6, GpioAf2);               //P26����ΪTIM4_CHA
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;               //��ݲ�ģʽ
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0;
    Adt_Init(M0P_ADTIM4, &stcAdtBaseCntCfg);                    //ADT�ز�������ģʽ��ʱ������
    
    stcAdtTIM4ACfg.enCap = AdtCHxCompareOutput;                 //�Ƚ����
    stcAdtTIM4ACfg.bOutEn = TRUE;                               //CHA���ʹ��
    stcAdtTIM4ACfg.enPerc = AdtCHxPeriodHigh;                   //����ֵ������ƥ��ʱCHA��ƽΪ�ߵ�ƽ
    stcAdtTIM4ACfg.enCmpc = AdtCHxCompareInv;                   //����ֵ��Ƚ�ֵAƥ��ʱ��CHA��ƽ��ת
    stcAdtTIM4ACfg.enStaStp = AdtCHxStateSelSS;                 //CHA��ʼ������ƽ��STACA��STPCA����
    stcAdtTIM4ACfg.enStaOut = AdtCHxPortOutLow;                 //CHA��ʼ��ƽΪ��
    stcAdtTIM4ACfg.enStpOut = AdtCHxPortOutLow;                 //CHA������ƽΪ��
    Adt_CHxXPortCfg(M0P_ADTIM4, AdtCHxA, &stcAdtTIM4ACfg);      //�˿�CHA����
    
    Adt_SetPeriod(M0P_ADTIM4, 0x1000);                          //��������
    Adt_SetCompareValue(M0P_ADTIM4, AdtCompareA, 0x1000/2);     //ͨ�ñȽϻ�׼ֵ�Ĵ���A����
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