#ifndef SSOOKET_H
#define SSOOKET_H

#include <QObject>
#include<QTcpServer>
#include<QTcpSocket>
#include<QtNetwork>
#include <QSerialPort>
#include<QtCore>
#include<softPwm.h>
#include "msp_part.h"
#include "opencv2/opencv.hpp"
#include <string>
#include <iomanip>
#include <zbar.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;
using namespace zbar;

/*quint16 roll_client , pitch_client , yaw_client , throttle_client ;
qint16 ang_roll , ang_pitch;
qint32 Height_quad , New_Height_quad;
char cheksum = 0 ;
char msp_speed ,client_time; */

class ssooket : public QObject
{
    Q_OBJECT
public:
    explicit ssooket(QObject *parent = 0);
    QTimer *timer_server_send_client;
    QTimer *servo_timer;
    QTimer *Save_image_timer;
    QTcpServer *sserver;
    QTcpSocket *ssoket;
    QElapsedTimer *rresive_client_timer;
    VideoCapture vcap;

signals:

public slots:

private slots:
    void rresive();
    void print();
    void establish_conection();
    void servo_func();
    void take_image();
};

#endif // SSOOKET_H
