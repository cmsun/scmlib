#include "ModbusParamWidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ModbusParamWidget w;
    w.show();

    return a.exec();
}
