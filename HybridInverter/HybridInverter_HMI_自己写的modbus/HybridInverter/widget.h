#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "qtRTUMaster.h"

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
    void on_param_pushButton_clicked();
    void on_modbus_read_param();
    void on_pushButton_3_clicked();
    void on_serialOpen_pushButton_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::Widget *ui;
    QtRTUMaster *rtu;
    QTimer *timer;
    QString paramName;
    void read_settings(QString name);
    void save_settings(QString name);
    static void param_display(const std::vector<mbu8> &sendData, const std::vector<mbu8> &recData, void *arg, mbu8 err);
};

#endif // WIDGET_H
