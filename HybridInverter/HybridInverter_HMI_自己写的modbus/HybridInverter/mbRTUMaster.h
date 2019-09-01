#ifndef MBRTUMASTER_H
#define MBRTUMASTER_H

#include <vector>
#include <list>
#include <stdint.h>

typedef uint8_t   mbu8;
typedef uint16_t  mbu16;
typedef uint32_t  mbu32;

//使用者传入的回调函数
typedef void (*MBCallback)(const std::vector<mbu8> &sendData, const std::vector<mbu8> &recData, void *arg, mbu8 err); 
struct MBMessage 
{
    std::vector<mbu8> sendData;    //发送的数据
    MBCallback callbackFunc;                //回调函数
    void *callbackArg;                      //传给回调函数的参数
    int reSend;                             //重发次数。重发超过三次没有回复则通讯超时

    MBMessage(void)
        :callbackFunc(NULL),callbackArg(NULL),reSend(0)
    {

    }

    MBMessage(const std::vector<mbu8> &data, MBCallback func, void *arg)
        :reSend(0)
    {
        sendData = data;
        callbackFunc = func;
        callbackArg = arg;
    }
};

class RTUMaster
{
private:
    std::vector<mbu8> recBuff; //接收缓冲区
    std::list<MBMessage> sendList;      //发送数据暂存区
    void analyze(std::vector<mbu8> &recData);              //根据不同功能码调用不同的检查函数
    typedef void (RTUMaster::*MBCheck)(std::vector<mbu8> &recData);
    void fmtChk_01_02(std::vector<mbu8> &recData);         //检查回复的数据报文格式是否正确
    void fmtChk_03_04(std::vector<mbu8> &recData);         //检查回复的数据报文格式是否正确
    void fmtChk_05_06(std::vector<mbu8> &recData);         //检查回复的数据报文格式是否正确
    void fmtChk_16(std::vector<mbu8> &recData);            //检查回复的数据报文格式是否正确

protected:
    virtual void enableSendTimer(void) = 0;     //使能发送定时器，用来检测通信超时，定时间隔可以设置比较大的数，如300ms 
    virtual void disableSendTimer(void) = 0;    //关闭发送定时器
    virtual void enableReceiveTimer(void) = 0;  //使能并启动接收定时器，用来检测一个RTU报文的结束，定时间隔为3.5T
    virtual void disableReceiveTimer(void) = 0; //关闭接收定时器
    virtual void lock(void) = 0;                //子类提供一个互斥锁，要在lock()函数中上锁
    virtual void unlock(void) = 0;              //在unlock()函数中解锁
    virtual void rawSend(std::vector<mbu8> &sendData) = 0; //串口发送函数
    //在串口接收中断中调用这个函数，把串口接收到的数据传给recData
    void rawReceive(const std::vector<mbu8> &recData);   
    //使用者提供一个定时器检测回复超时，在定时中断中调用这个函数。定时器必须是单次运行，不能是周期运行。
    //重载enableSendTimer()函数，并在函数中启动这个定时器;重载disableSendTimer()函数，并在函数中关闭这个定时器
    //当发送数据后300ms内未收到回复则调用onSendTimer()函数重发，3次后未收到回复则通迅错误
    void sendTimeout(void);                 
    //使用者提供一个接收定时器，在定时中断中调用这个函数。定时器必须是单次运行，不能是周期运行。
    //重载enableReceiveTimer()函数，并在函数中启动这个定时器;重载disableReceiveTimer()函数，并在函数中关闭这个定时器
    //这个定时器用来检查一个RTU报文的接收完成。定时时长为串口波特率的3.5个T，当接收数据后超过3.5T没有再接收到数据，就调用onReceiveTimer()
    void receiveTimeout(void);              
   
public:
    enum {RECEIVE_LENGTH_MAX = 128U}; //接收数据的最大长度
    static mbu16 computeCRC(const mbu8 *data, std::size_t length);   //计算CRC校验
    void masterSend(const std::vector<mbu8> &sendData, MBCallback callback = NULL, void *arg = NULL);
    void masterFun01(mbu8 devId, mbu16 mbAddr, mbu16 count, MBCallback callback = NULL, void *arg = NULL);
    void masterFun02(mbu8 devId, mbu16 mbAddr, mbu16 count, MBCallback callback = NULL, void *arg = NULL);
    void masterFun03(mbu8 devId, mbu16 mbAddr, mbu16 count, MBCallback callback = NULL, void *arg = NULL);
    void masterFun04(mbu8 devId, mbu16 mbAddr, mbu16 count, MBCallback callback = NULL, void *arg = NULL);
    void masterFun05(mbu8 devId, mbu16 mbAddr, bool status, MBCallback callback = NULL, void *arg = NULL);
    void masterFun06(mbu8 devId, mbu16 mbAddr, mbu16 value, MBCallback callback = NULL, void *arg = NULL);
    void masterFun16(mbu8 devId, mbu16 mbAddr, const std::vector<mbu16> regValue,
                     MBCallback callback = NULL, void *arg = NULL);
    int sendQueue(void); //查询发送队列中等发送的信息数

    typedef union{
        float f;
        mbu32 u;
    } floatU32_t;

    static inline float formatToFloat_noRever(mbu8 byte1, mbu8 byte2, mbu8 byte3, mbu8 byte4)
    {
        floatU32_t fu;
        fu.u = byte1<<24 | byte2<<16 | byte3<<8 | byte4;
        return fu.f;
    }
    static inline float formatToFloat_regRever(mbu8 byte1, mbu8 byte2, mbu8 byte3, mbu8 byte4)
    {
        floatU32_t fu;
        fu.u = byte1<<8 | byte2 | byte3<<24 | byte4<<16;
        return fu.f;
    }
    /*
     * static inline float formatToFloat_allRever(mbu8 byte1, mbu8 byte2, mbu8 byte3, mbu8 byte4)
     * {
     *     floatU32_t fu;
     *     fu.u = byte1 | byte2<<8 | byte3<<16 | byte4<<24;
     *     return fu.f;
     * }
     * static inline float formatToFloat_halfRever(mbu8 byte1, mbu8 byte2, mbu8 byte3, mbu8 byte4)
     * {
     *     floatU32_t fu;
     *     fu.u = byte1<<16 | byte2<<24 | byte3 | byte4<<8;
     *     return fu.f;
     * }
     */

    static inline int formatToInt_noRever(mbu8 byte1, mbu8 byte2, mbu8 byte3, mbu8 byte4)
    {
        return byte1<<24 | byte2<<16 | byte3<<8 | byte4;
    }
    static inline int formatToInt_regRever(mbu8 byte1, mbu8 byte2, mbu8 byte3, mbu8 byte4)
    {
        return byte1<<8 | byte2 | byte3<<24 | byte4<<16;
    }
    /*
     * static inline int formatToInt_allRever(mbu8 byte1, mbu8 byte2, mbu8 byte3, mbu8 byte4)
     * {
     *     return byte1 | byte2<<8 | byte3<<16 | byte4<<24;
     * }
     * static inline int formatToInt_halfRever(mbu8 byte1, mbu8 byte2, mbu8 byte3, mbu8 byte4)
     * {
     *     return byte1<<16 | byte2<<24 | byte3 | byte4<<8;
     * }
     */

public:
    RTUMaster(void);
    virtual ~RTUMaster(void){};
};

#endif
