#include <algorithm>
#include "mbRTUMaster.h"
#include "mbError.h"

const mbu8 crcHiTalbe[] = // CRC高字节值表
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 

    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,

    0x00, 0xC1, 0x81, 0x40
};

const mbu8 crcLoTalbe[] = // CRC低字节值表
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,

    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,

    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,

    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,

    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,

    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,

    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 

    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,

    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,

    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,

    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,

    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 

    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,

    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,

    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,

    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,

    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,

    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,

    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,

    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,

    0x41, 0x81, 0x80, 0x40
};

//modbus URT 计算crc校验码
mbu16 RTUMaster::computeCRC(const mbu8 *data, std::size_t length)
{
    mbu8 crcHi = 0xFF;  //CRC高字节初始化 
    mbu8 crcLo = 0xFF;  //CRC低字节初始化
    mbu32 index;        //CRC循环中的索引

    while (length--)
    {
        index = crcHi ^ *data++ ;
        crcHi = crcLo ^ crcHiTalbe[index];
        crcLo = crcLoTalbe[index] ;
    }

    return ((crcHi << 8) | crcLo) ;
}

RTUMaster::RTUMaster(void)
{
    recBuff.reserve(RECEIVE_LENGTH_MAX); //给接收数据缓冲区分配空间
}

//modbus master发送modbus报文，并进行超时检查
void RTUMaster::masterSend(const std::vector<mbu8> &sendData, MBCallback callback, void *arg)
{
    lock();
    sendList.push_back(MBMessage(sendData, callback, arg));
    unlock();

    if(sendList.size() == 1)
    {
        rawSend(sendList.front().sendData);    //串口发送数据
        enableSendTimer();                     //启动发送定时器
    }
}

void RTUMaster::sendTimeout(void)
{
    MBMessage *front;

    disableSendTimer();

    if(sendList.empty())
    {
        recBuff.clear();
        return;
    }

    front = &sendList.front();

    if((++front->reSend) == 3)    
    {
        if(front->callbackFunc) 
        {
            front->callbackFunc(front->sendData,
                                std::vector<mbu8>(),
                                front->callbackArg,
                                MODBUS_TIMEOUT_ERROR);     //modbus通信超时
        }
        recBuff.clear();
        lock();
        sendList.pop_front();
        unlock();
    }

    if(!sendList.empty()) //如果sendList中还有等待发送的数据就继续发送
    {
        rawSend(sendList.front().sendData); 
        enableSendTimer();
    }
}

//数据接收函数，使用者在串口接收中断中调用这个函数
void RTUMaster::rawReceive(const std::vector<mbu8> &recData) 
{
    disableReceiveTimer();    

    int addCount = 0;
    if(recBuff.size() + recData.size() >= RECEIVE_LENGTH_MAX)
    {
        addCount = RECEIVE_LENGTH_MAX - recBuff.size();
        for(int i = 0; i < addCount; ++i)
            recBuff.push_back(recData[i]);
        analyze(recBuff);
        recBuff.clear();
    }
    else
    {
        addCount = recData.size();
        for(int i = 0; i < addCount; ++i)
            recBuff.push_back(recData[i]);
        enableReceiveTimer();
    }
}

//收到数据后有3.5T的时间未再收到数据，则判定一个RTU报文结束
void RTUMaster::receiveTimeout(void)
{
    disableReceiveTimer();    
    analyze(recBuff);
    recBuff.clear();
}

//根据不同功能码调用各自的报文分析函数
void RTUMaster::analyze(std::vector<mbu8> &recData)  
{
    mbu8 err;
    mbu16 crc;
    MBMessage *front;
    std::vector<mbu8> *sendData;

    static const MBCheck funcArray[] = { 
        NULL,
        &RTUMaster::fmtChk_01_02,
        &RTUMaster::fmtChk_01_02,
        &RTUMaster::fmtChk_03_04,
        &RTUMaster::fmtChk_03_04,
        &RTUMaster::fmtChk_05_06,
        &RTUMaster::fmtChk_05_06,
        NULL, NULL, NULL, NULL,
        NULL,NULL, NULL, NULL, NULL,
        &RTUMaster::fmtChk_16
    };    

    disableSendTimer();

    if(sendList.empty())
    {
        err = MODBUS_UNEXPECT_ERROR;
        return;
    }

    front = &sendList.front();
    sendData = &front->sendData;

    //检查接收的数据长度
    if(recData.size() < 4)
    {
        err = MODBUS_FORMAT_ERROR;   
        goto error;
    }

    //检查设备地址
    if(recData[0] != sendData->at(0))
    {
        err = MODBUS_DEVID_ERROR;
        goto error;
    }

    //检查校验码
    crc = (recData[recData.size()-2] << 8) | recData.back();
    if(crc != computeCRC(&recData[0], recData.size()-2))
    {
        err = MODBUS_CHKSUM_ERROR;
        goto error;
    }

    //查检功能码
    if(recData[1] < sizeof(funcArray)/sizeof(MBCheck) &&
            funcArray[recData[1]] != NULL)
    {
        (this->*funcArray[recData[1]])(recData);    //调用报文分析函数
    }
    else
    {
        if(recData[1] == sendData->at(1)+0x80)
        {
            err = MODBUS_SLAVE_ERROR;
        }

error:
        if(front->callbackFunc) 
        {
            front->callbackFunc(*sendData, recData, front->callbackArg, err);
        }
    }

    lock();
    sendList.pop_front();
    unlock();

    if(!sendList.empty())
    {
        rawSend(sendList.front().sendData); 
        enableSendTimer(); 
    }
}

//功能码01 02的报文格式分析函数
void RTUMaster::fmtChk_01_02(std::vector<mbu8> &recData)
{
    mbu8 err = 0;
    MBMessage &front = sendList.front();
    std::vector<mbu8> &sendData = front.sendData;

    //master读取的位变量总共占用的字节数
    mbu16 readbyte = ((sendData[4]<<8 | sendData[5]) + 7)/8;

    //检查读取的位变量数和返回的字节数是否对得上
    if(recData.size() != readbyte+5U || recData[2] != readbyte)
    {
        err = MODBUS_FORMAT_ERROR; 
    }

    if(front.callbackFunc) 
    {
        front.callbackFunc(sendData, recData, front.callbackArg, err); 
    }
}

//功能码03 04的报文格式分析函数
void RTUMaster::fmtChk_03_04(std::vector<mbu8> &recData)
{
    mbu8 err = 0;
    MBMessage &front = sendList.front();
    std::vector<mbu8> &sendData = front.sendData;

    //master读取的寄存器变量总共占用的字节数
    mbu16 readbyte = (sendData[4]<<8 | sendData[5]) * 2;

    //检查读取的寄存器字节数和返回的字节数是否对得上
    if(recData.size() != readbyte+5U || recData[2] != readbyte)
    {
        err = MODBUS_FORMAT_ERROR; 
    }

    if(front.callbackFunc) 
    {
        front.callbackFunc(sendData, recData, front.callbackArg, err); 
    }
}

//功能码05 06的报文格式分析函数
void RTUMaster::fmtChk_05_06(std::vector<mbu8> &recData)
{
    mbu8 err = 0;
    MBMessage &front = sendList.front();
    std::vector<mbu8> &sendData = front.sendData;

    if(recData != sendData)
    {
        err = MODBUS_FORMAT_ERROR;
    }

    if(front.callbackFunc) 
    {
        front.callbackFunc(sendData, recData, front.callbackArg, err); 
    }
}

//功能码10的报文格式分析函数
void RTUMaster::fmtChk_16(std::vector<mbu8> &recData)
{
    mbu8 err = 0;
    MBMessage &front = sendList.front();
    std::vector<mbu8> &sendData = front.sendData;

    //检查报文长度
    if(recData.size() != 8)
    {
        err = MODBUS_FORMAT_ERROR; 
        goto callback;
    }

    if(!equal(recData.begin(), --(--(--recData.end())), sendData.begin()))
    {
        err = MODBUS_FORMAT_ERROR;
        goto callback;
    }

callback:
    if(front.callbackFunc) 
    {
        front.callbackFunc(sendData, recData, front.callbackArg, err); 
    }
}

void RTUMaster::masterFun01(mbu8 devId, mbu16 mbAddr, mbu16 count, MBCallback callback, void *arg)
{
    std::vector<mbu8> frame;

    frame.reserve(8);                  //RTU功能码1的报文长度为8
    frame.push_back(devId);            //设备地址
    frame.push_back(0x01);             //功能码01
    frame.push_back(mbAddr >> 8);      //modbus地址高字节
    frame.push_back(mbAddr & 0xff);    //modbus地址低字节
    frame.push_back(count >> 8);       //读取位数高字节
    frame.push_back(count & 0xff);     //读取位数低字节

    mbu16 crc = computeCRC(&frame[0], 6);
    frame.push_back(crc >> 8);          
    frame.push_back(crc & 0xff);

    masterSend(frame, callback, arg);
}

void RTUMaster::masterFun02(mbu8 devId, mbu16 mbAddr, mbu16 count, MBCallback callback, void *arg)
{
    std::vector<mbu8> frame;

    frame.reserve(8);                  //RTU功能码2的报文长度为8
    frame.push_back(devId);            //设备地址
    frame.push_back(0x02);             //功能码02
    frame.push_back(mbAddr >> 8);      //modbus地址高字节
    frame.push_back(mbAddr & 0xff);    //modbus地址低字节
    frame.push_back(count >> 8);       //读取位数高字节
    frame.push_back(count & 0xff);     //读取位数低字节

    mbu16 crc = computeCRC(&frame[0], 6);
    frame.push_back(crc >> 8);
    frame.push_back(crc & 0xff);

    masterSend(frame, callback, arg);
}

void RTUMaster::masterFun03(mbu8 devId, mbu16 mbAddr, mbu16 count, MBCallback callback, void *arg)
{
    std::vector<mbu8> frame;

    frame.reserve(8);                  //RTU功能码3的报文长度为8
    frame.push_back(devId);            //设备地址
    frame.push_back(0x03);             //功能码03
    frame.push_back(mbAddr >> 8);      //modbus地址高字节
    frame.push_back(mbAddr & 0xff);    //modbus地址低字节
    frame.push_back(count >> 8);       //读取寄存器数高字节
    frame.push_back(count & 0xff);     //读取寄存器数低字节

    mbu16 crc = computeCRC(&frame[0], 6);
    frame.push_back(crc >> 8);
    frame.push_back(crc & 0xff);

    masterSend(frame, callback, arg);
}

void RTUMaster::masterFun04(mbu8 devId, mbu16 mbAddr, mbu16 count, MBCallback callback, void *arg)
{
    std::vector<mbu8> frame;

    frame.reserve(8);                  //RTU功能码4的报文长度为8
    frame.push_back(devId);            //设备地址
    frame.push_back(0x04);             //功能码04
    frame.push_back(mbAddr >> 8);      //modbus地址高字节
    frame.push_back(mbAddr & 0xff);    //modbus地址低字节
    frame.push_back(count >> 8);       //读取模拟量输入寄存器数高字节
    frame.push_back(count & 0xff);     //读取模拟量输入寄存器数低字节

    mbu16 crc = computeCRC(&frame[0], 6);
    frame.push_back(crc >> 8);
    frame.push_back(crc & 0xff);

    masterSend(frame, callback, arg);
}

void RTUMaster::masterFun05(mbu8 devId, mbu16 mbAddr, bool status, MBCallback callback, void *arg)
{
    std::vector<mbu8> frame;

    frame.reserve(8);                  //RTU功能码5的报文长度为8
    frame.push_back(devId);            //设备地址
    frame.push_back(0x05);             //功能码05
    frame.push_back(mbAddr >> 8);      //modbus地址高字节
    frame.push_back(mbAddr & 0xff);    //modbus地址低字节
    frame.push_back(status ? 0xff : 0x00); //位状态
    frame.push_back(0x00);

    mbu16 crc = computeCRC(&frame[0], 6);
    frame.push_back(crc >> 8);
    frame.push_back(crc & 0xff);

    masterSend(frame, callback, arg);
}

void RTUMaster::masterFun06(mbu8 devId, mbu16 mbAddr, mbu16 value, MBCallback callback, void *arg)
{
    std::vector<mbu8> frame;

    frame.reserve(8);                  //RTU功能码6的报文长度为8
    frame.push_back(devId);            //设备地址
    frame.push_back(0x06);             //功能码06
    frame.push_back(mbAddr >> 8);      //modbus地址高字节
    frame.push_back(mbAddr & 0xff);    //modbus地址低字节
    frame.push_back(value >> 8);       //寄存器值高字节
    frame.push_back(value & 0xff);     //寄存器值低字节

    mbu16 crc = computeCRC(&frame[0], 6);
    frame.push_back(crc >> 8);
    frame.push_back(crc & 0xff);

    masterSend(frame, callback, arg);
}

void RTUMaster::masterFun16(mbu8 devId, mbu16 mbAddr, const std::vector<mbu16> regValue,
                            MBCallback callback, void *arg)
{
    std::vector<mbu8> frame;
    std::vector<mbu8>::size_type size = regValue.size();

    frame.reserve(9 + regValue.size()*2);        //报文长度(预留crc校验的空间，以避免再次分配内存)
    frame.push_back(devId);                      //设备地址
    frame.push_back(0x10);                       //功能码10
    frame.push_back(mbAddr >> 8);                //modbus地址高字节
    frame.push_back(mbAddr & 0xff);              //modbus地址低字节
    frame.push_back(0x00);                       
    frame.push_back(size);                       //寄存器数
    frame.push_back(size*2);                     //寄存器字节数
    for(std::vector<mbu8>::size_type i = 0; i < size; ++i)
    {
        frame.push_back(regValue[i] >> 8);       //寄存器值高字节
        frame.push_back(regValue[i] & 0xff);     //寄存器值低字节
    }

    mbu16 crc = computeCRC(&frame[0], frame.size());
    frame.push_back(crc >> 8);
    frame.push_back(crc & 0xff);

    masterSend(frame, callback, arg);
}

int RTUMaster::sendQueue(void)
{
    return sendList.size();
}
