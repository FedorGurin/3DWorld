#include <QApplication>
#include <QNetworkProxyFactory>
#include "view3DArea.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // загрузка рельефа из интернета: учитываем системные настройки прокси
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    view3DArea *area3D = new view3DArea;
    area3D->show();
    // аргументы: [файл с траекторией] [--terrain широта,долгота[,размер_км]]
    //           [--terrain-stream широта,долгота] [--record файл.rec] [--play файл.rec]
    for(int i = 1; i < argc; i++)
    {
        QString arg = QString::fromLocal8Bit(argv[i]);
        if(arg == "--terrain" && i + 1 < argc)
        {
            QStringList v = QString::fromLocal8Bit(argv[++i]).split(",");
            if(v.size() >= 2)
                area3D->loadTerrainOnline(v[0].toDouble(), v[1].toDouble(),
                                          v.size() > 2 ? v[2].toDouble() : 60.0);
        }else if(arg == "--record" && i + 1 < argc)
        {
            area3D->startRecording(QString::fromLocal8Bit(argv[++i]));
        }else if(arg == "--play" && i + 1 < argc)
        {
            area3D->startPlayback(QString::fromLocal8Bit(argv[++i]));
        }else if(arg == "--terrain-stream" && i + 1 < argc)
        {
            QStringList v = QString::fromLocal8Bit(argv[++i]).split(",");
            if(v.size() >= 2)
                area3D->startTerrainStreaming(v[0].toDouble(), v[1].toDouble());
        }else
            area3D->loadTXTFile(arg);
    }

    return a.exec();
}
