/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @fileName       : modbus.h
*  @author         : scm 351721714@qq.com
*  @create         : 2018/12/20 11:55:22
********************************************************************************
*/

#ifndef MODBUS_H
#define MODBUS_H

#include "mbconfig.h"

extern mbu8 mbRecBuff[];
extern mbu8 mbSendBuff[];
extern volatile mbu16 mbRecCount;

//逻辑线圈
#if defined(MB_COIL_COUNT)
extern mbu8 mbCoil[];
#endif

//离散输入量
#if defined(MB_DISINPUT_COUNT)
extern mbu8 mbDisInput[];
#endif

//保持寄存器
#if defined(MB_HOLDREG_COUNT)
extern mbu16 mbHoldReg[];
#endif

//寄存器输入
#if defined(MB_INPUTREG_COUNT)
extern mbu16 InputReg[];
#endif

typedef enum {
    MODBUS_RTU,
    MODBUS_ASCII,
} MODBUS_TYPE;

typedef struct _mbObject_t {
    //如果是RTU帧则frame指向接收缓冲区的第一个字节
    //如果是ASCII帧则会转化成RTU帧然后frame指向生成的RTU帧的第一个字节
    //如果是TCP帧则frame指向其中的RTU起始位置。
    mbu8 *frame;
    //frame的长度
    //对于RTU：frameLength是RTU数据帧的长度
    //对于ASCII: frameLength是除去起始符、终止符之后，再把ASCII字符串转成BYTE后的数据长度
    //对于TCP：frameLength是除去TCP/IP头部之后的长度
    mbu16 frameLength;
	
    //指向mbSendBuff
    mbu8 *sendBuff;	
    //对于RTU：buffCapacity是RTU数据帧的长度
    //对于ASCII: buffCapacity是除去起始符、终止符之后，再把ASCII字符串转成BYTE后的数据长度
    //对于TCP：buffCapacity是除去TCP/IP头部之后的长度
    mbu16 buffCapacity;
	
    //校验码长度，CRC是两个字节，LRC是一个字节。
    mbu8 chksumLength;
	
    void (* mb_func_receive)(mbu8);					//接收函数
    void (* mb_func_send)(mbu8 *, mbu16);			//发送函数
    void (* mb_func_timeout)(void);					//定时器超时函数
    void (* mb_func_poll)(void);					//轮循函数
    void (* mb_func_append_checksum)(mbu8 *, mbu16);			//添加校验码函数
} mbObject_t;

extern mbObject_t mbObject;

void mb_clear_receive_buff(void);

void mb_init(MODBUS_TYPE type); //modbus初始化函数
void mb_poll(void);             //modbus轮询函数
void mb_timeout(void);          //超时定时器的中断函数中调用
void mb_receive(mbu8 data);     //串口接收中断函数中调用

//移植需要实现以下四个函数
void mb_hardware_init(void);                //初始化串口和超时定时器
void mb_raw_send(mbu8 *data, mbu16 length); //数据发送函数
void mb_enable_timer(void);                 //启动超时定时器
void mb_disable_timer(void);                //关闭超时定时器并将超时计数清零

#endif /* end of include guard: MODBUS_H */
