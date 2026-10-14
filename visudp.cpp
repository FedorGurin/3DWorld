#include "visudp.h"
#include <iostream>
#include <QDataStream>
#include <cstddef>
#include <cstring>
VisUDP::VisUDP()
{
    synch_time = 0;
    blockConnect = false;
    flightObjects.clear();
    memset(&head, 0, sizeof(head));
    memset(&rec_udp, 0, sizeof(rec_udp));
    memset(&send_udp, 0, sizeof(send_udp));
    if(udpSocketRecive.bind(7030) == false)
        std::cout<<"Can`t bind to socket port" << std::endl;
    datagram.resize(sizeof(TMRequest));
}
//! копирование полезной нагрузки пакета в структуру с проверкой размера
//! (недостающая часть структуры заполняется нулями)
template<typename T>
static void readPayload(T &dst, const uint8_t *buf, uint32_t size)
{
    memset(static_cast<void*>(&dst), 0, sizeof(T));
    memcpy(static_cast<void*>(&dst), buf, qMin<size_t>(size, sizeof(T)));
}
bool VisUDP::checkDatagrams()
{

    if(udpSocketRecive.hasPendingDatagrams() == false)
        return false;

    while(udpSocketRecive.hasPendingDatagrams())
    {
        qint64 sizeDatagram = udpSocketRecive.pendingDatagramSize();
        if(sizeDatagram < 0)
            break;
        datagram.resize(qMax<qint64>(sizeDatagram, 1));
        sizeDatagram = udpSocketRecive.readDatagram(datagram.data(), datagram.size());
        // пакет короче заголовка - пропускаем
        if(sizeDatagram < static_cast<qint64>(sizeof(THRequest)))
            continue;

        // чтение заголовка
        memcpy(&head, datagram.constData(), sizeof(THRequest));

        // обработка пакетов
        if(head.typeRequest == COMMAND)
        {
            TCRequest req;
            memset(&req, 0, sizeof(req));
            memcpy(&req, datagram.constData(), qMin<size_t>(sizeDatagram, sizeof(req)));

            if(req.com == TC_RESET || req.com == TC_CLEAR)
                emit resetTrajectory();
            continue;
        }
        if(head.typeRequest != PARAM_OBJ &&
           head.typeRequest != PARAM_ARRAY &&
           head.typeRequest != LIST_VECTOR)
            continue;

        // пакет должен содержать хотя бы заголовок буфера данных
        const qint64 offsetBuf = offsetof(TMRequest, buffer);
        if(sizeDatagram < offsetBuf)
            continue;
        memset(&rec_udp, 0, sizeof(rec_udp));
        memcpy(&rec_udp, datagram.constData(), qMin<size_t>(sizeDatagram, sizeof(TMRequest)));

        // фактический размер данных в буфере
        uint32_t sizeBuf = rec_udp.sizeBuf;
        sizeBuf = qMin<uint32_t>(sizeBuf, SIZE_BUF_DEFAULT);
        sizeBuf = qMin<uint32_t>(sizeBuf, sizeDatagram - offsetBuf);

        if(head.typeRequest == PARAM_OBJ)
        {
            //! преобразуем буфер в структуру
            TSendVehicleVisSimple solid;
            readPayload(solid, rec_udp.buffer, sizeBuf);

            int index = testFlightObjInList(solid.id);
            if(index >= 0 )
            {
                flightObjects[index] = solid;
            }else
            {
                qDebug("Create new obj=%d\n",solid.id);
                flightObjects.push_back(solid);
            }
            synch_time = solid.time;
        }else if(head.typeRequest == PARAM_ARRAY)
        {
            // преобразуем буфер в структуру
            TSendArrayVisSimple arrayObj;
            readPayload(arrayObj, rec_udp.buffer, sizeBuf);
            // кол-во точек не может превышать размер принятых данных
            const size_t offsetCoord = offsetof(TSendArrayVisSimple, coord);
            size_t maxNum = sizeBuf > offsetCoord ?
                        (sizeBuf - offsetCoord) / sizeof(TSendArrayVisSimple::TGeoCoord) : 0;
            arrayObj.num = qMin<size_t>(arrayObj.num, maxNum);

            int index = testArrayObjInList(arrayObj.id);
            if(index >= 0 )
            {
                arrayObjects[index] = arrayObj;
            }else
            {
                qDebug("Create new obj=%d\n",arrayObj.id);
                arrayObjects.push_back(arrayObj);
            }
        }else if(head.typeRequest == LIST_VECTOR)
        {
            // преобразуем буфер в структуру
            TSendListVectorsVisSimple vecPacket;
            readPayload(vecPacket, rec_udp.buffer, sizeBuf);
            // кол-во точек не может превышать размер принятых данных
            const size_t offsetPoints = offsetof(TSendListVectorsVisSimple, points);
            size_t maxNum = sizeBuf > offsetPoints ?
                        (sizeBuf - offsetPoints) / sizeof(glm::vec3) : 0;
            size_t num = qMin<size_t>(vecPacket.num, maxNum);

            vecList.clear();
            for(size_t i = 0 ; i < num; i++)
            {
                vecList.push_back(vecPacket.points[i]);
            }
        }
    }
    return true;
}

void VisUDP::processPendingDatagrams()
{
//    QByteArray datagram;
//    do
//    {
//        datagram.resize(udpSocketRecive.pendingDatagramSize());
//        udpSocketRecive.readDatagram(datagram.data(), datagram.size());
//    }while (udpSocketRecive.hasPendingDatagrams());

//    //! чтение заголовка
//    QDataStream outHead(&datagram,QIODevice::ReadOnly);
//    outHead.setVersion(QDataStream::Qt_4_2);
//    outHead.readRawData((char*)&head,sizeof(THeadRequest));

//    printf("sizeof(TRec_Flight_Obj)=%d,\t datagram.size()=%d\n",
//           sizeof(TRec_Flight_Obj),
//           datagram.size());
//    if(testObjInList(head.uid_user)==true)
//    {
//        qDebug("refresh param\n");
//        outHead.readRawData((char*)&flightObjects[head.uid_user],sizeof(TVis));
//    }
//    else
//    {
//       qDebug("new param\n");
//       flightObjects.resize(flightObjects.size()+1);
//       outHead.readRawData((char*)&(flightObjects.back()),sizeof(TVis));
//    }
}
int VisUDP::testFlightObjInList(int uid)
{
    //! поиск объекта с уже существующим ID
    for(int i = 0;i < flightObjects.size();i++)
    {
        if(flightObjects[i].id == uid)
        {
            return i;
        }
    }
    return -1;
}
int VisUDP::testArrayObjInList(int uid)
{
    //! поиск объекта с уже существующим ID
    for(int i = 0;i < arrayObjects.size();i++)
    {
        if(arrayObjects[i].id == uid)
        {
            return i;
        }
    }
    return -1;
}
//void VisUDP::addToFlightObjList(TVis body)
//{
//    //! признак обнаружения объектов с одинаковыми id
//    bool find=false;
//    //! поиск объекта с уже существующим ID
//    for(int i=0;i<flightObjects.size();i++)
//    {
//        if(flightObjects[i].id==body.id)
//        {
//            flightObjects[i]=body;
//            find=true;
//        }
//    }
//    if(find==false)
//        flightObjects.push_back(body);
//}

void VisUDP::sendData(TTypeHead type,char *ptr, int size)
{
    if(size < 0 || size > SIZE_BUF_DEFAULT)
        return;
    send_udp.head.typeRequest   = type;
    send_udp.head.tics          = 0;
    send_udp.head.dtTics        = 0;
    send_udp.head.id            = 0;
    send_udp.head.size          = sizeof(TMRequest) - (SIZE_BUF_DEFAULT - size);

    send_udp.compressed = 0;
    send_udp.sizeBuf    = size;
    send_udp.err        = 0;
    memcpy((void*)send_udp.buffer,(void*)ptr,size);

    udpSocketSend.writeDatagram((char*)&send_udp,
                                send_udp.head.size,
                                QHostAddress::LocalHost,4998);


}

