#ifndef VISUDP_H
#define VISUDP_H
#include <QUdpSocket>
#include <QObject>
#include "math_func.h"
#include "globalData.h"

#define SIZE_BUF_MAX 2048

enum TTypeHead
{
    FLIGHT_OBJ=0,
    INFO_FLIGHT_OBJ=1,
    COMMAND=2
};
//! ��������� ������� ��� ��������/���������
typedef struct
{
    //! ������������� ������������ ������
    unsigned long id;
    //! ��� �������
    unsigned typeRequest;
    //! ������ ����� ������
    unsigned long size;
    //! ���������� �����(� �����)
    unsigned long tics;
    //! �������������� ��������(� �����) ���������� ������� �����
    unsigned long dtTics;
}THRequest;
//! ��������� ��� �������� ����� ������ ������
typedef struct
{
    //! ��������� ������
    THRequest head;
    //! ������� ������ ������ ������
    char compressed;
    //! ��� ������
    int err;
    //! ������ ��������� � �������
    unsigned long sizeBuf;
    //! ������ � �������(������ ������� ������ � sizebuf)
    char buffer[SIZE_BUF_MAX];
}TMRequest;

enum TypeCommand
{
    TC_RESET,/*������� ������� ����������� � �������������� ���������*/
    TC_CLEAR /*������� ����������*/
};
//! ��������� ��� �������� ������� � ������
typedef struct
{
    //! ��������� ������
    THRequest head;
    TypeCommand com;
}TCRequest;

class VisUDP:public QObject
{
    Q_OBJECT;
public:
    VisUDP();

    QVector<TSolidObj> *getObjects()
    {

        return &flightObjects;
    }
    void setSizeObj(int size)
    {
        flightObjects.resize(size);
    }
    void setBlockConection(bool value)
    {
        blockConnect=value;
    }

    //void sendData(int idObject,TCommand command, QByteArray array);

    //! �������� ������� ������ ��� ��������
    bool testObjInList(unsigned uid);
    void addToFlightObjList(TSolidObj body);
    //! �������� datagram
    void checkDatagrams();
private slots:
    void processPendingDatagrams();
signals:
    void sigAircraft();
    void sigAirTarget();
    void sigGroundTarget();
    void flightObjChanged();
    void resetTrajectory();

private:
    QUdpSocket udpSocketRecive;
    QUdpSocket udpSocketSend;
    QByteArray datagram;
    //! ���������� ����������
    bool blockConnect;
    THRequest head;
    //! ����� �������
    TMRequest rec_udp;
    //! �������� �������
    //TMRequest send_data;
    //! ����������� �������(���� �������� ������ ��� � ������. � ������ id)
    //! �� ������ ������ ���������� �����

    QVector<TSolidObj> flightObjects;//! ������� �������

};

#endif // VISUDP_H
