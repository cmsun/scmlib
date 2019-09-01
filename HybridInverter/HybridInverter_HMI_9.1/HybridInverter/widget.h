#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QtSerialBus/QModbusRtuSerialMaster>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

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
    Ui::Widget *ui;
    QTimer *timer;
    QString paramName;
    QModbusRtuSerialMaster *modbus;
    void read_settings(QString name);
    void save_settings(QString name);
};

#endif // WIDGET_H
