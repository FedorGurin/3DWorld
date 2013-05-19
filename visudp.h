#ifndef VISUDP_H
#define VISUDP_H
#include <QUdpSocket>
#include <QObject>
#include "gl_func.h"
#include "globalData.h"

enum TTypeHead
{
    FLIGHT_OBJECT=0
};

//! Структура запроса для отправки/получения
typedef struct THeadRequest_
{
    //! идентификатор пакета (для обработки запроса)
    unsigned int uid;
    //! Назначение сообщения:
    //              0 - чтение,
    //              1 - запись,
    //              2 - маска
    TTypeHead type;
    //! идентификатор пользователя
    unsigned int uid_user;
    //! размер пакета
    unsigned long size;
}THeadRequest;
//! описан
typedef struct
{
    THeadRequest    head;
    TVis      object;
}TRec_Flight_Obj;

enum TCommand
{
    CONTROL_STICK=1
};

//! сообщения к
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

    QVector<TVis> *getObjects()
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

    void sendData(int idObject,TCommand command, QByteArray array);

    //! проверка наличия буфера для адаптера
    bool testObjInList(unsigned uid);
    void addToFlightObjList(TVis body);
    //! проверка datagram
    void checkDatagrams();
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
    QByteArray datagram;
    //! блокировка соединения
    bool blockConnect;
    THeadRequest head;
    //! прием объекта
    TRec_Flight_Obj rec_udp;
    //! отправка объекта
    TSend_UDP send_data;
    //! графические объекты(если приходит объект уже с сущест. в списке id)
    //! то старый объект заменяется новым

    QVector<TVis> flightObjects;//! объекты который

};

#endif // VISUDP_H
