#include "eeprom.h"

_IIC_TypeDef iic;

void eeprom_init(void)
{
    iic_init(&iic, GPIOB, GPIO_Pin_6, GPIOB, GPIO_Pin_7);
}
    
void eeprom_read(uint16_t addr, uint8_t *data, uint16_t size)
{
#if EEPROM_PAGE_SIZE * EEPROM_PAGE_COUNT <= 256
    //EEPROM容量小于等于256个字节时的读方式
    iic_read_mem(&iic, DEVICE_ADDRESS, addr, data, size);
#else
    //EEPROM容量超过256个字节时的读方式
    iic_read_mem_addr16bit(&iic, DEVICE_ADDRESS, addr, data, size);
#endif
}

//如果addr在页中的偏移是0，则length最大值为EEPROM_PAGE_SIZE
//如果addr在页中的偏移是1，则length最大值为EEPROM_PAGE_SIZE-1
//依次类推
static void eeprom_pageWrite(uint16_t addr, uint8_t *data, uint16_t size)
{
#if EEPROM_PAGE_SIZE * EEPROM_PAGE_COUNT <= 256
    //EEPROM容量小于等于256个字节时的写方式
    iic_write_mem(&iic, DEVICE_ADDRESS, addr, data, size);
#else
    //EEPROM容量超过256个字节时的写方式
    iic_write_mem_addr16bit(&iic, DEVICE_ADDRESS, addr, data, size);
#endif
}

void eeprom_write(uint16_t addr, uint8_t *data, uint16_t size)
{
    int byte2write = 0, indexInPage = 0;
    
    while(size > 0)
    {
        indexInPage = addr % EEPROM_PAGE_SIZE;
        byte2write = EEPROM_PAGE_SIZE - indexInPage;
        if(byte2write > size) byte2write = size;
        
        eeprom_pageWrite(addr, data, byte2write);
        iic_delay_us(10000);    //每写一页延时10ms，等待保存成功才能再写下一页
        
        addr += byte2write;
        data += byte2write;
        size -= byte2write;
    }
}
