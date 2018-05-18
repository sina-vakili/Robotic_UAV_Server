#include "msp_part.h"
#include "ssooket.h"


extern quint16 roll_client , pitch_client , yaw_client , throttle_client ,AUX1 ;
qint16 ang_roll , ang_pitch;
char msp_speed;
qint32 Height_quad;
extern qint8 flags ;
extern qint32 New_Height_quad;
extern char  client_time ;

msp_part::msp_part(QObject *parent) : QObject(parent)
{
    send_resive_msp = new QElapsedTimer;  // mohasebeye zamaneh msp
    send_resive_msp->start();
    //================================================> etesale usarte beyne server va CF
    timer_msp = new QTimer (this);       // timere msp
    connect(timer_msp,SIGNAL(timeout()),SLOT(send_msp()));
    timer_msp->start(30);

    msp_sender = new QSerialPort(this);
    msp_sender->setPortName("/dev/ttyUSB0");
    msp_sender->setBaudRate(QSerialPort::Baud115200);
    msp_sender->setDataBits(QSerialPort::Data8);
    msp_sender->setParity(QSerialPort::NoParity);
    msp_sender->setStopBits(QSerialPort::OneStop);
    msp_sender->setFlowControl(QSerialPort::NoFlowControl);
    msp_sender->open(QIODevice::ReadWrite);
    connect(msp_sender,SIGNAL(readyRead()),SLOT(read_msp())); //------------> timer  baraye khandaneh dade az CF

    if (!(msp_sender->isOpen()))
        qDebug() << "msp not found usart";
}

//==================================================>send data from server to CF

char level = 0;

void msp_part::send_msp()
{
    if (msp_sender->isOpen()){

        quint16 value ;
        char high_value , low_value ,cheksum ;
        char long_value , code_value;

        switch (level) {

        case 0 :  //=========================================> bakhshe ersale ertefa

            cheksum = 0;

            msp_sender->write("$");
            msp_sender->write("M");
            msp_sender->write("<");

            long_value = 0 ;
            msp_sender->write(&long_value,1);
            code_value = 109;
            msp_sender->write(&code_value,1);

            cheksum ^= long_value;
            cheksum ^= code_value;
            msp_sender->write(&cheksum,1);

            level = 1 ;
            break;

        case 1 :  //=====================================================> bakhshe ersale roll , pitch , yaw , throttle

            cheksum = 0;  //-------------------------------->usart protocol
            msp_sender->write("$");
            msp_sender->write("M");
            msp_sender->write("<");
            long_value = 16 ;           // tolle dade
            cheksum ^= long_value;
            msp_sender->write(&long_value,1);
            code_value =200;            // code darkhast
            cheksum ^= code_value;

            //---------------------> tabdil va ersale roll
            value = roll_client;
            msp_sender->write(&code_value,1);
            high_value = value >> 8;
            high_value = high_value & 0xff ;
            low_value = value & 0xff;
            cheksum ^= high_value;
            cheksum ^= low_value;
            msp_sender->write(&low_value,1);
            msp_sender->write(&high_value,1);

            //---------------------> tabdil va ersale pitch
            value = pitch_client;
            high_value = value >> 8;
            high_value = high_value & 0xff ;
            low_value = value & 0xff;
            cheksum ^= high_value;
            cheksum ^= low_value;
            msp_sender->write(&low_value,1);
            msp_sender->write(&high_value,1);

            //----------------------> tabdil va ersale throttle
            value = throttle_client;
            high_value = value >> 8;
            high_value = high_value & 0xff ;
            low_value = value & 0xff;
            cheksum ^= high_value;
            cheksum ^= low_value;
            msp_sender->write(&low_value,1);
            msp_sender->write(&high_value,1);

            //---------------------> tabdil va ersale yaw
            value = yaw_client;
            high_value = value >> 8;
            high_value = high_value & 0xff ;
            low_value = value & 0xff;
            cheksum ^= high_value;
            cheksum ^= low_value;
            msp_sender->write(&low_value,1);
            msp_sender->write(&high_value,1);

            //-----------------------> tabdil va ersale ax1
            value = AUX1 ;
            high_value = value >> 8;
            high_value = high_value & 0xff ;
            low_value = value & 0xff;
            cheksum ^= high_value;
            cheksum ^= low_value;
            msp_sender->write(&low_value,1);
            msp_sender->write(&high_value,1);

            //-----------------------> tabdil va ersale ax2
            value = 1502 ;
            high_value = value >> 8;
            high_value = high_value & 0xff ;
            low_value = value & 0xff;
            cheksum ^= high_value;
            cheksum ^= low_value;
            msp_sender->write(&low_value,1);
            msp_sender->write(&high_value,1);

            //-----------------------> tabdil va ersale ax3
            value = 1503;
            high_value = value >> 8;
            high_value = high_value & 0xff ;
            low_value = value & 0xff;
            cheksum ^= high_value;
            cheksum ^= low_value;
            msp_sender->write(&low_value,1);
            msp_sender->write(&high_value,1);

            //-----------------------> tabdil va ersale ax4
            value = 1504 ;
            high_value = value >> 8;
            high_value = high_value & 0xff ;
            low_value = value & 0xff;
            cheksum ^= high_value;
            cheksum ^= low_value;
            msp_sender->write(&low_value,1);
            msp_sender->write(&high_value,1);
            msp_sender->write(&cheksum,1);

            level = 2;
            break;

        case 2 : //=======================================> bakhshe ersale darkhast ertefa

            cheksum = 0;

            msp_sender->write("$");
            msp_sender->write("M");
            msp_sender->write("<");
            long_value = 0 ;
            msp_sender->write(&long_value,1);
            code_value =108;
            msp_sender->write(&code_value,1);
            cheksum ^= long_value;
            cheksum ^= code_value;
            msp_sender->write(&cheksum,1);

            if (flags & 0b01)
            {
                level = 3 ;
            }
            else
            {
                level = 4 ;
            }
            break;

        case 3 : //======================================> calibration

            cheksum = 0;
            msp_sender->write("$");
            msp_sender->write("M");
            msp_sender->write("<");
            long_value = 0 ;
            msp_sender->write(&long_value,1);
            code_value = 205 ;
            msp_sender->write(&code_value,1);
            cheksum ^= long_value;
            cheksum ^= code_value;
            msp_sender->write(&cheksum,1);

            if (flags & 0b010){
                level = 4 ;
            }
            else
            {
                level = 0 ;
            }

            break;

        case 4 :  //garar dadaneh ertefa

            cheksum = 0 ;
            msp_sender->write("$");
            msp_sender->write("M");
            msp_sender->write("<");
            long_value = 15 ;
            msp_sender->write(&long_value,1);
            code_value = 209 ;
            msp_sender->write(&code_value,1);

            char nnull ;
            char l_height = New_Height_quad & 0xff ;
            char h_height = ( New_Height_quad >> 8 ) & 0xff ;
            char hh_height = ( New_Height_quad >> 16) & 0xff ;
            char hhh_height = ( New_Height_quad >> 24) & 0xff ;

            nnull = 0 ;
            msp_sender->write(&nnull,1);
            msp_sender->write(&nnull,1);
            msp_sender->write(&nnull,1);
            msp_sender->write(&nnull,1);
            msp_sender->write(&nnull,1);
            msp_sender->write(&nnull,1);
            msp_sender->write(&nnull,1);
            msp_sender->write(&nnull,1);
            msp_sender->write(&nnull,1);
            msp_sender->write(&l_height,1);
            msp_sender->write(&h_height,1);
            msp_sender->write(&hh_height,1);
            msp_sender->write(&hhh_height,1);
            msp_sender->write(&nnull,1);
            msp_sender->write(&nnull,1);

            cheksum ^= long_value;
            cheksum ^= code_value;
            cheksum ^= nnull;
            cheksum ^= nnull;
            cheksum ^= nnull;
            cheksum ^= nnull;
            cheksum ^= nnull;
            cheksum ^= nnull;
            cheksum ^= nnull;
            cheksum ^= nnull;
            cheksum ^= nnull;
            cheksum ^= l_height;
            cheksum ^= h_height;
            cheksum ^= hh_height;
            cheksum ^= hhh_height;
            cheksum ^= nnull;
            cheksum ^= nnull;

            msp_sender->write(&cheksum,1);

            level = 0 ;

            break ;
        }
    }
}


//==========================================> read data from CF to Server
void msp_part::read_msp()
{
    //---------------------> zamane daryafte msp
    msp_speed = send_resive_msp->elapsed();
    send_resive_msp->start();

    //-----------
    QByteArray msp_rr = msp_sender->readAll();

    //---------------------> daryafte angle++++++(108)++++++

    if(( static_cast<quint8>(msp_rr[4]) ) == 108)
    {
        ang_roll=(static_cast<quint8>(msp_rr[5])& 0xff)+ ((static_cast<quint8>(msp_rr[6]) )<<8 & 0xff00);
        ang_pitch= (static_cast<quint8>(msp_rr[7])& 0xff)+ ((static_cast<quint8>(msp_rr[8]) )<<8 & 0xff00);
    }

    //---------------------> daryafte ertefa++++++(109)++++++

    else if(( static_cast<quint8>(msp_rr[4]) ) == 109)
    {
        Height_quad = (static_cast<quint8>(msp_rr[5])& 0xff) + (((static_cast<quint8>(msp_rr[6]) )<<8) & 0xff00)
                + ((static_cast<quint8>(msp_rr[7])<<16) & 0xff0000) + (((static_cast<quint8>(msp_rr[8]) )<<24) & 0xff000000);
    }
}
