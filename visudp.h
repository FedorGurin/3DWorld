#ifndef VISUDP_H
#define VISUDP_H
#include <QUdpSocket>
#include <QObject>
#include "gl_func.h"


enum TTypeHead
{
    FLIGHT_OBJECT=0
};

//! ��������� ������� ��� ��������/���������
typedef struct THeadRequest_
{
    //! ������������� ������ (��� ��������� �������)
    unsigned int uid;
    //! ���������� ���������:
    //              0 - ������,
    //              1 - ������,
    //              2 - �����
    TTypeHead type;
    //! ������ ������
    unsigned long size;
}THeadRequest;
//! ������
typedef struct
{
    THeadRequest    head;
    TVis      object;
}TRec_Flight_Obj;

enum TCommand
{
    CONTROL_STICK=1
};

//! ��������� �
typedef struct
{
    int id;
    TCommand command;
    char buffer[80];
}TSend_UDP;

class VisUDP:public QObject
{
    Q_OBJECT;
public:
    VisUDP();

    QVector<TVis> *getObjects(){return &flightObjects;}
    //void sendData(int id,int command);
    void sendData(int idObject,TCommand command, QByteArray array);

    void addToFlightObjList(TVis body);
private slots:
    void processPendingDatagrams();
signals:
    void sigAircraft();
    void sigAirTarget();
    void sigGroundTarget();
    void flightObjChanged();

private:
    QUdpSocket udpSocketRecive;
    QUdpSocket udpSocketSend;

    TRec_Flight_Obj flightObj;
    //! ����� �������
    //TRec_UDP rec_udp;
    //! �������� �������
    TSend_UDP send_data;
    //! ����������� �������(���� �������� ������ ��� � ������. � ������ id)
    //! �� ������ ������ ���������� �����

    QVector<TVis> flightObjects;//! ������� �������
};

#endif // VISUDP_H
