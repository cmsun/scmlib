/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : flash.c
*  @author         : scm 351721714@qq.com
*  @create         : 2019/01/31 20:47:44
********************************************************************************
*/

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "ff.h"
#include "flash.h"

uint32_t GetSector(uint32_t Address)
{
    uint32_t sector = 0;

    if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
    {
        sector = FLASH_SECTOR_0;  
    }
    else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
    {
        sector = FLASH_SECTOR_1;  
    }
    else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
    {
        sector = FLASH_SECTOR_2;  
    }
    else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
    {
        sector = FLASH_SECTOR_3;  
    }
    else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
    {
        sector = FLASH_SECTOR_4;  
    }
    else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
    {
        sector = FLASH_SECTOR_5;  
    }
    else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
    {
        sector = FLASH_SECTOR_6;  
    }
    else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
    {
        sector = FLASH_SECTOR_7;  
    }
    else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
    {
        sector = FLASH_SECTOR_8;  
    }
    else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
    {
        sector = FLASH_SECTOR_9;  
    }
    else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
    {
        sector = FLASH_SECTOR_10;  
    }
    else /* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11) */
    {
        sector = FLASH_SECTOR_11;
    }

    return sector;
}

int firmware_program(FIL *fil, uint32_t addr, uint32_t size)
{
    uint8_t buff[64];
    uint32_t br;
    uint32_t ledblink = 0;

    /* Get the 1st sector to erase */
    uint32_t FirstSector = GetSector(addr);
    /* Get the number of sector to erase from 1st sector*/
    uint32_t NbOfSectors = GetSector(addr + size) - FirstSector + 1;

    /* Unlock the Flash to enable the flash control register access *************/ 
    HAL_FLASH_Unlock();

    /* Fill EraseInit structure*/
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.Sector = FirstSector;
    EraseInitStruct.NbSectors = NbOfSectors;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
    {
        HAL_FLASH_Lock(); 
        return 1;
    }
    
    /* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
       you have to make sure that these data are rewritten before they are accessed during code
       execution. If this cannot be done safely, it is recommended to flush the caches by setting the
       DCRST and ICRST bits in the FLASH_CR register. */
    __HAL_FLASH_DATA_CACHE_DISABLE();
    __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

    __HAL_FLASH_DATA_CACHE_RESET();
    __HAL_FLASH_INSTRUCTION_CACHE_RESET();

    __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
    __HAL_FLASH_DATA_CACHE_ENABLE();
    
    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
    
    while(f_read(fil, buff, sizeof(buff), &br) == FR_OK)
    {
        for(uint32_t i = 0; i < br; ++i)
        {
            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, buff[i]) == HAL_OK)
            {
                addr++;
            }
            else
            {
                HAL_FLASH_Lock(); 
                return 1;
            }
        }

        if(br == 0)
        {
            HAL_FLASH_Lock(); 
            return 0;
        }
        
        if(ledblink++ > 100)
        {
            ledblink = 0;
            HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
            HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        }
    }
    
    HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);

    /* Lock the Flash to disable the flash control register access (recommended
       to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock(); 
    return 1;
}

