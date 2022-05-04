#include "tick.h"
#include "iic.h"
#include "digitron.h"

#define RANGE(x, min, max) ((x)<(min)?(min):((x)>(max)?(max):(x)))

typedef struct {
    enum {
        DECIMAL_PLACE = 0,
        ONES_PLACE = 1,
        TENS_PLACE = 2
    } step;
    float value;
    _IIC_TypeDef iic;
} DigitronStruct_TypeDef;

DigitronStruct_TypeDef Digitron;

//数码管数字显示映射
const uint8_t DigitronFont[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};   

static void AIP650_write(uint16_t addr, uint16_t value)
{
    _IIC_TypeDef *hiic = &Digitron.iic;
    iic_start(hiic);
    iic_send_byte(hiic, addr);
    iic_wait_ack(hiic);
    iic_send_byte(hiic, value);
    iic_wait_ack(hiic);
    iic_stop(hiic); 
}

void digitron_init(void)
{
    Digitron.step = TENS_PLACE;
    Digitron.value = 0;
    
    iic_init(&Digitron.iic, IIC_SCL_GPIO_PORT, IIC_SCL_GPIO_PIN, IIC_SDA_GPIO_PORT, IIC_SDA_GPIO_PIN);
}

static inline void digitron_show_decimal_place(uint8_t value)   //显示小数位
{
    uint8_t index = RANGE(value, 0U, 9U);
    AIP650_write(0x6C, DigitronFont[index]);
}

static inline void digitron_show_ones_place(uint8_t value)      //显示个位
{
    uint8_t index = RANGE(value, 0U, 9U);
    AIP650_write(0x6A, DigitronFont[index]|(1<<7));
}

static inline void digitron_show_tens_place(uint8_t value)      //显示十位
{
    uint8_t index = RANGE(value, 0U, 9U);
    AIP650_write(0x68, DigitronFont[index]);
}

void digitron_enable(void)
{
    AIP650_write(0x48,0x31);
}

void digitron_disable(void)
{
    AIP650_write(0x48,0x00);
}

void digitron_set_value(float value)
{
    Digitron.value = value + 0.05f; //四舍五入，保留一位小数点。
}

void digitron_task_loop()
{
    uint8_t value;
    static uint32_t tick =  0;
    
    if(tick_value() - tick > 100)
    {
        tick = tick_value();
        switch((int)Digitron.step)
        {
            case DECIMAL_PLACE:
                value = (uint32_t)(Digitron.value * 10) % 10;
                digitron_show_decimal_place(value);
                break;
            case ONES_PLACE:
                value = (uint32_t)Digitron.value % 10;
                digitron_show_ones_place(value);
                break;
            case TENS_PLACE:
                value = (uint32_t)Digitron.value / 10;
                digitron_show_tens_place(value);
                break;
            default:
                break;
        }
        Digitron.step++;
        Digitron.step %= 3;
    }
}