QT += core network serialport
QT -= gui

CONFIG += c++11

TARGET = final_server_one
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ssooket.cpp \
    msp_part.cpp

HEADERS += \
    ssooket.h \
    msp_part.h

LIBS += -L /usr/local/lib -lwiringPi -lpthread

LIBS += `pkg-config opencv --libs`

LIBS += -lzbar

