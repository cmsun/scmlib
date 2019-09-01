/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : flash.c
*  @Author         : scm 351721714@qq.com
*  @Create         : 2018/01/18 10:25:00
*  @Last Modified  : 2018/02/06 11:48:28
********************************************************************************
*/

#include "flash.h"

FLASH_OPT_RESULT flashWriteHalfWord(uint32_t _addr, uint16_t _data)
{
    uint16_t save[STM32_SECTOR_SIZE/2];
    int page = (_addr - STM32_FLASH_BASE) / STM32_SECTOR_SIZE;
    int index = (_addr - STM32_FLASH_BASE) % STM32_SECTOR_SIZE;
    uint32_t page_start = STM32_FLASH_BASE + page * STM32_SECTOR_SIZE;

    if(_addr < STM32_FLASH_BASE)
        return FLASH_OPT_LOWADDR_ERR;
    else if(_addr > STM32_FLASH_BASE + STM32_FLASH_SIZE - 2)
        return FLASH_OPT_HIGHADDR_ERR;
    else if(_addr % 2)
        return FLASH_OPT_ADDR_ALIGN_ERR;

    for(int i = 0; i < STM32_SECTOR_SIZE/2; ++i)
        save[i] = *((uint16_t *)page_start + i);
    save[index] = _data;
    FLASH_Unlock();
    FLASH_ErasePage(page);
    for(int i = 0; i < STM32_SECTOR_SIZE/2; ++i)
        *((uint16_t *)page_start + i) = save[i];
    FLASH_Lock();

    return FLASH_OPT_SUCCESS;
}

FLASH_OPT_RESULT flashWriteWord(uint32_t _addr, uint32_t _data)
{
    uint32_t save[STM32_SECTOR_SIZE/4];
    int page = (_addr - STM32_FLASH_BASE) / STM32_SECTOR_SIZE;
    int index = (_addr - STM32_FLASH_BASE) % STM32_SECTOR_SIZE;
    uint32_t page_start = STM32_FLASH_BASE + page * STM32_SECTOR_SIZE;

    if(_addr < STM32_FLASH_BASE)
        return FLASH_OPT_LOWADDR_ERR;
    else if(_addr > STM32_FLASH_BASE + STM32_FLASH_SIZE - 4)
        return FLASH_OPT_HIGHADDR_ERR;
    else if(_addr % 4)
        return FLASH_OPT_ADDR_ALIGN_ERR;

    for(int i = 0; i < STM32_SECTOR_SIZE/4; ++i)
        save[i] = *((uint32_t *)page_start + i);
    save[index] = _data;
    FLASH_Unlock();
    FLASH_ErasePage(page);
    for(int i = 0; i < STM32_SECTOR_SIZE/4; ++i)
        *((uint16_t *)page_start + i) = save[i];
    FLASH_Lock();
}

FLASH_OPT_RESULT flashWriteBuff(uint32_t _addr, uint16_t *_buff, uint32_t _len)
{
    uint16_t save[STM32_SECTOR_SIZE/2];
    int page = (_addr - STM32_FLASH_BASE) / STM32_SECTOR_SIZE;
    int index = (_addr - STM32_FLASH_BASE) % STM32_SECTOR_SIZE;
    uint32_t page_start = STM32_FLASH_BASE + page * STM32_SECTOR_SIZE;
    uint32_t page_end = page_start + STM32_SECTOR_SIZE;
    int remain = 0;
    if(page_end - index > _len)
        remain = _len - (page_end - index);

    /*
     * if(_addr < STM32_FLASH_BASE)
     *     return FLASH_OPT_LOWADDR_ERR;
     * else if(_addr > STM32_FLASH_BASE + STM32_FLASH_SIZE - 2)
     *     return FLASH_OPT_HIGHADDR_ERR;
     * else if(_addr % 2)
     *     return FLASH_OPT_ADDR_ALIGN_ERR;
     */

    for(int i = 0; i < STM32_SECTOR_SIZE/2; ++i)
        save[i] = *((uint16_t *)page_start + i);
    FLASH_Unlock();
    FLASH_ErasePage(page);
    for(int i = 0; i < STM32_SECTOR_SIZE/2; ++i)
        *((uint16_t *)page_start + i) = save[i];
    FLASH_Lock();

    return FLASH_OPT_SUCCESS;
}
