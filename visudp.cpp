#include "visudp.h"

VisUDP::VisUDP()
{
    flightObjects.clear();    
    udpSocketRecive.bind(4801);
    datagram.resize(sizeof(TRec_Flight_Obj));
    //connect(&udpSocketRecive, SIGNAL(readyRead()),this, SLOT(processPendingDatagrams()));
}
void VisUDP::checkDatagrams()
{
    while(udpSocketRecive.pendingDatagramSize()==sizeof(TRec_Flight_Obj))
    {
        udpSocketRecive.readDatagram(datagram.data(), datagram.size());

        //! чтение заголовка
        QDataStream outHead(&datagram,QIODevice::ReadOnly);
        outHead.setVersion(QDataStream::Qt_4_2);
        outHead.readRawData((char*)&head,sizeof(THeadRequest));

        qDebug("sizeof(TRec_Flight_Obj)=%d,\t datagram.size()=%d\n",
               sizeof(TRec_Flight_Obj),
               datagram.size());

        qDebug("head_user_id=%d\n",head.uid_user);
        //! проверка есть ли объект уже в списке
        if(testObjInList(head.uid_user)==true)
        {
            outHead.readRawData((char*)&flightObjects[head.uid_user],sizeof(TVis));
            qDebug("yg=%f\n",flightObjects[head.uid_user].yg);
        }
        else
        {
            qDebug("new param\n");
            flightObjects.resize(flightObjects.size()+1);
            outHead.readRawData((char*)&(flightObjects.back()),sizeof(TVis));
        }
    }
}

void VisUDP::processPendingDatagrams()
{
    QByteArray datagram;
    do
    {
        datagram.resize(udpSocketRecive.pendingDatagramSize());
        udpSocketRecive.readDatagram(datagram.data(), datagram.size());
    }while (udpSocketRecive.hasPendingDatagrams());

    //! чтение заголовка
    QDataStream outHead(&datagram,QIODevice::ReadOnly);
    outHead.setVersion(QDataStream::Qt_4_2);
    outHead.readRawData((char*)&head,sizeof(THeadRequest));

    printf("sizeof(TRec_Flight_Obj)=%d,\t datagram.size()=%d\n",
           sizeof(TRec_Flight_Obj),
           datagram.size());
    if(testObjInList(head.uid_user)==true)
    {
        qDebug("refresh param\n");
        outHead.readRawData((char*)&flightObjects[head.uid_user],sizeof(TVis));
    }
    else
    {
       qDebug("new param\n");
       flightObjects.resize(flightObjects.size()+1);
       outHead.readRawData((char*)&(flightObjects.back()),sizeof(TVis));
    }
}
bool VisUDP::testObjInList(unsigned uid)
{
    //! признак обнаружения объектов с одинаковыми id
    bool find=false;
    //! поиск объекта с уже существующим ID
    for(int i=0;i<flightObjects.size();i++)
    {
        if(flightObjects[i].id==uid)
        {
            find=true;
        }
    }
    return find;
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

void VisUDP::sendData(int idObject,TCommand command, QByteArray array)
{
    send_data.id=idObject;
    send_data.command=command;
    memcpy(send_data.buffer,array.data(),array.size());
    udpSocketSend.writeDatagram((char*)&send_data,sizeof(TSend_UDP),QHostAddress::LocalHost, 4111);
}
