#include "ModbusParamWidget.h"
#include "ui_ModbusParamWidget.h"
#include "QDebug"
#include "QSerialPortInfo"
#include "QSettings"
#include "QFileDialog"

#define MODBUS_SLAVE_ADDR   1
#define PARAM_REG_CNT       35
#define DEFAULT_PARAM       "./HybridInverter.ini"

ModbusParamWidget::ModbusParamWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModbusParamWidget),
    timer(new QTimer(this)),
    paramName(DEFAULT_PARAM),
    modbus(new QModbusRtuSerialMaster(this))
{
    ui->setupUi(this);

    modbus->setTimeout(200);        //设置回复超时时间
    modbus->setNumberOfRetries(0);  //设置超时重发次数
    modbus->setInterFrameDelay(20); //设置成帧时间

    //读取当前设备的串口号
    foreach(const QSerialPortInfo &port, QSerialPortInfo::availablePorts())
        ui->serialPort_comboBox->addItem(port.portName());

    //定时200ms读取一次下位机的参数
    timer->setInterval(200);

    //读取本地保存的参数
    read_settings(paramName);

    connect(ui->param_pushButton_0, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_1, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_2, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_3, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_4, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_5, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_6, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_7, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_8, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_9, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_10, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_11, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_12, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_13, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_14, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_15, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_16, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_17, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_18, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_19, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_20, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_21, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_22, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_23, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_24, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_25, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_26, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_27, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_28, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_29, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_30, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_31, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_32, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_33, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));
    connect(ui->param_pushButton_34, SIGNAL(clicked()), this, SLOT(on_modify_single_param()));

    connect(ui->pushButton_modify_all_param, SIGNAL(clicked()), this, SLOT(on_modify_all_param()));
    connect(ui->pushButton_serial_port_open, SIGNAL(clicked()), this, SLOT(on_serial_port_open()));
    connect(ui->pushButton_save_default_settings, SIGNAL(clicked()), this, SLOT(on_save_default_settings()));
    connect(ui->pushButton_load_settings_from, SIGNAL(clicked()), this, SLOT(on_load_settings_from()));
    connect(ui->pushButton_save_settings_as, SIGNAL(clicked()), this, SLOT(on_save_settings_as()));
    connect(timer, SIGNAL(timeout()), this, SLOT(on_modbus_read_param()));
}

ModbusParamWidget::~ModbusParamWidget()
{
    delete ui;
}

//设置单个参数
void ModbusParamWidget::on_modify_single_param()
{
    auto type = QModbusDataUnit::RegisterType::HoldingRegisters;
    QPushButton *sender = qobject_cast<QPushButton *>(QObject::sender());
    uint16_t addr = static_cast<uint16_t>(sender->objectName().split('_').last().toInt());
    QModbusDataUnit dataunit = QModbusDataUnit(type, addr, 1);

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
            dataunit.setValue(0, static_cast<uint16_t>(box->currentIndex()));
        }
        else if(edit != NULL)
        {
            dataunit.setValue(0, static_cast<uint16_t>(edit->text().toInt()));
        }
        else
        {
            qDebug("Find ModbusParamWidgets error: %s %d", __FILE__, __LINE__);
            return;
        }
    }

    qDebug("addr:%d reg:%d", addr, dataunit.value(0));
    modbus->sendWriteRequest(dataunit, MODBUS_SLAVE_ADDR);
}

//设置所有参数
void ModbusParamWidget::on_modify_all_param()
{
    auto type = QModbusDataUnit::RegisterType::HoldingRegisters;
    uint16_t addr = 0, count = PARAM_REG_CNT, value;
    QModbusDataUnit dataunit = QModbusDataUnit(type, addr, count);

    for(int i = 0; i < PARAM_REG_CNT; ++i)
    {
        QString boxName = QString("param_comboBox_%1").arg(QString::number(i));
        QString editName = QString("param_lineEdit_%1").arg(QString::number(i));
        QComboBox *box = ui->param_groupBox->findChild<QComboBox *>(boxName);
        QLineEdit *edit = ui->param_groupBox->findChild<QLineEdit *>(editName);
        if(box != NULL)
        {
            value = static_cast<uint16_t>(box->currentIndex());
            dataunit.setValue(i, value);
        }
        else if(edit != NULL)
        {
            value = static_cast<uint16_t>(edit->text().toInt());
            dataunit.setValue(i, value);
        }
        else
        {
            qDebug("Find ModbusParamWidgets error: %s %d", __FILE__, __LINE__);
            return;
        }
    }

    modbus->sendWriteRequest(dataunit, MODBUS_SLAVE_ADDR);
}

//定时读取设备参数
void ModbusParamWidget::on_modbus_read_param()
{
    auto type = QModbusDataUnit::RegisterType::HoldingRegisters;
    uint16_t addr = 0;
    uint16_t count = PARAM_REG_CNT;
    QModbusDataUnit dataunit = QModbusDataUnit(type, addr, count);
    auto *reply = modbus->sendReadRequest(dataunit, MODBUS_SLAVE_ADDR);
    if(reply)
    {
        if(!reply->isFinished())
            connect(reply, SIGNAL(finished()), this, SLOT(on_display_param()));
        else
            delete reply;
    }
}

//显示参数
void ModbusParamWidget::on_display_param()
{
    QModbusReply *reply = qobject_cast<QModbusReply *>(QObject::sender());
    QModbusDataUnit unit;

    if(reply == NULL) return;

    if(reply->error() == QModbusDevice::NoError)
    {
        unit = reply->result();
    }
    else
    {
        qDebug() << reply->errorString();
        reply->deleteLater();
        return;
    }

    for(uint i = 0; i < unit.valueCount(); ++i)
    {
        QString labelName = QString("param_label_%1").arg(QString::number(i));
        QString boxName = QString("param_comboBox_%1").arg(QString::number(i));
        QLabel *label = ui->param_groupBox->findChild<QLabel *>(labelName);
        QComboBox *box = ui->param_groupBox->findChild<QComboBox *>(boxName);
        if(label != NULL)
        {
            int value = static_cast<int>(unit.value(i));
            if(box == NULL)
                label->setText(QString::number(value));
            else
                label->setText(box->itemText(value));
        }
    }

    reply->deleteLater();
}

//打开串口
void ModbusParamWidget::on_serial_port_open()
{
    if(modbus->state() == QModbusDevice::ConnectedState)
    {
        timer->stop();
        modbus->disconnectDevice();
        ui->pushButton_serial_port_open->setText("串口已经关闭");
    }
    else
    {
        QString portName = ui->serialPort_comboBox->currentText();
        int baudRate = ui->baudrate_comboBox->currentText().toInt();
        modbus->setConnectionParameter(QModbusDevice::SerialPortNameParameter, portName);
        modbus->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, baudRate);
        modbus->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, 8);
        modbus->setConnectionParameter(QModbusDevice::SerialParityParameter, 'N');
        modbus->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, 1);
        if(modbus->connectDevice())
        {
            ui->pushButton_serial_port_open->setText("串口已经打开");
            timer->start();
        }
    }
}

//保存参数到默认配置文件里
void ModbusParamWidget::on_save_default_settings()
{
    save_settings(paramName);
}

//读取其它参数文件
void ModbusParamWidget::on_load_settings_from()
{
    QString filter = tr("配置文件(*.ini)");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "./", filter, 0);
    if(!fileName.isEmpty())
    {
        read_settings(fileName);
    }
}

//参数另存为其它文件
void ModbusParamWidget::on_save_settings_as()
{
    QString filter = tr("配置文件(*.ini)");
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), "./", filter, 0);
    if(!fileName.isNull())
    {
        save_settings(fileName);
    }
}

void ModbusParamWidget::read_settings(QString name)
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
            qDebug("Find ModbusParamWidgets error: %s %d", __FILE__, __LINE__);
        }
    }
    delete settings;
}

void ModbusParamWidget::save_settings(QString name)
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
            qDebug("Find ModbusParamWidgets error: %s %d", __FILE__, __LINE__);
        }
    }
    delete settings;

}

