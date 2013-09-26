#include "visudp.h"

VisUDP::VisUDP()
{
    flightObjects.clear();    
    bool result=udpSocketRecive.bind(4910);
    if(result==false)
        qDebug("bind is fail\n");
    datagram.resize(sizeof(TMRequest));
}
void VisUDP::checkDatagrams()
{
    if(udpSocketRecive.hasPendingDatagrams()==true)
    {
        udpSocketRecive.readDatagram(datagram.data(), datagram.size());
        //! чтение заголовка
        QDataStream outHead(&datagram,QIODevice::ReadOnly);
        outHead.setVersion(QDataStream::Qt_4_6);
        outHead.readRawData((char*)&head,sizeof(THRequest));       
        QDataStream out(&datagram,QIODevice::ReadOnly);
        out.setVersion(QDataStream::Qt_4_6);
        if(head.typeRequest==FLIGHT_OBJ)
        {
            TMRequest req;
            out.readRawData((char*)&req,head.size);
            //! распаковать буффер
            QByteArray compBuf(req.buffer,req.sizeBuf);
            QByteArray uncompBuf=qUncompress(compBuf);

            TTrans3DWorld* solid_obj=(TTrans3DWorld*)(uncompBuf.data());
            for(int i=0;i<solid_obj->sizeObj;i++)
            {
                //! проверка есть ли объект уже в списке
                if(testObjInList(solid_obj->obj[i].id)==true)
                {
                    flightObjects[solid_obj->obj[i].id]=solid_obj->obj[i];
                }
                else
                {
                    flightObjects.push_back(solid_obj->obj[i]);
                }
            }
        }else if(head.typeRequest==COMMAND)
        {
            TCRequest req;
            out.readRawData((char*)&req,head.size);

            if(req.com==TC_RESET)
                emit resetTrajectory();
        }
    }
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

//void VisUDP::sendData(int idObject,TCommand command, QByteArray array)
//{
//    send_data.id=idObject;
//    send_data.command=command;
//    memcpy(send_data.buffer,array.data(),array.size());
//    udpSocketSend.writeDatagram((char*)&send_data,sizeof(TSend_UDP),QHostAddress::LocalHost, 4111);
//}
