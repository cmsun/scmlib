/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : w25qxx.h
*  @author         : scm 351721714@qq.com
*  @create         : 2019/02/12 22:49:02
********************************************************************************
*/

#ifndef W25QXX_H
#define W25QXX_H

#include <stdint.h>

#define W25QXX_BLOCK_SIZE 8
#define W25QXX_SECTOR_SIZE 4096
#define W25QXX_SECTOR_COUNT 512

//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F

uint8_t W25QXX_ReadSR(void);
void W25QXX_Write_SR(uint8_t sr);
void W25QXX_Write_Enable(void);
void W25QXX_Write_Disable(void);
void W25QXX_Wait_Busy(void);
void W25QXX_PowerDown(void);
void W25QXX_WAKEUP(void);
uint16_t W25QXX_ReadID(void);
void W25QXX_Erase_Sector(uint32_t Dst_Addr);
void W25QXX_Erase_Chip(void) ;
void W25QXX_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void W25QXX_Write_Page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void W25QXX_Write_NoCheck(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void W25QXX_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);


#endif /* end of include guard: W25QXX_H */
