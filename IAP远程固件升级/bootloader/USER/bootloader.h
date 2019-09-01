/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : bootloader.h
*  @author         : scm 351721714@qq.com
*  @create         : 2019/01/31 14:21:12
********************************************************************************
*/

#ifndef BOOTLOADER_H
#define BOOTLOADER_H

/*
 * 假设flash有512K，给bootloader分配大小为16K(0x4000)的flash空间，
 * 预留496K(0x7C000)的flash空间，keil工程要做以下设置：
 * bootloader工程设置：
 * 1: Target -> IROM1 : 0x8000000 0x4000
 * 2: Debug -> Settings -> Flash Download -> Programming Algorithm : 0x8000000 0x4000
 * app工程设置：
 * 1: Target -> IROM1 : 0x8004000 0x7C000
 * 2: Debug -> Settings -> Flash Download -> Programming Algorithm : 0x8004000 0x7C000
 * 3: system_stm32f4xxx.c 修改 VECT_TAB_OFFSET 宏定义为 0x4000。bootloader中虽然把中断
 *    向量表设置到了app中的中断向量表的位置，但是app中的system_init()函数会重新再次设置
 *    中断向量表，这个宏定义是中断向量表的位置。
 * 4: 生成bin文件方法 User -> After Build/Rebuild输入
 *    C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bin --output "$Lfirmware.bin" "#L"
 */

#define BOOTLOADER_ADDR 0x8000000                       //bootloader在flash中的地址
#define BOOTLOADER_SIZE 0x8000                          //给bootloader分配32K的flash空间大小
#define APP_ADDR (BOOTLOADER_ADDR | BOOTLOADER_SIZE)    //app固件烧写在flash中的地址
#define APP_SIZE 0x78000                                //给app固件分配480K的flash空间大小

void bootloader_jump_to_app(void);

#endif /* end of include guard: BOOTLOADER_H */
