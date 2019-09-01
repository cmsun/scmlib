#ifndef QTRTUMASTER_H
#define QTRTUMASTER_H

#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QMutex>

#include "mbRTUMaster.h"

class QtRTUMaster : public QSerialPort, public RTUMaster
{
    Q_OBJECT

private:
    QTimer sendTimer;
    QTimer receiveTimer;
    QMutex mutex;

private slots:
    void sendTimeout(void);
    void receiveTimeout(void);
    void receive(void);

protected:
    virtual void enableReceiveTimer(void);  //使能并启动接收定时器，用来检测一个RTU报文的结束，定时间隔为3.5T
    virtual void disableReceiveTimer(void); //关闭接收定时器
    virtual void enableSendTimer(void);     //使能发送定时器，用来检测通信超时，定时间隔可以设置比较大的数，如300ms 
    virtual void disableSendTimer(void);    //关闭发送定时器
    virtual void lock(void);                //上锁
    virtual void unlock(void);              //解锁
    virtual void rawSend(std::vector<mbu8> &sendData);

public:
    QtRTUMaster(QObject *parent = NULL, int sendTimerInerval = 300, int receiveTimerInterval = 5);
    void setSendTimerInterval(int interval);
    void setReceiveTimerInterval(int interval);
};

#endif
