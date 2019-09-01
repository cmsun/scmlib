#include "widget.h"
#include "ui_widget.h"
#include "QDebug"
#include "QSerialPortInfo"
#include "QSettings"
#include "QFileDialog"

#include "mbRTUMaster.h"

#define MODBUS_SLAVE_ID     1
#define PARAM_REG_CNT       35
#define DEFAULT_PARAM    "./HybridInverter.ini"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    rtu(new QtRTUMaster(this, 500, 5)),
    timer(new QTimer(this)),
    paramName(DEFAULT_PARAM)
{
    ui->setupUi(this);

    //读取当前设备的串口号
    foreach(const QSerialPortInfo &port, QSerialPortInfo::availablePorts())
        ui->serialPort_comboBox->addItem(port.portName());

    //定时100ms读取一次下位机的参数配置
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_modbus_read_param()));

    //读取设备中的配置参数
    read_settings(paramName);

    connect(ui->param_pushButton_0, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_1, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_2, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_3, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_4, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_5, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_6, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_7, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_8, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_9, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_10, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_11, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_12, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_13, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_14, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_15, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_16, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_17, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_18, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_19, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_20, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_21, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_22, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_23, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_24, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_25, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_26, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_27, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_28, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_29, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_30, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_31, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_32, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_33, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
    connect(ui->param_pushButton_34, SIGNAL(clicked()), this, SLOT(on_param_pushButton_clicked()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::read_settings(QString name)
{
    QSettings *settings = new QSettings(name, QSettings::IniFormat);
    for(int i = 0; i < PARAM_REG_CNT; ++i)
    {
        QString boxName = QString("param_comboBox_%1").arg(QString::number(i));
        QString editName = QString("param_lineEdit_%1").arg(QString::number(i));
        QString labelName = QString("param_label_%1").arg(QString::number(i));
        QComboBox *box = ui->param_groupBox->findChild<QComboBox *>(boxName);
        QLineEdit *edit = ui->param_groupBox->findChild<QLineEdit *>(editName);
        QLabel *label = ui->param_groupBox->findChild<QLabel *>(labelName);
        if(box != NULL)
        {
            box->setCurrentIndex(settings->value(boxName).toInt());
            if(label != NULL) label->setText(box->currentText());
        }
        else if(edit != NULL)
        {
            edit->setText(settings->value(editName).toString());
        }
        else
        {
            qDebug("Find widgets error: %s %d", __FILE__, __LINE__);
            return;
        }
    }
    delete settings;
}

void Widget::save_settings(QString name)
{
    QSettings *settings = new QSettings(name, QSettings::IniFormat);
    for(int i = 0; i < PARAM_REG_CNT; ++i)
    {
        QString boxName = QString("param_comboBox_%1").arg(QString::number(i));
        QString editName = QString("param_lineEdit_%1").arg(QString::number(i));
        QComboBox *box = ui->param_groupBox->findChild<QComboBox *>(boxName);
        QLineEdit *edit = ui->param_groupBox->findChild<QLineEdit *>(editName);
        if(box != NULL)
        {
            settings->setValue(boxName, box->currentIndex());
        }
        else if(edit != NULL)
        {
            settings->setValue(editName, edit->text());
        }
        else
        {
            qDebug("Find widgets error: %s %d", __FILE__, __LINE__);
            return;
        }
    }
    delete settings;

}

//设置单个参数
void Widget::on_param_pushButton_clicked()
{
    uint16_t addr, reg;

    //使用Qt的类型转换，将指针恢复为QPushButton类型
    QPushButton *sender = qobject_cast<QPushButton *>(QObject::sender());
    addr = sender->objectName().split('_').last().toInt();

    if(addr >= PARAM_REG_CNT)
    {
        qDebug("Find sender error: %s %d", __FILE__, __LINE__);
        return;
    }
    else
    {
        QString boxName = QString("param_comboBox_%1").arg(QString::number(addr));
        QString editName = QString("param_lineEdit_%1").arg(QString::number((addr)));
        QComboBox *box = ui->param_groupBox->findChild<QComboBox *>(boxName);
        QLineEdit *edit = ui->param_groupBox->findChild<QLineEdit *>(editName);
        if(box != NULL)
        {
            reg = box->currentIndex();
        }
        else if(edit != NULL)
        {
            reg = edit->text().toInt();
        }
        else
        {
            qDebug("Find widgets error: %s %d", __FILE__, __LINE__);
            return;
        }
    }

    qDebug("addr:%d reg:%d", addr, reg);
    rtu->masterFun06(MODBUS_SLAVE_ID, addr, reg);
}

//设置所有参数
void Widget::on_pushButton_3_clicked()
{
    std::vector<mbu16> regs;

    for(int i = 0; i < PARAM_REG_CNT; ++i)
    {
        QString boxName = QString("param_comboBox_%1").arg(QString::number(i));
        QString editName = QString("param_lineEdit_%1").arg(QString::number(i));
        QComboBox *box = ui->param_groupBox->findChild<QComboBox *>(boxName);
        QLineEdit *edit = ui->param_groupBox->findChild<QLineEdit *>(editName);
        if(box != NULL)
        {
            regs.push_back(box->currentIndex());
        }
        else if(edit != NULL)
        {
            regs.push_back(edit->text().toInt());
        }
        else
        {
            qDebug("Find widgets error: %s %d", __FILE__, __LINE__);
            return;
        }
    }

    rtu->masterFun16(MODBUS_SLAVE_ID, 0, regs);
}

//定时100ms不断读取设备参数
void Widget::on_modbus_read_param()
{
    if(rtu->isOpen())
    {
        rtu->masterFun03(MODBUS_SLAVE_ID, 0, PARAM_REG_CNT, Widget::param_display, this);
    }
}

//显示参数
void Widget::param_display(const std::vector<mbu8> &sendData, const std::vector<mbu8> &recData, void *arg, mbu8 err)
{
    static std::vector<mbu8> back;
    Widget *w = (Widget *)arg;

    if(err == 0)
    {
        if(back == recData)
            return;
        else
            back = recData;
    }
    else
    {
        if(err) qDebug("接收错误：%d", err);
        if(sendData[1] != sendData[1]) qDebug("接收到下位机错误码：%d", recData[1]);
        return;
    }

    if(!recData.empty())
    {
        for(int i = 0; i < PARAM_REG_CNT; ++i)
        {
            QString labelName = QString("param_label_%1").arg(QString::number(i));
            QString boxName = QString("param_comboBox_%1").arg(QString::number(i));
            QLabel *label = w->ui->param_groupBox->findChild<QLabel *>(labelName);
            QComboBox *box = w->ui->param_groupBox->findChild<QComboBox *>(boxName);
            if(label != NULL)
            {
                if(box == NULL)
                    label->setText(QString::number((recData[3+2*i]<<8)|recData[4+2*i]));
                else
                    label->setText(box->currentText());
            }
        }
    }
    else
    {
        qDebug("modbus 通信失败");
    }
}

//打开串口
void Widget::on_serialOpen_pushButton_clicked()
{
    if(rtu->isOpen())
    {
        timer->stop();
        rtu->close();
        ui->serialOpen_pushButton->setText("串口已经关闭");
    }
    else
    {
        rtu->setBaudRate(ui->baudrate_comboBox->currentText().toInt());
        rtu->setPortName(ui->serialPort_comboBox->currentText());
        rtu->open(QIODevice::ReadWrite);
        if(rtu->isOpen()) ui->serialOpen_pushButton->setText("串口已经打开");
        timer->start();
    }
}

//保存参数到默认配置文件里
void Widget::on_pushButton_clicked()
{
    save_settings(paramName);
}

//读取其它参数文件
void Widget::on_pushButton_2_clicked()
{
    QString filter = tr("配置文件(*.ini)");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", filter, 0);
    if(!fileName.isEmpty())
    {
        read_settings(fileName);
    }
}

//参数另存为其它文件
void Widget::on_pushButton_4_clicked()
{
    QString filter = tr("配置文件(*.ini)");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), "", filter, 0);
    if(!fileName.isNull())
    {
        save_settings(fileName);
    }
}
