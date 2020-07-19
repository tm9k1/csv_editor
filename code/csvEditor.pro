
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = csvEditor
TEMPLATE = app


SOURCES += main.cpp\
        csv_util.cpp \
        mainwindow.cpp

HEADERS  += mainwindow.h \
    csv_util.h

FORMS    += mainwindow.ui

#Uncomment to check qDebug messages in a separate console window.
#CONFIG += console
