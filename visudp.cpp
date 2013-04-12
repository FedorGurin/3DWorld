#include "visudp.h"

VisUDP::VisUDP()
{
    flightObjects.clear();
    udpSocketRecive.bind(4801);
    connect(&udpSocketRecive, SIGNAL(readyRead()),this, SLOT(processPendingDatagrams()));
}
void VisUDP::processPendingDatagrams()
{
    QByteArray datagram;
    do
    {
        datagram.resize(udpSocketRecive.pendingDatagramSize());
        udpSocketRecive.readDatagram(datagram.data(), datagram.size());
    }while (udpSocketRecive.hasPendingDatagrams());

    QDataStream outHead(&datagram,QIODevice::ReadOnly);
    outHead.readRawData((char*)&head,sizeof(THeadRequest));

    QDataStream out(&datagram,QIODevice::ReadOnly);
    if(testObjInList(head.uid_user)==true)
    {
        out.readRawData((char*)&flightObjects[head.uid_user],sizeof(TVis));
    }else
    {
       TVis body;
       out.readRawData((char*)&body,sizeof(TVis));
       flightObjects.push_back(body);
    }

//    if(head.type==MODULE)
//        out.readRawData((char*)&answerModuleReq,answerHReq.size);
//    else if(answerHReq.type==SELECTED)
//        out.readRawData((char*)&answerSReq,     answerHReq.size);
//    else if(answerHReq.type==LISTPM)
//        out.readRawData((char*)&answerMemReq,   answerHReq.size);



    //addToFlightObjList(flightObj.object);

    //emit flightObjChanged();
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
