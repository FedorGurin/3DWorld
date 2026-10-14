#include "terrainDownloader.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QTextStream>
#include <QRegExp>
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
    imagery  = true;
    imgPending = 0;
    imgZoom = 0;
    imgX0 = imgX1 = imgY0 = imgY1 = 0;
    // тот же источник снимков, что и при подгрузке рельефа (см. TerrainStreamer)
    imageryUrl = "https://tiles.maps.eox.at/wmts/1.0.0/s2cloudless_3857/default/g/{z}/{y}/{x}.jpg";
    const QByteArray envUrl = qgetenv("TERRAIN_IMAGERY_URL");
    if(envUrl.isEmpty() == false)
        imageryUrl = QString::fromUtf8(envUrl);
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
    if(reply->property("imagery").toBool())
    {
        QImage image;
        if(reply->error() == QNetworkReply::NoError)
        {
            QByteArray data = reply->readAll();
            if(image.loadFromData(data))
            {
                const QString path = reply->property("path").toString();
                QDir().mkpath(QFileInfo(path).path());
                QFile f(path);
                if(f.open(QIODevice::WriteOnly))
                    f.write(data);
            }
        }
        imageTileReady(x, y, image);
        return;
    }
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
    ascFileName = fileName;
    if(imagery)
        startImagery();
    else
        emit finished(fileName);
}

void TerrainDownloader::startImagery()
{
    // снимок ~2048 пикселей по широте
    const double pixel = areaKm * 1000.0 / 2048.0;
    const double pixelAtZ0 = 156543.03 * cos(centerLat * M_PI / 180.0);
    imgZoom = qBound(1, int(ceil(log2(pixelAtZ0 / pixel))), 17);
    int count = 0;
    for(;;)
    {
        imgX0 = int(floor(lonToTileX(lonMin, imgZoom)));
        imgX1 = int(floor(lonToTileX(lonMax, imgZoom)));
        imgY0 = int(floor(latToTileY(latMax, imgZoom)));
        imgY1 = int(floor(latToTileY(latMin, imgZoom)));
        count = (imgX1 - imgX0 + 1) * (imgY1 - imgY0 + 1);
        if(count <= 196 || imgZoom <= 1)
            break;
        imgZoom--;
    }
    imgTiles.clear();
    imgPending = count;
    const QString src = QString::number(qHash(imageryUrl), 16);
    for(int x = imgX0; x <= imgX1; x++)
    {
        for(int y = imgY0; y <= imgY1; y++)
        {
            const QString path = QString("%1/imagery_%2/%3/%4/%5.img")
                    .arg(cacheDir, src).arg(imgZoom).arg(x).arg(y);
            QImage cached;
            if(QFileInfo::exists(path) && cached.load(path))
            {
                imageTileReady(x, y, cached);
                continue;
            }
            QString url = imageryUrl;
            url.replace("{z}", QString::number(imgZoom));
            url.replace("{x}", QString::number(x));
            url.replace("{y}", QString::number(y));
            QNetworkRequest request{QUrl(url)};
            request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
            request.setRawHeader("User-Agent", "3DWorld terrain loader");
            QNetworkReply *reply = manager->get(request);
            reply->setProperty("tileX", x);
            reply->setProperty("tileY", y);
            reply->setProperty("imagery", true);
            reply->setProperty("path", path);
        }
    }
}

void TerrainDownloader::imageTileReady(int x, int y, const QImage &image)
{
    if(image.isNull() == false)
        imgTiles.insert(tileKey(x, y), image.convertToFormat(QImage::Format_RGB32));
    imgPending--;
    if(imgPending == 0)
        buildImagery();
}

void TerrainDownloader::buildImagery()
{
    // снимок сохраняется, только если загружены все тайлы
    const int total = (imgX1 - imgX0 + 1) * (imgY1 - imgY0 + 1);
    if(imgTiles.size() == total)
    {
        // пиксели снимка - регулярная сетка по широте и долготе, как у карты
        const int H = 2048;
        const int W = qMax(2, int(H * (lonMax - lonMin) / (latMax - latMin) *
                                  cos(centerLat * M_PI / 180.0) + 0.5));
        QImage out(W, H, QImage::Format_RGB32);
        for(int r = 0; r < H; r++)
        {
            const double lat = latMax - (r + 0.5) / H * (latMax - latMin);
            const double py = latToTileY(lat, imgZoom) * TILE_SIZE;
            QRgb *line = reinterpret_cast<QRgb*>(out.scanLine(r));
            for(int c = 0; c < W; c++)
            {
                const double lon = lonMin + (c + 0.5) / W * (lonMax - lonMin);
                const double px = lonToTileX(lon, imgZoom) * TILE_SIZE;
                const int tx = qBound(imgX0, int(floor(px / TILE_SIZE)), imgX1);
                const int ty = qBound(imgY0, int(floor(py / TILE_SIZE)), imgY1);
                const QImage &tile = imgTiles[tileKey(tx, ty)];
                const int ix = qBound(0, int(px) - tx * TILE_SIZE, tile.width() - 1);
                const int iy = qBound(0, int(py) - ty * TILE_SIZE, tile.height() - 1);
                line[c] = tile.pixel(ix, iy);
            }
        }
        QString imgName = ascFileName;
        imgName.replace(QRegExp("\\.asc$"), ".jpg");
        out.save(imgName, "JPG", 90);
        imgTiles.clear();
        emit finished(ascFileName);
    }else
    {
        imgTiles.clear();
        emit finished(ascFileName);
        emit failed(QString::fromUtf8("Снимок области загружен не полностью - рельеф без снимка"));
    }
}
