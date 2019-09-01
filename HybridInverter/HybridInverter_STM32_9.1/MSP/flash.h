/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : flash.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2018/01/18 10:25:07
*  @Last Modified  : 2018/01/29 16:03:19
********************************************************************************
*/

#ifndef FLASH_H
#define FLASH_H

#include <stdbool.h>
#include "stm32f10x.h"
#include "stm32f10x_flash.h"

// LD和MD开型产品都是1K字节一个页，其它产品为2K字节一个页
#if defined(STM32F10X_LD) || defined(STM32F10X_LD_VL) || defined(STM32F10X_MD) || defined(STM32F10X_MD_VL)
    #define STM32_SECTOR_SIZE 1024
#else
    #define STM32_SECTOR_SIZE 2048
#endif

#define STM32_SECTOR_COUNT  64
#define STM32_FLASH_BASE 0x08000000
#define STM32_FLASH_SIZE (STM32_SECTOR_COUNT*STM32_SECTOR_SIZE)

typedef enum {
    FLASH_OPT_SUCCESS,
    FLASH_OPT_LOWADDR_ERR,
    FLASH_OPT_HIGHADDR_ERR,
    FLASH_OPT_ADDR_ALIGN_ERR, //如果flash操作地址不对齐
} FLASH_OPT_RESULT;

FLASH_OPT_RESULT flashReadByte(uint32_t _addr, uint8_t *_data);
FLASH_OPT_RESULT flashReadBuff(uint32_t _addr, uint8_t *_buff);
FLASH_OPT_RESULT flashReadHalfWord(uint32_t _addr, uint16_t *_data);
FLASH_OPT_RESULT flashReadWord(uint32_t _addr, uint32_t *_data);

/*
 * 以下函数每调用一次都会擦除一次flash页，如果要写入多个数值，最好使用flashWriteBuff
 * 函数，而不是多次调用flashWriteByte、flashWriteHalfWord或者flashWriteWord这三个函数
 */
FLASH_OPT_RESULT flashWriteByte(uint32_t _addr, uint8_t _data);
FLASH_OPT_RESULT flashWriteHalfWord(uint32_t _addr, uint16_t *_data);
FLASH_OPT_RESULT flashWriteWord(uint32_t _addr, uint32_t *_data);
// FLASH_OPT_RESULT flashWriteBuff_NoCheck(uint32_t _addr, uint8_t *_buff, uint32_t _len);
FLASH_OPT_RESULT flashWriteBuff(uint32_t _addr, uint8_t *_buff, uint32_t _len);

#endif /* end of include guard: FLASH_H */
