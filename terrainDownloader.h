#ifndef TERRAINDOWNLOADER_H
#define TERRAINDOWNLOADER_H

#include <QObject>
#include <QImage>
#include <QMap>
#include <QString>
#include <QVector>

class QNetworkAccessManager;
class QNetworkReply;

//! загрузка карты высот из интернета (открытый набор AWS Terrain Tiles,
//! формат Terrarium) и сохранение ее в файл ESRI ASCII Grid
class TerrainDownloader : public QObject
{
    Q_OBJECT
public:
    explicit TerrainDownloader(QObject *parent = nullptr);

    //! начать загрузку области с центром lat, lon (град) и стороной sizeKm (км);
    //! gridSize - кол-во узлов сетки по широте
    bool start(double lat, double lon, double sizeKm, int gridSize = 400);
    //! идет ли загрузка
    bool isRunning() const {return pending > 0 || imgPending > 0;}
    //! загружать спутниковый снимок области (сохраняется рядом с картой, .jpg)
    bool imagery;
    //! адрес тайлов снимков: {z}, {x}, {y}
    QString imageryUrl;
    //! адрес тайлов: {z}, {x}, {y} заменяются номерами тайла
    QString urlTemplate;
    //! каталог для кэша тайлов и готовых карт
    QString cacheDir;
signals:
    //! ход загрузки: загружено тайлов из общего числа
    void progress(int done, int total);
    //! карта сохранена в файл fileName
    void finished(const QString &fileName);
    //! ошибка загрузки
    void failed(const QString &message);
private slots:
    void replyFinished(QNetworkReply *reply);
private:
    //! ключ тайла
    static qint64 tileKey(int x, int y) {return (qint64(x) << 32) | qint64(y);}
    //! запрос одного тайла (из кэша или из сети)
    void requestTile(int x, int y);
    //! тайл загружен
    void tileReady(int x, int y, const QImage &image);
    //! все тайлы загружены - строим сетку и сохраняем файл
    void buildGrid();
    //! высота в точке, м (билинейная интерполяция по тайлам)
    double sampleHeight(double lat, double lon) const;
    QString tilePath(int x, int y) const;
    //! загрузка снимка области после построения карты
    void startImagery();
    void imageTileReady(int x, int y, const QImage &image);
    //! сборка снимка в сетку карты и сохранение
    void buildImagery();

    QMap<qint64, QImage> imgTiles;
    int imgPending;
    int imgZoom;
    int imgX0, imgX1, imgY0, imgY1;
    QString ascFileName;

    QNetworkAccessManager *manager;
    QMap<qint64, QImage> tiles;
    int pending;
    int total;
    int zoom;
    int tileX0, tileX1, tileY0, tileY1;
    double latMin, latMax, lonMin, lonMax;
    double centerLat, centerLon, areaKm;
    int nRows;
    bool hasError;
};

#endif // TERRAINDOWNLOADER_H
