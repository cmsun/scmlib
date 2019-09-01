#ifndef EEPROM_H
#define EEPROM_H

#include "stdint.h"
#include "iic.h"

#define DEVICE_ADDRESS      0x50
#define EEPROM_PAGE_COUNT   32
#define EEPROM_PAGE_SIZE    8

void eeprom_init(void);
//虽然一个字节的地址能表示256个字节，但是size必须要用uint16_t来表示，否则如果一次要读取256个字节时
//将会出错。对于一个uint8_t来说256十六进制表示为0xff，但是对于立即数或者多个字节以上的变量来说，256
//十六进制表示为0x00000100，那么用立即数或者两个字节以上的变量来给size变量赋值256时会导致size的值为0
void eeprom_read(uint16_t addr, uint8_t *data, uint16_t size);
void eeprom_write(uint16_t addr, uint8_t *data, uint16_t size);

#endif

