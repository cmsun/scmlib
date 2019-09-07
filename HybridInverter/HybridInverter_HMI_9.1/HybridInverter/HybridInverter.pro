#-------------------------------------------------
#
# Project created by QtCreator 2019-08-28T15:23:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport serialbus

TARGET = HybridInverter
TEMPLATE = app


SOURCES += main.cpp \
    ModbusParamWidget.cpp
HEADERS  += \
    ModbusParamWidget.h

FORMS    += \
    ModbusParamWidget.ui
