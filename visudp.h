#ifndef VISUDP_H
#define VISUDP_H
#include <QUdpSocket>
#include <QObject>
#include "math_func.h"
#include "globalData.h"

#define SIZE_BUF_DEFAULT 2048

enum TTypeHead:uint32_t
{
    PARAM_OBJ       = 0,
    PARAM_ARRAY     = 4,
    INFO_FLIGHT_OBJ = 1,
    COMMAND         = 2,
    CONTROL_STICK   = 3,    /*управление ручкой указанными объектом*/
    LIST_VECTOR     = 5

};
//! структура запроса для отправки/получения
typedef struct
{
    //! идентификатор программного модуля
    uint32_t id;
    //! тип запроса
    TTypeHead typeRequest;
    //! размер всего пакета
    uint32_t size;
    //! глобальное время(в тиках)
    uint32_t tics;
    //! предполагаемая задержка(в тиках) измеренная опытным путем
    uint32_t dtTics;
}THRequest;
//! структура для передачи всего буфера данных
typedef struct
{
    //! заголовок пакета
    THRequest head;
    //! признак сжатия буфера данных
    uint8_t compressed;
    //! код ошибки
    uint8_t err;
    //! размер структуры в буффере
    uint32_t sizeBuf;
    //! буффер с данными(размер массива указан в sizebuf)
    uint8_t buffer[SIZE_BUF_DEFAULT];
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

//! класс для приема данных по UDP
class VisUDP:public QObject
{
    Q_OBJECT;
public:
    VisUDP();

    //! список воздушных объектов
    QVector<TSendVehicleVisSimple> *getVehicleObj()
    {
        return &flightObjects;
    }
    QVector<TSendArrayVisSimple> *getArrayObj()
    {
        return &arrayObjects;
    }
    QVector<glm::vec3> *getVecObj()
    {
        return &vecList;
    }
    //! кол-во объектов
    void setSizeObj(int size)
    {
        flightObjects.resize(size);
    }
    //! блокировка соединения
    void setBlockConection(bool value)
    {
        blockConnect = value;
    }

    //! отправить данные другому сервису
    void sendData(TTypeHead type,char *buffer, int size);
    //! проверка наличия объекта в списке
    int testFlightObjInList(unsigned uid);
    int testArrayObjInList(unsigned uid);
    void addToFlightObjList(TSendVehicleVisSimple body);
    void addToArrayObjList(TSendArrayVisSimple body);
    //! проверка datagram
    bool checkDatagrams();
    double synch_time;
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
    //! заголовок пакета
    THRequest head;
    //! прием объекта
    TMRequest rec_udp;
    //! отправка объекта
    TMRequest send_udp;
    //! графические объекты(если приходит объект уже с сущест. в списке id)
    //! то старый объект заменяется новым
    QVector<TSendVehicleVisSimple>  flightObjects;//! объекты воздушных целей
    QVector<TSendArrayVisSimple>    arrayObjects; //! объекты состоящие из нескольких точках
    QVector<glm::vec3> vecList;

};

#endif // VISUDP_H
