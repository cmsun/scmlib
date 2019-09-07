#ifndef ModbusParamWidget_H
#define ModbusParamWidget_H

#include <QWidget>
#include <QTimer>
#include <QtSerialBus/QModbusRtuSerialMaster>

namespace Ui {
class ModbusParamWidget;
}

class ModbusParamWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModbusParamWidget(QWidget *parent = 0);
    ~ModbusParamWidget();

private slots:
    void on_modify_single_param();
    void on_modify_all_param();
    void on_serial_port_open();
    void on_save_default_settings();
    void on_load_settings_from();
    void on_save_settings_as();
    void on_modbus_read_param();
    void on_display_param();

private:
    Ui::ModbusParamWidget *ui;
    QTimer *timer;
    QString paramName;
    QModbusRtuSerialMaster *modbus;
    void read_settings(QString name);
    void save_settings(QString name);
};

#endif // ModbusParamWidget_H
