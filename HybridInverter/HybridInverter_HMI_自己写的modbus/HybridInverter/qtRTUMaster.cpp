#include "qtRTUMaster.h"

QtRTUMaster::QtRTUMaster(QObject *parent, int sendTimerInerval, int receiveTimerInterval)
    : QSerialPort(parent)
{
    sendTimer.setSingleShot(true);
    sendTimer.setInterval(sendTimerInerval);
    receiveTimer.setSingleShot(true);
    receiveTimer.setInterval(receiveTimerInterval);
    connect(&sendTimer, SIGNAL(timeout()), this, SLOT(sendTimeout()));
    connect(&receiveTimer, SIGNAL(timeout()), this, SLOT(receiveTimeout()));
    connect(this, SIGNAL(readyRead()), this, SLOT(receive()));
}

void QtRTUMaster::setSendTimerInterval(int interval)
{
    sendTimer.setInterval(interval);
}

void QtRTUMaster::setReceiveTimerInterval(int interval)
{
    receiveTimer.setInterval(interval);
}

void QtRTUMaster::enableSendTimer(void)     //使能发送定时器，用来检测通信超时，定时间隔可以设置比较大的数，如300ms 
{
    sendTimer.start();
}

void QtRTUMaster::disableSendTimer(void)    //关闭发送定时器
{
    sendTimer.stop(); 
}

void QtRTUMaster::enableReceiveTimer(void)   //使能并启动接收定时器，用来检测一个RTU报文的结束，定时间隔为3.5T
{
    receiveTimer.start();
}

void QtRTUMaster::disableReceiveTimer(void) //关闭接收定时器
{
    receiveTimer.stop();
}

void QtRTUMaster::lock(void)                //上锁
{
    mutex.lock();
}

void QtRTUMaster::unlock(void)              //解锁
{
    mutex.unlock(); 
}

void QtRTUMaster::sendTimeout(void)
{
    RTUMaster::sendTimeout();
}

void QtRTUMaster::receiveTimeout(void)
{
    RTUMaster::receiveTimeout();
}

void QtRTUMaster::rawSend(std::vector<mbu8> &sendData)
{
    QSerialPort::write((char *)&sendData[0], sendData.size()); 
}

void QtRTUMaster::receive(void)
{
    QByteArray byteArray = QSerialPort::readAll();
    RTUMaster::rawReceive(std::vector<mbu8>(byteArray.begin(), byteArray.end()));
}

