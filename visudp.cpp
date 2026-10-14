#include "visudp.h"
#include <iostream>
#include <QDataStream>
#include <cstddef>
#include <cstring>
#include <math.h>
VisUDP::VisUDP()
{
    synch_time = 0;
    blockConnect = false;
    smoothing    = true;
    playing      = false;
    playIndex    = 0;
    recordStartMs = 0;
    flightObjects.clear();
    memset(&head, 0, sizeof(head));
    memset(&rec_udp, 0, sizeof(rec_udp));
    memset(&send_udp, 0, sizeof(send_udp));
    if(udpSocketRecive.bind(7030) == false)
        std::cout<<"Can`t bind to socket port" << std::endl;
    // при большом числе объектов пакеты приходят потоком - увеличиваем буфер сокета
    udpSocketRecive.setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 8 * 1024 * 1024);
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
bool VisUDP::checkDatagrams(qint64 nowMs)
{
    if(udpSocketRecive.hasPendingDatagrams() == false)
        return false;

    bool received = false;
    while(udpSocketRecive.hasPendingDatagrams())
    {
        qint64 sizeDatagram = udpSocketRecive.pendingDatagramSize();
        if(sizeDatagram < 0)
            break;
        datagram.resize(qMax<qint64>(sizeDatagram, 1));
        sizeDatagram = udpSocketRecive.readDatagram(datagram.data(), datagram.size());
        if(sizeDatagram <= 0)
            continue;
        datagram.resize(int(sizeDatagram));
        // при воспроизведении записи сетевые пакеты не используются
        if(isPlaying())
            continue;
        if(recordFile.isOpen())
            writeRecord(nowMs, datagram);
        processDatagram(datagram, nowMs);
        received = true;
    }
    return received;
}

void VisUDP::processDatagram(const QByteArray &datagram, qint64 nowMs)
{
    const qint64 sizeDatagram = datagram.size();
    // пакет короче заголовка - пропускаем
    if(sizeDatagram < static_cast<qint64>(sizeof(THRequest)))
        return;

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
        return;
    }
    if(head.typeRequest != PARAM_OBJ &&
       head.typeRequest != PARAM_OBJ_LIST &&
       head.typeRequest != PARAM_ARRAY &&
       head.typeRequest != LIST_VECTOR)
        return;

    // пакет должен содержать хотя бы заголовок буфера данных
    const qint64 offsetBuf = offsetof(TMRequest, buffer);
    if(sizeDatagram < offsetBuf)
        return;
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
        updateFlightObj(solid, nowMs);
        synch_time = solid.time;
    }else if(head.typeRequest == PARAM_OBJ_LIST)
    {
        // uint32 - кол-во объектов, далее массив TSendVehicleVisSimple
        if(sizeBuf < sizeof(uint32_t))
            return;
        uint32_t count = 0;
        memcpy(&count, rec_udp.buffer, sizeof(count));
        const uint32_t maxCount = (sizeBuf - sizeof(uint32_t)) / sizeof(TSendVehicleVisSimple);
        count = qMin(count, maxCount);
        for(uint32_t i = 0; i < count; i++)
        {
            TSendVehicleVisSimple solid;
            readPayload(solid, rec_udp.buffer + sizeof(uint32_t) + i * sizeof(TSendVehicleVisSimple),
                        sizeof(TSendVehicleVisSimple));
            updateFlightObj(solid, nowMs);
            synch_time = solid.time;
        }
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
    return flightIndex.value(uid, -1);
}
void VisUDP::updateFlightObj(const TSendVehicleVisSimple &solid, qint64 nowMs)
{
    int index = testFlightObjInList(solid.id);
    // индекс мог устареть, если список меняли напрямую (setSizeObj)
    if(index >= flightObjects.size() || (index >= 0 && flightObjects[index].id != solid.id))
    {
        rebuildIndex();
        index = testFlightObjInList(solid.id);
    }
    if(index >= 0)
    {
        TObjSample &smp = samples[index];
        smp.prev    = smp.last;
        smp.tPrev   = smp.tLast;
        smp.hasPrev = true;
        smp.last    = solid;
        smp.tLast   = nowMs;
        flightObjects[index] = solid;
    }else
    {
        qDebug("Create new obj=%d (total %d)", solid.id, flightObjects.size() + 1);
        TObjSample smp;
        smp.last    = solid;
        smp.prev    = solid;
        smp.tLast   = nowMs;
        smp.tPrev   = nowMs;
        smp.hasPrev = false;
        flightIndex.insert(solid.id, flightObjects.size());
        flightObjects.push_back(solid);
        samples.push_back(smp);
    }
}

//! интерполяция угла по кратчайшей дуге, рад
static double lerpAngle(double a, double b, double t)
{
    double d = fmod(b - a, 2.0 * M_PI);
    if(d > M_PI)  d -= 2.0 * M_PI;
    if(d < -M_PI) d += 2.0 * M_PI;
    return a + d * t;
}

void VisUDP::updateDisplay(qint64 nowMs)
{
    if(samples.size() != flightObjects.size())
        return;
    for(int i = 0; i < samples.size(); i++)
    {
        const TObjSample &smp = samples[i];
        TSendVehicleVisSimple v = smp.last;
        if(smoothing && smp.hasPrev && smp.tLast > smp.tPrev)
        {
            const double interval = (smp.tLast - smp.tPrev) * 0.001;
            const double since    = qBound(0.0, (nowMs - smp.tLast) * 0.001, 1.0);
            const double vg2 = smp.last.vg_x * smp.last.vg_x +
                               smp.last.vg_y * smp.last.vg_y +
                               smp.last.vg_z * smp.last.vg_z;
            // углы - интерполяция между двумя последними пакетами с запаздыванием
            // на один интервал (плавно при любой частоте пакетов)
            const double t = qBound(0.0, since / interval, 1.0);
            v.psi   = lerpAngle(smp.prev.psi,   smp.last.psi,   t);
            v.gamma = lerpAngle(smp.prev.gamma, smp.last.gamma, t);
            v.tan   = smp.prev.tan + (smp.last.tan - smp.prev.tan) * t;
            if(vg2 > 1e-6)
            {
                // передана скорость в земной СК - экстраполяция без запаздывания
                v.cg_x = smp.last.cg_x + smp.last.vg_x * since;
                v.cg_y = smp.last.cg_y + smp.last.vg_y * since;
                v.cg_z = smp.last.cg_z + smp.last.vg_z * since;
            }else
            {
                v.cg_x = smp.prev.cg_x + (smp.last.cg_x - smp.prev.cg_x) * t;
                v.cg_y = smp.prev.cg_y + (smp.last.cg_y - smp.prev.cg_y) * t;
                v.cg_z = smp.prev.cg_z + (smp.last.cg_z - smp.prev.cg_z) * t;
            }
        }
        flightObjects[i] = v;
    }
}

QList<int> VisUDP::removeStale(qint64 nowMs, qint64 timeoutMs)
{
    QList<int> removed;
    if(timeoutMs <= 0 || samples.size() != flightObjects.size())
        return removed;
    for(int i = samples.size() - 1; i >= 0; i--)
    {
        if(nowMs - samples[i].tLast > timeoutMs)
        {
            removed.append(flightObjects[i].id);
            samples.remove(i);
            flightObjects.remove(i);
        }
    }
    if(removed.isEmpty() == false)
    {
        rebuildIndex();
        qDebug("Removed %d silent objects (total %d)", removed.size(), flightObjects.size());
    }
    return removed;
}

void VisUDP::clearObjects()
{
    flightObjects.clear();
    samples.clear();
    arrayObjects.clear();
    vecList.clear();
    flightIndex.clear();
}

bool VisUDP::startRecording(const QString &fileName, qint64 nowMs)
{
    stopRecording();
    recordFile.setFileName(fileName);
    if(recordFile.open(QIODevice::WriteOnly) == false)
        return false;
    recordFile.write(RECORD_MAGIC, 8);
    recordStartMs = nowMs;
    return true;
}

void VisUDP::stopRecording()
{
    if(recordFile.isOpen())
        recordFile.close();
}

void VisUDP::writeRecord(qint64 nowMs, const QByteArray &data)
{
    // запись: время от начала, мс (int64) + размер (uint32) + пакет
    const qint64 t = nowMs - recordStartMs;
    const quint32 size = data.size();
    recordFile.write(reinterpret_cast<const char*>(&t), sizeof(t));
    recordFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
    recordFile.write(data);
}

bool VisUDP::loadPlayback(const QString &fileName)
{
    QFile f(fileName);
    if(f.open(QIODevice::ReadOnly) == false)
        return false;
    QByteArray all = f.readAll();
    if(all.size() < 8 || memcmp(all.constData(), RECORD_MAGIC, 8) != 0)
        return false;
    playRecords.clear();
    int pos = 8;
    while(pos + 12 <= all.size())
    {
        qint64 t;
        quint32 size;
        memcpy(&t, all.constData() + pos, sizeof(t));
        memcpy(&size, all.constData() + pos + 8, sizeof(size));
        pos += 12;
        if(size > quint32(all.size() - pos))
            break;
        playRecords.append(qMakePair(t, all.mid(pos, int(size))));
        pos += int(size);
    }
    playIndex = 0;
    playing   = playRecords.isEmpty() == false;
    clearObjects();
    return playing;
}

void VisUDP::stopPlayback()
{
    playing = false;
    playRecords.clear();
    playIndex = 0;
}

qint64 VisUDP::playbackDuration() const
{
    return playRecords.isEmpty() ? 0 : playRecords.last().first;
}

void VisUDP::advancePlayback(qint64 positionMs)
{
    if(playing == false)
        return;
    // перемотка назад - воспроизводим заново с начала
    if(playIndex > 0 && positionMs < playRecords[playIndex - 1].first)
    {
        clearObjects();
        playIndex = 0;
        emit resetTrajectory();
    }
    while(playIndex < playRecords.size() && playRecords[playIndex].first <= positionMs)
    {
        processDatagram(playRecords[playIndex].second, playRecords[playIndex].first);
        playIndex++;
    }
}

void VisUDP::rebuildIndex()
{
    flightIndex.clear();
    for(int i = 0; i < flightObjects.size(); i++)
        flightIndex.insert(flightObjects[i].id, i);
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

