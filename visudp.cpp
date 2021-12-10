#include "visudp.h"
#include <iostream>
#include <QDataStream>
VisUDP::VisUDP()
{
    synch_time = 0;
    flightObjects.clear();
    if(udpSocketRecive.bind(7030) == false)
        std::cout<<"Can`t bind to socket port" << std::endl;
    datagram.resize(sizeof(TMRequest));
}
bool VisUDP::checkDatagrams()
{

    if(udpSocketRecive.hasPendingDatagrams() == false)
        return false;

    while(udpSocketRecive.hasPendingDatagrams())
    {
        udpSocketRecive.readDatagram(datagram.data(), udpSocketRecive.pendingDatagramSize());
        //! чтение заголовка
        QDataStream outHead(&datagram,QIODevice::ReadOnly);
        outHead.setVersion(QDataStream::Qt_4_6);
        outHead.readRawData(reinterpret_cast<char* >(&head),sizeof(THRequest));
        QDataStream out(&datagram,QIODevice::ReadOnly);
        out.setVersion(QDataStream::Qt_4_6);
        //out.setByteOrder(QDataStream::BigEndian);
        //! обработка пакетов
        if(head.typeRequest == PARAM_OBJ)
        {
            //! чтение заголовка
            out.readRawData(reinterpret_cast<char* >(&rec_udp),head.size);
            QByteArray compBuf(reinterpret_cast<char* >(rec_udp.buffer),rec_udp.sizeBuf);

            //! преобразуем буфер в структуру
            TSendVehicleVisSimple *solid = reinterpret_cast<TSendVehicleVisSimple* > (rec_udp.buffer);

            /*if(rec_udp.compressed==1)
            {
                qDebug("find compress\n");
                //! распаковать буфер
                QByteArray compBuf(rec_udp.buffer,rec_udp.sizeBuf);
                QByteArray uncompBuf=qUncompress(compBuf);
                TTrans3DWorld* solid_obj=(TTrans3DWorld*)(uncompBuf.data());
            }*/

            int index = testFlightObjInList(solid->id);
            if(index >= 0 )
            {
                flightObjects[index] = *solid;
            }else if(index<0)
            {
                qDebug("Сreate new obj=%d\n",solid->id);
                flightObjects.push_back(*solid);
            }
            synch_time = solid->time;
        }else if(head.typeRequest == PARAM_ARRAY)
        {
            //! чтение заголовка
            out.readRawData(reinterpret_cast<char* >(&rec_udp),head.size);
            QByteArray compBuf(reinterpret_cast<char* >(rec_udp.buffer),rec_udp.sizeBuf);

            //! преобразуем буфер в структуру
            TSendArrayVisSimple *arrayObj = reinterpret_cast<TSendArrayVisSimple* > (rec_udp.buffer);

            /*if(rec_udp.compressed==1)
            {
                qDebug("find compress\n");
                //! распаковать буфер
                QByteArray compBuf(rec_udp.buffer,rec_udp.sizeBuf);
                QByteArray uncompBuf=qUncompress(compBuf);
                TTrans3DWorld* solid_obj=(TTrans3DWorld*)(uncompBuf.data());
            }*/

            int index = testArrayObjInList(arrayObj->id);
            if(index >= 0 )
            {
                arrayObjects[index] = *arrayObj;
            }else if(index<0)
            {
                qDebug("Сreate new obj=%d\n",arrayObj->id);
                arrayObjects.push_back(*arrayObj);
            }
            //synch_time = solid->time;

        }else if(head.typeRequest == COMMAND)
        {
            TCRequest req;
            //out.readRawData((char*)&req,head.size);

            if(req.com==TC_RESET)
                emit resetTrajectory();
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
int VisUDP::testFlightObjInList(unsigned uid)
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
int VisUDP::testArrayObjInList(unsigned uid)
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

