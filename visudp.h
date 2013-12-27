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
//! структура запроса для отправки/получения
typedef struct
{
    //! идентификатор программного модуля
    unsigned long id;
    //! тип запроса
    unsigned typeRequest;
    //! размер всего пакета
    unsigned long size;
    //! глобальное время(в тиках)
    unsigned long tics;
    //! предполагаемая задержка(в тиках) измеренная опытным путем
    unsigned long dtTics;
}THRequest;
//! структура для передачи всего буфера данных
typedef struct
{
    //! заголовок пакета
    THRequest head;
    //! признак сжатия буфера данных
    char compressed;
    //! код ошибки
    int err;
    //! размер структуры в буффере
    unsigned long sizeBuf;
    //! буффер с данными(размер массива указан в sizebuf)
    char buffer[SIZE_BUF_MAX];
}TMRequest;

enum TypeCommand
{
    TC_RESET,/*перевод системы отображение в первоначальное состояние*/
    TC_CLEAR /*очистка траектории*/
};
//! структура для передачи команды в модуль
typedef struct
{
    //! заголовок пакета
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

    //! проверка наличия буфера для адаптера
    bool testObjInList(unsigned uid);
    void addToFlightObjList(TSolidObj body);
    //! проверка datagram
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
    //! блокировка соединения
    bool blockConnect;
    THRequest head;
    //! прием объекта
    TMRequest rec_udp;
    //! отправка объекта
    //TMRequest send_data;
    //! графические объекты(если приходит объект уже с сущест. в списке id)
    //! то старый объект заменяется новым

    QVector<TSolidObj> flightObjects;//! объекты который

};

#endif // VISUDP_H
