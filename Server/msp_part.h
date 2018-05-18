#ifndef MSP_PART_H
#define MSP_PART_H

//#include "ssooket.h"
#include <QtCore>
#include <QObject>
//#include<QTcpServer>
//#include<QTcpSocket>
//#include<QtNetwork>
#include <QSerialPort>
#include<QtCore>
#include <wiringPi.h>


class msp_part : public QObject
{
    Q_OBJECT
public:
    explicit msp_part(QObject *parent = 0);
    QTimer *timer_msp;
    QSerialPort *msp_sender;
    QElapsedTimer *send_resive_msp;

signals:

public slots:


private slots:
    void send_msp();
    void read_msp();

};

#endif // MSP_PART_H
