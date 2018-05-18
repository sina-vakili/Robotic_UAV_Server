#include "ssooket.h"
#include <string>
#include <iostream>


int number_image = 0 ;
quint16 roll_client , pitch_client , yaw_client , throttle_client , AUX1;
qint32  New_Height_quad ;
char  client_time ;
qint8 flags , Atl_mode;
extern qint16 ang_roll , ang_pitch;
extern qint32 Height_quad ;
extern char msp_speed ;

ImageScanner scanner;  //Qrcode

//=====================================================> bakhsh start timer ha va barnameh
ssooket::ssooket(QObject *parent) : QObject(parent)
{
    rresive_client_timer= new QElapsedTimer;  // mohasebeye timer daryaft
    rresive_client_timer->start();

    //-------------------------------->take image
    Save_image_timer = new QTimer(this);      // timer take image
    connect(Save_image_timer,SIGNAL(timeout()),SLOT(take_image()));
    Save_image_timer->start(40);  //start every time
    vcap.open(0);
    if(!vcap.isOpened()){
        qDebug() << "Error opening video stream or file" ;
    }

    //----------------------> gosh dadan server be ip
    sserver = new QTcpServer (this);

    if(!sserver->listen(QHostAddress::Any, 1234))
    {
        qDebug() << "Server could not start!";
    }
    else
    {
        qDebug() << "Server started!";
    }

    servo_timer=new QTimer (this); //timer amade bashe servo
    connect(servo_timer,SIGNAL(timeout()),SLOT(servo_func()));
    servo_timer->start(10);

    //----------------------> timer ersal data be client
    timer_server_send_client= new QTimer (this);
    connect(timer_server_send_client,SIGNAL(timeout()),SLOT(print()));
    timer_server_send_client->stop();


    connect(sserver, SIGNAL(newConnection()), SLOT(establish_conection()));

    //----------------------->bakhshe servo

    wiringPiSetup();
    //wiringPiSetupGpio();
    pinMode(2,1);
    pinMode(0,INPUT);
    digitalWrite(0,1);
    pinMode(3,1);

    softPwmCreate(3,0,100);

    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);  //Qrcode
}

//===========================================================> ersale dade be client
void ssooket::print(){


    char cheksum ;
    char high_angel_roll , low_angel_roll , high_angel_pitch , low_angel_pitch ;
    char lo_Height_quad  , hi_Height_quad , h_hi_Height_quad ,hh_hi_Height_quad ;


    //------------------------------->protocol ersali @ & $ ang_roll , ang_pitch , Height_quad , cheksum
    ssoket->write("@");                                 // D:00   ramz_@
    ssoket->write("&");                                 // D:01   ramz_&
    ssoket->write("$");                                 // D:02   ramz_$

    high_angel_roll = (ang_roll>>8)& 0xff;
    low_angel_roll =(ang_roll& 0xff);

    ssoket->write(&low_angel_roll,1);                   // D:03   low_angel_roll
    ssoket->write(&high_angel_roll,1);                  // D:04   high_angel_roll

    high_angel_pitch = (ang_pitch>>8)& 0xff;
    low_angel_pitch =(ang_pitch& 0xff);

    ssoket->write(&low_angel_pitch,1);                  // D:05   low_angel_pitch
    ssoket->write(&high_angel_pitch,1);                 // D:06   high_angle_pitch

    lo_Height_quad = (Height_quad & 0xff);
    hi_Height_quad = (Height_quad >> 8)& 0xff;
    h_hi_Height_quad = (Height_quad >> 16)& 0xff;
    hh_hi_Height_quad = (Height_quad >> 24)& 0xff;

    ssoket->write(&lo_Height_quad,1);                  // D:07   low_height_quad
    ssoket->write(&hi_Height_quad,1);                  // D:08   hi_height_quad
    ssoket->write(&h_hi_Height_quad,1);                // D:09   h_hi_height_quad
    ssoket->write(&hh_hi_Height_quad,1);               // D:10   hh_hi_height_quad

    cheksum ^= high_angel_roll ;
    cheksum ^= low_angel_roll ;
    cheksum ^= high_angel_pitch ;
    cheksum ^= low_angel_pitch ;
    cheksum ^= lo_Height_quad ;
    cheksum ^= hi_Height_quad ;
    cheksum ^= h_hi_Height_quad ;
    cheksum ^= hh_hi_Height_quad ;

    ssoket->write(&msp_speed,1);                       // D:11   msp_speed
    ssoket->write(&client_time,1);                     // D:12   client_time

    cheksum ^= msp_speed ;
    cheksum ^=client_time ;

    ssoket->write(&cheksum,1);                         // D:13   cheksum

    //====================================================
    if (rresive_client_timer->elapsed()> 800 )
    {
        throttle_client = 1000;
        yaw_client = 1000;
        //        pitch_client = 1500;
        //        roll_client = 1500 ;
        //        AUX1 = 1900 ;

        qDebug() << "Your connection , disconnect" ;

        digitalWrite(2,HIGH);
    }
    else
    {
        digitalWrite(2,LOW);
    }
}

//============================================================>  tabeah etesale bayne server va client
void ssooket::establish_conection()
{
    ssoket = new QTcpSocket (this) ;
    ssoket = sserver->nextPendingConnection();
    connect(ssoket,SIGNAL(readyRead()),SLOT(rresive()));
    timer_server_send_client->start(10);

    Save_image_timer->start(1000);
    //ssoket->write("hello client\r\n");
    // ssoket->flush();// motaleeeee shavad
}

//-----------------------------> tabea estefadeh az servo
void ssooket::servo_func()
{
    if(digitalRead(0))
    {
        softPwmWrite(3,19);
    }
    else
    {
        softPwmWrite(3,7);
    }
}

//----------------------------> tabea Read Qrcode

void ssooket::take_image()
{
    if(vcap.isOpened())
    {
        cv::Mat frame, frame_grayscale;
        vcap >> frame;
        cvtColor(frame, frame_grayscale, CV_BGR2GRAY);
        int width = frame_grayscale.cols;
        int height = frame_grayscale.rows;
        uchar *raw = (uchar *)(frame_grayscale.data);
        Image image(width, height, "Y800", raw, width * height);
        scanner.scan(image);
        for (Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
        {
            std::cout   << "decoded " << symbol->get_type_name();
            std::cout << " symbol \"" << symbol->get_data() << '"' << endl;
        }
    }
    //     if(vcap.isOpened())
    //    {
    //    Mat frame;
    //    vcap >> frame;
    //    number_image ++ ;
    //    std::stringstream qq;
    //    qq << number_image ;
    //    std::string str = qq.str();
    //    imwrite("Image"+str+".jpg",frame);
    //    }
}

//=============================================================> tavabeh  daryaft dade az client be server

void ssooket::rresive (){
    quint8 cheksum=0;

    //---------->zaman daryafte data az client
    client_time = rresive_client_timer->elapsed();
    rresive_client_timer->start();

    QByteArray s = ssoket->readAll();

    //-------------------------------mohasebeye avaliye cheksum baraye barrasi shart
    cheksum^=static_cast<quint8>(s[3]) & 0xff;  cheksum^=static_cast<quint8>(s[4])& 0xff;
    cheksum^=static_cast<quint8>(s[5])& 0xff;  cheksum^=static_cast<quint8>(s[6]) & 0xff;
    cheksum^=static_cast<quint8>(s[7]) & 0xff;  cheksum^=static_cast<quint8>(s[8])& 0xff;
    cheksum^=static_cast<quint8>(s[9])& 0xff;  cheksum^=static_cast<quint8>(s[10])& 0xff;
    cheksum^=static_cast<quint8>(s[11])& 0xff;  cheksum^=static_cast<quint8>(s[12])& 0xff;
    cheksum^=static_cast<quint8>(s[13])& 0xff;  cheksum^=static_cast<quint8>(s[14])& 0xff;
    cheksum^=static_cast<quint8>(s[15])& 0xff;
    cheksum &=0xff;

    if (cheksum==(static_cast<quint8>(s[16])&0xff))
    {
        //-------------------------------------------->convert reived char to int16 (from client to server)

        New_Height_quad = (qint32((static_cast<quint8>(s[11]) & 0xff)+((static_cast<quint8>(s[12])<<8))
                + (static_cast<quint8>(s[13])<<16)+((static_cast<quint8>(s[14])<<24))));
        //qDebug() << New_Height_quad ;
        roll_client = (static_cast<quint8>(s[3])<<8)+((static_cast<quint8>(s[4])& 0xff));
        pitch_client = (static_cast<quint8>(s[5])<<8)+((static_cast<quint8>(s[6])& 0xff));
        yaw_client = (static_cast<quint8>(s[7])<<8)+((static_cast<quint8>(s[8])& 0xff));
        flags = (static_cast<quint8>(s[15])) ;

        if(flags & 0b000010)
        {
            throttle_client = 1500 ;
            AUX1 = 1900 ;
        }
        else
        {
            throttle_client =(static_cast<quint8>(s[9])<<8)+((static_cast<quint8>(s[10])& 0xff));
            AUX1 = 1100 ;
        }
    }
}
