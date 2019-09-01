/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : update.c
*  @author         : scm 351721714@qq.com
*  @create         : 2019/01/31 23:27:19
********************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include "update.h"
#include "ff.h"
#include "md5.h"
#include "usart.h"
#include "byteorder.h"

//通信错误定义
#define ERR_STORAGE     (1<<0)  //存储器错误
#define ERR_HARD_FAULT  (1<<1)  //设备硬件错误
#define ERR_TASK        (1<<2)  //设备任务错误
#define ERR_SEQUENCE    (1<<3)  //传输错误
#define ERR_MD5         (1<<4)  //文件的MD5校验错误

enum {
    UPDATE_CMD_START = 0,       //开始传输命令
    UPDATE_CMD_END = 1,         //结束传输命令
    UPDATE_CMD_RESET = 2        //复位命令
};

enum {
    DATA_TYPE_FILENAME = 0,     //数据类型：文件名
    DATA_TYPE_CONTENTS = 1,     //数据类型：文件内容
    DATA_TYPE_MD5 = 2           //数据类型：MD5
};

FIL g_hFile = {0};                     //文件句柄
const char g_tempName[] = "temp.dat";  //临时文件名
char g_finalName[32] = {0};            //最终文件名
char *g_pFinalName = NULL;             //如果为NULL则使用默认文件名保存，否则以g_finalName文件名保存

int g_fileSize = 0;                    //文件的大小
int g_dataOffset = 0;                  //期待的文件数据的偏移位置
int g_transmitTimeout = 0;             //文件数据传输超时计数
uint8_t g_MD5[16] = {0};               //保存上位机计算的文件的MD5校验值

static inline void update_send(uint8_t *data, uint16_t len)
{
    HAL_UART_Transmit(&huart1, data, len, 10);
}

uint8_t update_checksum(uint8_t *data, uint16_t len)
{
    uint8_t sum = 0;
    for(int i = 0; i < len; ++i)
    {
        sum ^= data[i];
    }
    return sum;
}

//删除保存文件数据的文件，结束本次传输
static void update_clear_data(void)
{
    f_close(&g_hFile);
    f_unlink(g_tempName);
    
    g_pFinalName = NULL;
    g_fileSize = 0;
    g_dataOffset = 0;
    g_transmitTimeout = 0;
}

static int update_md5_check(FIL *f)
{
    mbedtls_md5_context ctx;
    uint8_t md5[16], buff[32];
    uint32_t br;
    
    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts(&ctx);
    f_rewind(f);
    while(f_read(f, buff, sizeof(buff), &br) == FR_OK && br > 0)
    {
        mbedtls_md5_update(&ctx, buff, br);
    }
    mbedtls_md5_finish(&ctx, md5);
    mbedtls_md5_free(&ctx);
    return memcmp(g_MD5, md5, sizeof(md5)) != 0;
}

//文件传输命令帧处理
void update_Cmd_process(uint8_t *recData, uint16_t len)
{
    Update_Cmd_Req *req;
    Update_Cmd_Ack ack;
    uint8_t ackflag = 0;

    g_transmitTimeout = 0;                              //传输超时计数清零
    
    req = (Update_Cmd_Req *)recData;
    memset(&ack, 0, sizeof(ack));
    
    if(req->command == UPDATE_CMD_START)                //开始传输文件
    {
//        检测设备状态
//        if()
//        {
//            ackflag |= ERR_HARD_FAULT;
//            ackflag |= ERR_TASK;
//        }
        /*else*/ if(f_open(&g_hFile, g_tempName, FA_CREATE_ALWAYS|FA_READ|FA_WRITE) == FR_OK) //创建临时文件
        {
            g_fileSize = ntohl(req->firmwareSize);      //文件的大小
            g_dataOffset = 0;                           //从0偏移开始接收文件数据
        }
        else                                            //如果创建文件失败
        {
            update_clear_data();                        //清除数据
            ackflag |= ERR_STORAGE;                     //告知上位机：存储错误
        }
    }
    else if(req->command == UPDATE_CMD_END)             //完成文件传输
    {
        if(update_md5_check(&g_hFile) != 0)
        {
            update_clear_data();                        //清除数据
            ackflag |= ERR_MD5;                         //告知上位机：MD5校验错误        
        }
        
        if(ackflag == 0 && f_close(&g_hFile) != FR_OK)  //保存文件
        {
            update_clear_data();                        //清除数据
            ackflag |= ERR_STORAGE;                     //告知上位机：存储错误
        }

        if(ackflag == 0 && g_pFinalName != NULL)        //如果设置了文件名
        {
            f_unlink(g_pFinalName);                     //删除已经存在的文件，否则无法重命名成同名文件
            if(f_rename(g_tempName, g_pFinalName) != FR_OK)
            {
                update_clear_data();                    //清除数据
                ackflag |= ERR_STORAGE;                 //告知上位机：存储错误
            }
        }
        
        g_pFinalName = NULL;
        g_fileSize = 0;
        g_dataOffset = 0;
        g_transmitTimeout = 0;
    }
    else if(req->command == UPDATE_CMD_RESET)           //复位单片机，执行固件升级
    {
        //复位程序，然后从bootloader启动并执行烧写固件
//        __set_FAULTMASK(1);                             //关闭所有中断
//        NVIC_SystemReset();                             //软件复位
        
        //跳回bootloader程序中执行烧写固件。bootloader程序在0x8000000地址处。
//        typedef void (*BOOTLOADER)(void);
//        BOOTLOADER bootloader = *(BOOTLOADER *)(0x8000000+4);
        void(*bootloader)(void) = *((void(**)(void))(0x8000000+4));
        SCB->VTOR = 0x8000000;
        __ASM {MSR MSP, *(volatile uint32_t*)(0x8000000)}
        bootloader();
    }
    else                                                //数据传输失误，做废先前传输的数据
    {
        update_clear_data();
    }
    
    ack.head = UPDATE_FRAME_HEAD;
    ack.type = req->type + 1;
    ack.length = sizeof(ack) - 5;
    memcpy(&ack.ID, &req->ID, sizeof(ack.ID));
    ack.acknowledge = ackflag;
    ack.reserve = 0;
    ack.checkSum = update_checksum(((uint8_t *)&ack), sizeof(ack)-2);
    ack.tail = UPDATE_FRAME_TAIL;
    update_send((uint8_t *)&ack, sizeof(ack));
}

//文件数据传输帧处理
void update_Transmit_process(uint8_t *recData, uint16_t len)
{
    Update_Transmit_Req *req;
    Update_Transmit_Ack ack;
    uint32_t offset;
    uint8_t type, count, ackflag = 0;
    UINT bw;
    
    g_transmitTimeout = 0;                      //传输超时计数清零
    
    req = (Update_Transmit_Req *)recData;
    type = req->dataType;                       //数据帧传输的数据类型（0：文件名 1：文件内容 2：MD5校验值）
    count = req->sendCnt;                       //本次传输的文件数据字节数
    offset = ntohl(req->firmwareIndex);         //本次传输的第一个数据在文件中的偏移值
    
    if(type == DATA_TYPE_FILENAME)              //传输文件名
    {
        memcpy(g_finalName, &req->data, count);
        g_pFinalName = g_finalName;
    }
    else if(type == DATA_TYPE_CONTENTS)         //传输文件内容
    {
        if((g_dataOffset != offset)             //检查偏移值是否对得上
            || (offset + count > g_fileSize)    //检查接收的文件数据是否已经超过原先约定的数值
            || (count + 24 != len))             //检查传输的文件数据和帧长度是否对得上，以免数组越界
        {
            update_clear_data();
            ackflag |= ERR_SEQUENCE;            //传输错误
            goto out;
        }
    
        if(f_write(&g_hFile, &req->data, count, &bw) != FR_OK)
        {
            update_clear_data();
            ackflag |= ERR_STORAGE;             //存储错误
        }
    
        //本次传输的数据存储无异常，等待接收后面的数据。
        if(ackflag == 0)
        {
            g_dataOffset += count;
        }
    }
    else if(type == DATA_TYPE_MD5)              //传输文件MD5校验值
    {
        if(count != sizeof(g_MD5))
            ackflag |= ERR_MD5;
        else
            memcpy(g_MD5, &req->data, sizeof(g_MD5));
    }

out:    
    ack.head = UPDATE_FRAME_HEAD;
    ack.type = req->type + 1;
    ack.length = sizeof(ack) - 5;
    memcpy(&ack.ID, &req->ID, sizeof(ack.ID));
    ack.acknowledge = ackflag;
    ack.nextOffset = htonl(g_dataOffset);
    ack.reserve = 0;
    ack.checkSum = update_checksum((uint8_t *)&ack, sizeof(ack)-2);
    ack.tail = UPDATE_FRAME_TAIL;
    update_send((uint8_t *)&ack, sizeof(ack));
}

//数据传输中断检测。在10ms或者20ms任务中调用这个函数。
void update_timeout_check(void)
{
    if(g_fileSize != 0 && g_transmitTimeout++ > 500)
    {
        update_clear_data();
    }
}
