#include "visudp.h"

VisUDP::VisUDP()
{
    udpSocketRecive.bind(4801);
    connect(&udpSocketRecive, SIGNAL(readyRead()),this, SLOT(processPendingDatagrams()));
}
void VisUDP::processPendingDatagrams()
{
    QByteArray datagram;
    THeadRequest head;
    do
    {
        datagram.resize(udpSocketRecive.pendingDatagramSize());
        udpSocketRecive.readDatagram(datagram.data(), datagram.size());
    }while (udpSocketRecive.hasPendingDatagrams());

    QDataStream outHead(&datagram,QIODevice::ReadOnly);
    //outHead.setVersion(QDataStream::Qt_4_2);
    outHead.readRawData((char*)&flightObj,sizeof(TRec_Flight_Obj));
    //outHead.setByteOrder(QDataStream::LittleEndian);

    //QDataStream out(&datagram,QIODevice::ReadOnly);
    //out.setVersion(QDataStream::Qt_4_2);
    //out.setByteOrder(QDataStream::LittleEndian);

    //qDebug("Data is resived, size=%d\n",flightObj.head.size);
    //if(head.type==OBJ_AIRCRAFT || head.type==OBJ_ASP)
    if(true)
    {
        //out.readRawData((char*)&flightObj.object,flightObj.head.size);
        //qDebug("code=%d\n",flightObj.object.code);
        //qDebug("id=%d\n",flightObj.object.id);
    }
    else
    {
        qDebug("Unknown type packet\n");
        return;
    }

    addToFlightObjList(flightObj.object);

    emit flightObjChanged();
}
void VisUDP::addToFlightObjList(TVis body)
{
    //! признак обнаружения объектов с одинаковыми id
    bool find=false;
    //! поиск объекта с уже существующим ID
    for(int i=0;i<flightObjects.size();i++)
    {
        if(flightObjects[i].id==body.id)
        {
            flightObjects[i]=body;
            find=true;
        }
    }
    if(find==false)
        flightObjects.push_back(body);
}

void VisUDP::sendData(int idObject,TCommand command, QByteArray array)
{
    send_data.id=idObject;
    send_data.command=command;
    memcpy(send_data.buffer,array.data(),array.size());
    udpSocketSend.writeDatagram((char*)&send_data,sizeof(TSend_UDP),QHostAddress::LocalHost, 4111);
}
