#-------------------------------------------------
#
# Project created by QtCreator 2019-08-28T15:23:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = HybridInverter
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp mbRTUMaster.cpp qtRTUMaster.cpp

HEADERS  += widget.h mbError.h mbRTUMaster.h qtRTUMaster.h

FORMS    += widget.ui
