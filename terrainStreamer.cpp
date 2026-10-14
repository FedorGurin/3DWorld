#include "terrainStreamer.h"
#include "terrainColor.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QVector3D>
#include <math.h>
#include <algorithm>

//! радиус Земли, м
static const double EARTH_R = 6371000.0;
//! размер тайла, пиксели
static const int TILE_PX = 256;
//! одновременных запросов
static const int MAX_IN_FLIGHT = 6;
//! тайлов, компилируемых за один кадр
static const int MAX_COMPILE_PER_FRAME = 3;
//! глубина "юбки" по краям тайла, закрывающей щели между тайлами, м
static const float SKIRT_DEPTH = 300.0f;

static double tileToLon(double tx, int z)
{
    return tx / double(1 << z) * 360.0 - 180.0;
}
static double tileToLat(double ty, int z)
{
    const double n = M_PI * (1.0 - 2.0 * ty / double(1 << z));
    return atan(sinh(n)) * 180.0 / M_PI;
}
static double lonToTile(double lon, int z)
{
    return (lon + 180.0) / 360.0 * (1 << z);
}
static double latToTile(double lat, int z)
{
    const double r = lat * M_PI / 180.0;
    return (1.0 - log(tan(r) + 1.0 / cos(r)) / M_PI) / 2.0 * (1 << z);
}

TerrainStreamer::TerrainStreamer(QObject *parent) : QObject(parent)
{
    zoom   = 11;
    radius = 4;
    step   = 4;
    urlTemplate = "https://s3.amazonaws.com/elevation-tiles-prod/terrarium/{z}/{x}/{y}.png";
    cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/terrain";
    manager  = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    inFlight = 0;
    lat0 = 43.35;
    lon0 = 42.44;
    minH = 0.0f;
    centerX = centerY = -1;
}

TerrainStreamer::~TerrainStreamer()
{
}

void TerrainStreamer::setOrigin(double lat, double lon)
{
    if(fabs(lat - lat0) < 1e-9 && fabs(lon - lon0) < 1e-9)
        return;
    lat0 = qBound(-84.0, lat, 84.0);
    lon0 = lon;
    // координаты всех тайлов зависят от начала координат
    clear();
}

void TerrainStreamer::clear()
{
    for(auto it = tiles.begin(); it != tiles.end(); ++it)
        releaseList(it->list);
    tiles.clear();
    queue.clear();
    centerX = centerY = -1;
    minH = 0.0f;
}
void TerrainStreamer::releaseList(GLuint list)
{
    if(list != 0)
        deadLists.append(list);
}

QString TerrainStreamer::tilePath(int x, int y) const
{
    return QString("%1/%2/%3/%4.png").arg(cacheDir).arg(zoom).arg(x).arg(y);
}

int TerrainStreamer::readyCount() const
{
    int n = 0;
    for(const TTile &t : tiles)
        if(t.state == 2)
            n++;
    return n;
}

float TerrainStreamer::waterFraction() const
{
    long water = 0, count = 0;
    for(const TTile &t : tiles)
    {
        water += t.water;
        count += t.count;
    }
    return count > 0 ? float(water) / count : 0.0f;
}

void TerrainStreamer::update(double focusX, double focusZ)
{
    // точка фокуса в географических координатах (касательная плоскость в начале координат)
    const double lat = lat0 + focusX / EARTH_R * 180.0 / M_PI;
    const double lon = lon0 + focusZ / (EARTH_R * cos(lat0 * M_PI / 180.0)) * 180.0 / M_PI;
    const int cx = int(floor(lonToTile(lon, zoom)));
    const int cy = int(floor(latToTile(qBound(-84.0, lat, 84.0), zoom)));
    if(cx == centerX && cy == centerY)
        return;
    centerX = cx;
    centerY = cy;

    const int n = 1 << zoom;
    // выгружаем тайлы, ушедшие далеко от точки
    for(auto it = tiles.begin(); it != tiles.end();)
    {
        int dx = abs(it->x - cx);
        dx = qMin(dx, n - dx);
        if(dx > radius + 1 || abs(it->y - cy) > radius + 1)
        {
            releaseList(it->list);
            queue.removeAll(it.key());
            it = tiles.erase(it);
        }else
            ++it;
    }
    // ставим в очередь недостающие, ближние - первыми
    QList<QPair<int, qint64> > missing;
    for(int dy = -radius; dy <= radius; dy++)
    {
        for(int dx = -radius; dx <= radius; dx++)
        {
            const int tx = ((cx + dx) % n + n) % n;
            const int ty = cy + dy;
            if(ty < 0 || ty >= n)
                continue;
            const qint64 k = key(tx, ty);
            if(tiles.contains(k))
                continue;
            TTile t;
            t.x = tx; t.y = ty;
            t.state = 0;
            t.gridSize = 0;
            t.list = 0;
            t.water = t.count = 0;
            tiles.insert(k, t);
            missing.append(qMakePair(dx * dx + dy * dy, k));
        }
    }
    std::sort(missing.begin(), missing.end(),
              [](const QPair<int, qint64> &a, const QPair<int, qint64> &b) {return a.first < b.first;});
    // новые ближние тайлы - в начало очереди
    for(int i = missing.size() - 1; i >= 0; i--)
        queue.prepend(missing[i].second);
    pump();
}

void TerrainStreamer::pump()
{
    while(inFlight < MAX_IN_FLIGHT && queue.isEmpty() == false)
    {
        const qint64 k = queue.takeFirst();
        auto it = tiles.find(k);
        if(it == tiles.end() || it->state != 0)
            continue;
        // тайл из кэша
        QImage cached;
        if(QFileInfo::exists(tilePath(it->x, it->y)) && cached.load(tilePath(it->x, it->y)))
        {
            buildTile(*it, cached);
            continue;
        }
        QString url = urlTemplate;
        url.replace("{z}", QString::number(zoom));
        url.replace("{x}", QString::number(it->x));
        url.replace("{y}", QString::number(it->y));
        QNetworkRequest request{QUrl(url)};
        request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
        request.setRawHeader("User-Agent", "3DWorld terrain streamer");
        QNetworkReply *reply = manager->get(request);
        reply->setProperty("tileKey", k);
        reply->setProperty("tileZoom", zoom);
        it->state = 1;
        inFlight++;
    }
    const int pending = pendingCount();
    if(pending > 0)
        emit status(QString::fromUtf8("Подгрузка рельефа: %1 тайлов").arg(pending));
    else
        emit status(QString::fromUtf8("Рельеф загружен: %1 тайлов").arg(readyCount()));
}

void TerrainStreamer::replyFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    inFlight--;
    const qint64 k = reply->property("tileKey").toLongLong();
    auto it = tiles.find(k);
    // тайл уже выгружен или сменился уровень детализации
    if(it == tiles.end() || reply->property("tileZoom").toInt() != zoom)
    {
        pump();
        return;
    }
    QImage image;
    QByteArray data;
    if(reply->error() == QNetworkReply::NoError)
    {
        data = reply->readAll();
        image.loadFromData(data, "PNG");
    }
    if(image.isNull())
    {
        it->state = 3;
        emit status(QString::fromUtf8("Ошибка загрузки тайла: %1").arg(reply->errorString()));
    }else
    {
        QDir().mkpath(QFileInfo(tilePath(it->x, it->y)).path());
        QFile f(tilePath(it->x, it->y));
        if(f.open(QIODevice::WriteOnly))
            f.write(data);
        buildTile(*it, image);
    }
    pump();
}

void TerrainStreamer::buildTile(TTile &tile, const QImage &src)
{
    const QImage image = src.convertToFormat(QImage::Format_RGB32);
    const int cells = TILE_PX / step;
    const int g = cells + 1;
    const double cosLat0 = cos(lat0 * M_PI / 180.0);

    // высоты и координаты узлов
    QVector<QVector3D> pos(g * g);
    for(int i = 0; i < g; i++)
    {
        const double py = qMin(i * step, TILE_PX - 1) + 0.5;
        const double lat = tileToLat(tile.y + double(i * step) / TILE_PX, zoom);
        const double x = (lat - lat0) * M_PI / 180.0 * EARTH_R;
        for(int j = 0; j < g; j++)
        {
            const double px = qMin(j * step, TILE_PX - 1) + 0.5;
            const double lon = tileToLon(tile.x + double(j * step) / TILE_PX, zoom);
            // разность долгот с учетом перехода через 180 град
            double dLon = lon - lon0;
            if(dLon > 180.0)  dLon -= 360.0;
            if(dLon < -180.0) dLon += 360.0;
            const double z = dLon * M_PI / 180.0 * EARTH_R * cosLat0;
            const QRgb c = image.pixel(int(px), int(py));
            // формат Terrarium: высота = R*256 + G + B/256 - 32768
            double h = qRed(c) * 256.0 + qGreen(c) + qBlue(c) / 256.0 - 32768.0;
            if(h < 0.0)
                h = 0.0;
            pos[i * g + j] = QVector3D(x, h, z);
        }
    }
    tile.water = tile.count = 0;
    tile.gridSize = g;
    tile.vertices.resize(g * g * 9);
    for(int i = 0; i < g; i++)
    {
        for(int j = 0; j < g; j++)
        {
            const QVector3D &p = pos[i * g + j];
            const QVector3D north = pos[qMax(i - 1, 0) * g + j] - pos[qMin(i + 1, g - 1) * g + j];
            const QVector3D east  = pos[i * g + qMin(j + 1, g - 1)] - pos[i * g + qMax(j - 1, 0)];
            QVector3D n = QVector3D::crossProduct(east, north).normalized();
            if(n.y() < 0.0f)
                n = -n;
            float c[3];
            terrainColor(p.y(), n.y(), c);
            float *v = &tile.vertices[(i * g + j) * 9];
            v[0] = p.x(); v[1] = p.y(); v[2] = p.z();
            v[3] = n.x(); v[4] = n.y(); v[5] = n.z();
            v[6] = c[0];  v[7] = c[1];  v[8] = c[2];
            if(p.y() < 3.0f)
                tile.water++;
            tile.count++;
            if(minH == 0.0f || p.y() < minH)
                minH = p.y();
        }
    }
    tile.state = 2;
}

void TerrainStreamer::compileTile(TTile &tile)
{
    const int g = tile.gridSize;
    const float *v = tile.vertices.constData();
    auto vertex = [&](int i, int j, float drop)
    {
        const float *p = v + (i * g + j) * 9;
        glColor3fv(p + 6);
        glNormal3fv(p + 3);
        glVertex3f(p[0], p[1] - drop, p[2]);
    };
    tile.list = glGenLists(1);
    glNewList(tile.list, GL_COMPILE);
    for(int i = 0; i < g - 1; i++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for(int j = 0; j < g; j++)
        {
            vertex(i, j, 0.0f);
            vertex(i + 1, j, 0.0f);
        }
        glEnd();
    }
    // "юбки" по четырем краям тайла закрывают щели на стыках
    glBegin(GL_QUAD_STRIP);
    for(int j = 0; j < g; j++) {vertex(0, j, 0.0f); vertex(0, j, SKIRT_DEPTH);}
    glEnd();
    glBegin(GL_QUAD_STRIP);
    for(int j = 0; j < g; j++) {vertex(g - 1, j, 0.0f); vertex(g - 1, j, SKIRT_DEPTH);}
    glEnd();
    glBegin(GL_QUAD_STRIP);
    for(int i = 0; i < g; i++) {vertex(i, 0, 0.0f); vertex(i, 0, SKIRT_DEPTH);}
    glEnd();
    glBegin(GL_QUAD_STRIP);
    for(int i = 0; i < g; i++) {vertex(i, g - 1, 0.0f); vertex(i, g - 1, SKIRT_DEPTH);}
    glEnd();
    glEndList();
    // данные вершин больше не нужны - геометрия в списке отображения
    tile.vertices.clear();
    tile.vertices.squeeze();
}

void TerrainStreamer::draw()
{
    for(GLuint list : deadLists)
        glDeleteLists(list, 1);
    deadLists.clear();

    int compiled = 0;
    for(auto it = tiles.begin(); it != tiles.end(); ++it)
    {
        if(it->state != 2)
            continue;
        if(it->list == 0)
        {
            if(compiled >= MAX_COMPILE_PER_FRAME)
                continue;
            compileTile(*it);
            compiled++;
        }
        glCallList(it->list);
    }
}
