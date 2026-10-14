#include "terrainDownloader.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QTextStream>
#include <math.h>

//! размер тайла, пиксели
static const int TILE_SIZE = 256;
//! максимальное кол-во тайлов в одной загрузке
static const int MAX_TILES = 100;
//! максимальный уровень детализации набора Terrarium
static const int MAX_ZOOM = 14;

static double lonToTileX(double lon, int z)
{
    return (lon + 180.0) / 360.0 * (1 << z);
}
static double latToTileY(double lat, int z)
{
    double r = lat * M_PI / 180.0;
    return (1.0 - log(tan(r) + 1.0 / cos(r)) / M_PI) / 2.0 * (1 << z);
}

TerrainDownloader::TerrainDownloader(QObject *parent) : QObject(parent)
{
    urlTemplate = "https://s3.amazonaws.com/elevation-tiles-prod/terrarium/{z}/{x}/{y}.png";
    cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/terrain";
    manager  = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    pending = total = 0;
    zoom = 0;
    tileX0 = tileX1 = tileY0 = tileY1 = 0;
    latMin = latMax = lonMin = lonMax = 0.0;
    centerLat = centerLon = areaKm = 0.0;
    nRows = 0;
    hasError = false;
}

bool TerrainDownloader::start(double lat, double lon, double sizeKm, int gridSize)
{
    if(isRunning())
        return false;
    if(fabs(lat) > 84.0 || sizeKm <= 0.0 || gridSize < 2)
    {
        emit failed(QString::fromUtf8("Неверные параметры области"));
        return false;
    }
    centerLat = lat;
    centerLon = lon;
    areaKm    = sizeKm;
    nRows     = gridSize;

    // область - квадрат со стороной sizeKm
    const double dLat = sizeKm / 111.32 / 2.0;
    const double dLon = dLat / cos(lat * M_PI / 180.0);
    latMin = lat - dLat; latMax = lat + dLat;
    lonMin = lon - dLon; lonMax = lon + dLon;

    // уровень детализации: размер пикселя тайла не крупнее шага сетки
    const double step = sizeKm * 1000.0 / (gridSize - 1);
    const double pixelAtZ0 = 156543.03 * cos(lat * M_PI / 180.0);
    zoom = int(ceil(log2(pixelAtZ0 / step)));
    zoom = qBound(1, zoom, MAX_ZOOM);
    // ограничиваем кол-во тайлов
    for(;;)
    {
        tileX0 = int(floor(lonToTileX(lonMin, zoom)));
        tileX1 = int(floor(lonToTileX(lonMax, zoom)));
        tileY0 = int(floor(latToTileY(latMax, zoom)));
        tileY1 = int(floor(latToTileY(latMin, zoom)));
        total = (tileX1 - tileX0 + 1) * (tileY1 - tileY0 + 1);
        if(total <= MAX_TILES || zoom <= 1)
            break;
        zoom--;
    }

    tiles.clear();
    hasError = false;
    pending  = total;
    emit progress(0, total);
    for(int x = tileX0; x <= tileX1; x++)
        for(int y = tileY0; y <= tileY1; y++)
            requestTile(x, y);
    return true;
}

QString TerrainDownloader::tilePath(int x, int y) const
{
    return QString("%1/%2/%3/%4.png").arg(cacheDir).arg(zoom).arg(x).arg(y);
}

void TerrainDownloader::requestTile(int x, int y)
{
    // тайл уже загружался раньше - берем из кэша
    QImage cached;
    if(QFileInfo::exists(tilePath(x, y)) && cached.load(tilePath(x, y)))
    {
        tileReady(x, y, cached);
        return;
    }
    QString url = urlTemplate;
    url.replace("{z}", QString::number(zoom));
    url.replace("{x}", QString::number(x));
    url.replace("{y}", QString::number(y));
    QNetworkRequest request{QUrl(url)};
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setRawHeader("User-Agent", "3DWorld terrain loader");
    QNetworkReply *reply = manager->get(request);
    reply->setProperty("tileX", x);
    reply->setProperty("tileY", y);
}

void TerrainDownloader::replyFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    const int x = reply->property("tileX").toInt();
    const int y = reply->property("tileY").toInt();
    QImage image;
    if(reply->error() != QNetworkReply::NoError)
    {
        if(hasError == false)
            emit failed(QString::fromUtf8("Ошибка загрузки: %1").arg(reply->errorString()));
        hasError = true;
    }else
    {
        QByteArray data = reply->readAll();
        if(image.loadFromData(data, "PNG"))
        {
            QDir().mkpath(QFileInfo(tilePath(x, y)).path());
            QFile f(tilePath(x, y));
            if(f.open(QIODevice::WriteOnly))
                f.write(data);
        }
    }
    tileReady(x, y, image);
}

void TerrainDownloader::tileReady(int x, int y, const QImage &image)
{
    if(image.isNull() == false)
        tiles.insert(tileKey(x, y), image.convertToFormat(QImage::Format_RGB32));
    pending--;
    emit progress(total - pending, total);
    if(pending == 0)
    {
        if(hasError)
            return;
        buildGrid();
    }
}

double TerrainDownloader::sampleHeight(double lat, double lon) const
{
    // пиксельные координаты в проекции Меркатора для уровня zoom
    const double px = lonToTileX(lon, zoom) * TILE_SIZE - 0.5;
    const double py = latToTileY(lat, zoom) * TILE_SIZE - 0.5;
    const int x0 = int(floor(px)), y0 = int(floor(py));
    const double fx = px - x0, fy = py - y0;

    auto pixel = [this](int gx, int gy) -> double
    {
        const int tx = int(floor(double(gx) / TILE_SIZE));
        const int ty = int(floor(double(gy) / TILE_SIZE));
        auto it = tiles.find(tileKey(tx, ty));
        if(it == tiles.end())
        {
            // соседний тайл за границей области - берем ближайший загруженный пиксель
            const int cx = qBound(tileX0, tx, tileX1);
            const int cy = qBound(tileY0, ty, tileY1);
            it = tiles.find(tileKey(cx, cy));
            if(it == tiles.end())
                return 0.0;
            gx = qBound(cx * TILE_SIZE, gx, cx * TILE_SIZE + TILE_SIZE - 1);
            gy = qBound(cy * TILE_SIZE, gy, cy * TILE_SIZE + TILE_SIZE - 1);
        }
        const QImage &img = it.value();
        QRgb c = img.pixel(gx - int(floor(double(gx) / TILE_SIZE)) * TILE_SIZE,
                           gy - int(floor(double(gy) / TILE_SIZE)) * TILE_SIZE);
        // формат Terrarium: высота = R*256 + G + B/256 - 32768
        return qRed(c) * 256.0 + qGreen(c) + qBlue(c) / 256.0 - 32768.0;
    };
    const double h00 = pixel(x0,     y0);
    const double h10 = pixel(x0 + 1, y0);
    const double h01 = pixel(x0,     y0 + 1);
    const double h11 = pixel(x0 + 1, y0 + 1);
    const double h0 = h00 + (h10 - h00) * fx;
    const double h1 = h01 + (h11 - h01) * fx;
    return h0 + (h1 - h0) * fy;
}

void TerrainDownloader::buildGrid()
{
    // регулярная сетка в градусах: шаг одинаковый по широте и долготе
    const double cellsize = (latMax - latMin) / (nRows - 1);
    const int nCols = int(floor((lonMax - lonMin) / cellsize)) + 1;

    QDir().mkpath(cacheDir);
    const QString fileName = QString("%1/terrain_%2_%3_%4km.asc")
            .arg(cacheDir)
            .arg(centerLat, 0, 'f', 4)
            .arg(centerLon, 0, 'f', 4)
            .arg(areaKm, 0, 'f', 0);
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text) == false)
    {
        emit failed(QString::fromUtf8("Не удалось записать файл %1").arg(fileName));
        return;
    }
    QTextStream out(&file);
    out << "ncols         " << nCols << "\n";
    out << "nrows         " << nRows << "\n";
    out << "xllcorner     " << QString::number(lonMin, 'f', 10) << "\n";
    out << "yllcorner     " << QString::number(latMin, 'f', 10) << "\n";
    out << "cellsize      " << QString::number(cellsize, 'g', 15) << "\n";
    out << "nodata_value  -9999\n";
    // строки - с севера на юг
    for(int i = 0; i < nRows; i++)
    {
        const double lat = latMax - i * cellsize;
        QString line;
        line.reserve(nCols * 8);
        for(int j = 0; j < nCols; j++)
        {
            const double lon = lonMin + j * cellsize;
            double h = sampleHeight(lat, lon);
            // глубины моря не отображаем - уровень воды 0
            if(h < 0.0)
                h = 0.0;
            if(j > 0)
                line += ' ';
            line += QString::number(h, 'f', 1);
        }
        out << line << "\n";
    }
    file.close();
    tiles.clear();
    emit finished(fileName);
}
