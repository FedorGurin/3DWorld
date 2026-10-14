#include "terrainStreamer.h"
#include "terrainColor.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPainter>
#include <QStandardPaths>
#include <QVector3D>
#include <math.h>
#include <algorithm>

#ifndef GL_GENERATE_MIPMAP
#define GL_GENERATE_MIPMAP 0x8191
#endif
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

//! радиус Земли, м
static const double EARTH_R = 6371000.0;
//! размер тайла, пиксели
static const int TILE_PX = 256;
//! одновременных запросов
static const int MAX_IN_FLIGHT = 8;
//! операций с OpenGL (компиляция блока, загрузка текстуры) за один кадр
static const int MAX_COMPILE_PER_FRAME = 6;
//! глубина "юбки" по краям блока, закрывающей щели между тайлами, м
static const float SKIRT_DEPTH = 300.0f;
//! разница уровней детализации ближнего и дальнего слоев (4x4 тайла в одном)
static const int LEVEL_ZOOM_STEP = 2;

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
    urlTemplate = "https://s3.amazonaws.com/elevation-tiles-prod/terrarium/{z}/{x}/{y}.png";
    // Sentinel-2 cloudless 2016 (EOX IT Services GmbH, лицензия CC BY 4.0);
    // другой источник можно задать переменной окружения TERRAIN_IMAGERY_URL
    imageryUrl = "https://tiles.maps.eox.at/wmts/1.0.0/s2cloudless_3857/default/g/{z}/{y}/{x}.jpg";
    imageryAttribution = "Sentinel-2 cloudless - https://s2maps.eu by EOX IT Services GmbH "
                         "(Contains modified Copernicus Sentinel data 2016 & 2017)";
    const QByteArray envUrl = qgetenv("TERRAIN_IMAGERY_URL");
    if(envUrl.isEmpty() == false)
    {
        imageryUrl = QString::fromUtf8(envUrl);
        imageryAttribution = QString::fromUtf8(qgetenv("TERRAIN_IMAGERY_ATTRIBUTION"));
    }
    cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/terrain";
    manager  = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
    inFlight = 0;
    lat0 = 43.35;
    lon0 = 42.44;
    minH = 0.0f;
    imageryEnabled = true;

    // ближний слой: ~14 км на тайл в средних широтах, 9x9 тайлов;
    // дальний слой: в 4 раза крупнее, 7x7 тайлов (около +-200 км)
    TLevel nearLevel;
    nearLevel.zoom   = 11;
    nearLevel.radius = 4;
    nearLevel.step   = 4;
    nearLevel.blocks = 1;
    nearLevel.centerX = nearLevel.centerY = -1;
    TLevel farLevel = nearLevel;
    farLevel.zoom   = nearLevel.zoom - LEVEL_ZOOM_STEP;
    farLevel.radius = 3;
    farLevel.step   = 4;
    farLevel.blocks = 1 << LEVEL_ZOOM_STEP;
    levels << nearLevel << farLevel;
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

void TerrainStreamer::setImagery(bool value)
{
    if(imageryEnabled == value)
        return;
    imageryEnabled = value;
    // при включении запрашиваем снимки для уже загруженных тайлов
    if(value)
    {
        for(int l = 0; l < levels.size(); l++)
            levels[l].centerX = levels[l].centerY = -1;
    }
}

void TerrainStreamer::releaseTile(TTile &tile)
{
    for(GLuint list : tile.listColor)
        if(list != 0) deadLists.append(list);
    for(GLuint list : tile.listTex)
        if(list != 0) deadLists.append(list);
    tile.listColor.clear();
    tile.listTex.clear();
    if(tile.texture != 0)
        deadTextures.append(tile.texture);
    tile.texture = 0;
}

void TerrainStreamer::clear()
{
    for(int l = 0; l < levels.size(); l++)
    {
        for(auto it = levels[l].tiles.begin(); it != levels[l].tiles.end(); ++it)
            releaseTile(*it);
        levels[l].tiles.clear();
        levels[l].centerX = levels[l].centerY = -1;
    }
    heightQueue.clear();
    imageQueue.clear();
    minH = 0.0f;
}

QString TerrainStreamer::tilePath(int zoom, int x, int y) const
{
    return QString("%1/%2/%3/%4.png").arg(cacheDir).arg(zoom).arg(x).arg(y);
}

QString TerrainStreamer::imagePath(int zoom, int x, int y) const
{
    // кэш снимков разделен по источнику
    const QString src = QString::number(qHash(imageryUrl), 16);
    return QString("%1/imagery_%2/%3/%4/%5.img").arg(cacheDir, src).arg(zoom).arg(x).arg(y);
}

QString TerrainStreamer::makeUrl(const QString &tmpl, int zoom, int x, int y) const
{
    QString url = tmpl;
    url.replace("{z}", QString::number(zoom));
    url.replace("{x}", QString::number(x));
    url.replace("{y}", QString::number(y));
    return url;
}

int TerrainStreamer::readyCount() const
{
    int n = 0;
    for(const TLevel &level : levels)
        for(const TTile &t : level.tiles)
            if(t.state == 2)
                n++;
    return n;
}

float TerrainStreamer::waterFraction() const
{
    // по дальнему слою - он покрывает всю область вокруг
    long water = 0, count = 0;
    for(const TTile &t : levels.last().tiles)
    {
        water += t.water;
        count += t.count;
    }
    return count > 0 ? float(water) / count : 0.0f;
}

bool TerrainStreamer::hasImagery() const
{
    if(imageryEnabled == false)
        return false;
    for(const TLevel &level : levels)
        for(const TTile &t : level.tiles)
            if(t.texture != 0)
                return true;
    return false;
}

void TerrainStreamer::update(double focusX, double focusZ)
{
    // точка фокуса в географических координатах (касательная плоскость в начале координат)
    const double lat = lat0 + focusX / EARTH_R * 180.0 / M_PI;
    const double lon = lon0 + focusZ / (EARTH_R * cos(lat0 * M_PI / 180.0)) * 180.0 / M_PI;
    for(int l = 0; l < levels.size(); l++)
        updateLevel(l, qBound(-84.0, lat, 84.0), lon);
    pump();
}

void TerrainStreamer::updateLevel(int levelIndex, double lat, double lon)
{
    TLevel &level = levels[levelIndex];
    const int cx = int(floor(lonToTile(lon, level.zoom)));
    const int cy = int(floor(latToTile(lat, level.zoom)));
    if(cx == level.centerX && cy == level.centerY)
        return;
    level.centerX = cx;
    level.centerY = cy;

    const int n = 1 << level.zoom;
    // выгружаем тайлы, ушедшие далеко от точки
    for(auto it = level.tiles.begin(); it != level.tiles.end();)
    {
        int dx = abs(it->x - cx);
        dx = qMin(dx, n - dx);
        if(dx > level.radius + 1 || abs(it->y - cy) > level.radius + 1)
        {
            releaseTile(*it);
            it = level.tiles.erase(it);
        }else
            ++it;
    }
    // убираем из очередей запросы к выгруженным тайлам
    auto gone = [&](const TRequest &r) {return r.level == levelIndex && level.tiles.contains(r.key) == false;};
    heightQueue.erase(std::remove_if(heightQueue.begin(), heightQueue.end(), gone), heightQueue.end());
    imageQueue.erase(std::remove_if(imageQueue.begin(), imageQueue.end(), gone), imageQueue.end());

    // недостающие тайлы, ближние - первыми
    QList<QPair<int, qint64> > missing;
    for(int dy = -level.radius; dy <= level.radius; dy++)
    {
        for(int dx = -level.radius; dx <= level.radius; dx++)
        {
            const int tx = ((cx + dx) % n + n) % n;
            const int ty = cy + dy;
            if(ty < 0 || ty >= n)
                continue;
            const qint64 k = key(tx, ty);
            auto it = level.tiles.find(k);
            if(it == level.tiles.end())
            {
                TTile t;
                t.level = levelIndex;
                t.x = tx; t.y = ty;
                t.state = 0;
                t.g = 0;
                t.imagePending = -1;
                t.imageFailed  = false;
                t.texture = 0;
                t.water = t.count = 0;
                it = level.tiles.insert(k, t);
                missing.append(qMakePair(dx * dx + dy * dy, k));
            }else if(imageryEnabled && it->imagePending < 0 && it->texture == 0 && it->imageFailed == false)
                missing.append(qMakePair(dx * dx + dy * dy, k));
        }
    }
    std::sort(missing.begin(), missing.end(),
              [](const QPair<int, qint64> &a, const QPair<int, qint64> &b) {return a.first < b.first;});
    QList<TRequest> heights, images;
    for(const auto &m : missing)
    {
        TTile &t = level.tiles[m.second];
        if(t.state == 0)
            heights.append({levelIndex, m.second, -1});
        if(imageryEnabled && t.imagePending < 0 && t.texture == 0)
        {
            t.imagePending = 4;
            t.image = QImage(2 * TILE_PX, 2 * TILE_PX, QImage::Format_RGB32);
            t.image.fill(Qt::black);
            for(int p = 0; p < 4; p++)
                images.append({levelIndex, m.second, p});
        }
    }
    // ближний слой - в начало очереди: он важнее дальнего
    if(levelIndex == 0)
    {
        heightQueue = heights + heightQueue;
        imageQueue  = images + imageQueue;
    }else
    {
        heightQueue += heights;
        imageQueue  += images;
    }
}

void TerrainStreamer::pump()
{
    // сначала высоты (без них нечего рисовать), затем снимки
    while(inFlight < MAX_IN_FLIGHT && (heightQueue.isEmpty() == false || imageQueue.isEmpty() == false))
    {
        TRequest r = heightQueue.isEmpty() == false ? heightQueue.takeFirst() : imageQueue.takeFirst();
        startRequest(r);
    }
    const int pending = pendingCount();
    if(pending > 0)
        emit status(QString::fromUtf8("Подгрузка рельефа: %1 запросов").arg(pending));
    else
        emit status(QString::fromUtf8("Рельеф загружен: %1 тайлов").arg(readyCount()));
}

void TerrainStreamer::startRequest(const TRequest &r)
{
    TLevel &level = levels[r.level];
    auto it = level.tiles.find(r.key);
    if(it == level.tiles.end())
        return;
    TTile &tile = *it;
    int zoom = level.zoom, x = tile.x, y = tile.y;
    QString path, url;
    if(r.part < 0)
    {
        if(tile.state != 0)
            return;
        path = tilePath(zoom, x, y);
        url  = makeUrl(urlTemplate, zoom, x, y);
    }else
    {
        // снимок в 2 раза подробнее: 4 тайла следующего уровня
        zoom++;
        x = 2 * x + (r.part & 1);
        y = 2 * y + (r.part >> 1);
        path = imagePath(zoom, x, y);
        url  = makeUrl(imageryUrl, zoom, x, y);
    }
    // из кэша
    QImage cached;
    if(QFileInfo::exists(path) && cached.load(path))
    {
        if(r.part < 0)
            buildTile(tile, cached);
        else
            imagePart(tile, r.part, cached);
        return;
    }
    QNetworkRequest request{QUrl(url)};
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    request.setRawHeader("User-Agent", "3DWorld terrain streamer");
    QNetworkReply *reply = manager->get(request);
    reply->setProperty("level", r.level);
    reply->setProperty("key", r.key);
    reply->setProperty("part", r.part);
    reply->setProperty("path", path);
    reply->setProperty("origin", QString("%1,%2").arg(lat0, 0, 'f', 9).arg(lon0, 0, 'f', 9));
    if(r.part < 0)
        tile.state = 1;
    inFlight++;
}

void TerrainStreamer::replyFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    inFlight--;
    const int levelIndex = reply->property("level").toInt();
    const qint64 k = reply->property("key").toLongLong();
    const int part = reply->property("part").toInt();
    const QString origin = QString("%1,%2").arg(lat0, 0, 'f', 9).arg(lon0, 0, 'f', 9);
    auto it = levels[levelIndex].tiles.find(k);
    // тайл уже выгружен или сменилось начало координат
    if(it == levels[levelIndex].tiles.end() || reply->property("origin").toString() != origin)
    {
        pump();
        return;
    }
    QImage image;
    QByteArray data;
    if(reply->error() == QNetworkReply::NoError)
    {
        data = reply->readAll();
        image.loadFromData(data);
    }
    if(image.isNull() == false)
    {
        const QString path = reply->property("path").toString();
        QDir().mkpath(QFileInfo(path).path());
        QFile f(path);
        if(f.open(QIODevice::WriteOnly))
            f.write(data);
    }
    if(part < 0)
    {
        if(image.isNull())
        {
            it->state = 3;
            emit status(QString::fromUtf8("Ошибка загрузки тайла высот: %1").arg(reply->errorString()));
        }else
            buildTile(*it, image);
    }else
    {
        if(image.isNull())
            emit status(QString::fromUtf8("Ошибка загрузки снимка: %1").arg(reply->errorString()));
        imagePart(*it, part, image);
    }
    pump();
}

void TerrainStreamer::imagePart(TTile &tile, int part, const QImage &image)
{
    if(tile.imagePending <= 0)
        return;
    if(image.isNull())
        tile.imageFailed = true;
    else
    {
        QPainter painter(&tile.image);
        painter.drawImage(QRect((part & 1) * TILE_PX, (part >> 1) * TILE_PX, TILE_PX, TILE_PX), image);
    }
    tile.imagePending--;
    if(tile.imagePending == 0 && tile.imageFailed)
        tile.image = QImage();
}

void TerrainStreamer::buildTile(TTile &tile, const QImage &src)
{
    const TLevel &level = levels[tile.level];
    const QImage image = src.convertToFormat(QImage::Format_RGB32);
    const int cells = TILE_PX / level.step;
    const int g = cells + 1;
    const double cosLat0 = cos(lat0 * M_PI / 180.0);

    // высоты и координаты узлов
    QVector<QVector3D> pos(g * g);
    for(int i = 0; i < g; i++)
    {
        const int py = qMin(i * level.step, TILE_PX - 1);
        const double lat = tileToLat(tile.y + double(i * level.step) / TILE_PX, level.zoom);
        const double x = (lat - lat0) * M_PI / 180.0 * EARTH_R;
        for(int j = 0; j < g; j++)
        {
            const int px = qMin(j * level.step, TILE_PX - 1);
            const double lon = tileToLon(tile.x + double(j * level.step) / TILE_PX, level.zoom);
            // разность долгот с учетом перехода через 180 град
            double dLon = lon - lon0;
            if(dLon > 180.0)  dLon -= 360.0;
            if(dLon < -180.0) dLon += 360.0;
            const double z = dLon * M_PI / 180.0 * EARTH_R * cosLat0;
            const QRgb c = image.pixel(px, py);
            // формат Terrarium: высота = R*256 + G + B/256 - 32768
            double h = qRed(c) * 256.0 + qGreen(c) + qBlue(c) / 256.0 - 32768.0;
            if(h < 0.0)
                h = 0.0;
            pos[i * g + j] = QVector3D(x, h, z);
        }
    }
    tile.water = tile.count = 0;
    tile.g = g;
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

GLuint TerrainStreamer::compileRegion(const TTile &tile, int i0, int i1, int j0, int j1, bool textured)
{
    const int g = tile.g;
    const float *v = tile.vertices.constData();
    const float inv = 1.0f / float(g - 1);
    auto vertex = [&](int i, int j, float drop)
    {
        const float *p = v + (i * g + j) * 9;
        if(textured)
        {
            // снимок модулируется освещением - цвет вершины белый
            glColor3f(1.0f, 1.0f, 1.0f);
            glTexCoord2f(j * inv, i * inv);
        }else
            glColor3fv(p + 6);
        glNormal3fv(p + 3);
        glVertex3f(p[0], p[1] - drop, p[2]);
    };
    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    for(int i = i0; i < i1; i++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for(int j = j0; j <= j1; j++)
        {
            vertex(i, j, 0.0f);
            vertex(i + 1, j, 0.0f);
        }
        glEnd();
    }
    // "юбки" по четырем краям блока закрывают щели на стыках
    glBegin(GL_QUAD_STRIP);
    for(int j = j0; j <= j1; j++) {vertex(i0, j, 0.0f); vertex(i0, j, SKIRT_DEPTH);}
    glEnd();
    glBegin(GL_QUAD_STRIP);
    for(int j = j0; j <= j1; j++) {vertex(i1, j, 0.0f); vertex(i1, j, SKIRT_DEPTH);}
    glEnd();
    glBegin(GL_QUAD_STRIP);
    for(int i = i0; i <= i1; i++) {vertex(i, j0, 0.0f); vertex(i, j0, SKIRT_DEPTH);}
    glEnd();
    glBegin(GL_QUAD_STRIP);
    for(int i = i0; i <= i1; i++) {vertex(i, j1, 0.0f); vertex(i, j1, SKIRT_DEPTH);}
    glEnd();
    glEndList();
    return list;
}

void TerrainStreamer::compileTile(TTile &tile, bool textured)
{
    const int blocks = levels[tile.level].blocks;
    const int cellsPerBlock = (tile.g - 1) / blocks;
    QVector<GLuint> &lists = textured ? tile.listTex : tile.listColor;
    lists.clear();
    for(int bi = 0; bi < blocks; bi++)
        for(int bj = 0; bj < blocks; bj++)
            lists.append(compileRegion(tile,
                                       bi * cellsPerBlock, (bi + 1) * cellsPerBlock,
                                       bj * cellsPerBlock, (bj + 1) * cellsPerBlock,
                                       textured));
}

void TerrainStreamer::drawTile(TTile &tile, const QSet<qint64> *covered, int &compileBudget)
{
    if(tile.state != 2)
        return;
    if(tile.listColor.isEmpty())
    {
        if(compileBudget <= 0)
            return;
        compileTile(tile, false);
        compileBudget--;
    }
    // снимок загружен - создаем текстуру и вариант геометрии с текстурными координатами
    if(imageryEnabled && tile.texture == 0 && tile.imagePending == 0 &&
       tile.imageFailed == false && tile.image.isNull() == false && compileBudget > 0)
    {
        const QImage img = tile.image.convertToFormat(QImage::Format_RGBA8888);
        glGenTextures(1, &tile.texture);
        glBindTexture(GL_TEXTURE_2D, tile.texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, img.constBits());
        glBindTexture(GL_TEXTURE_2D, 0);
        tile.image = QImage();
        compileTile(tile, true);
        compileBudget--;
    }
    const bool textured = imageryEnabled && tile.texture != 0 && tile.listTex.isEmpty() == false;
    if(textured)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tile.texture);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    const QVector<GLuint> &lists = textured ? tile.listTex : tile.listColor;
    const int blocks = levels[tile.level].blocks;
    for(int bi = 0; bi < blocks; bi++)
    {
        for(int bj = 0; bj < blocks; bj++)
        {
            // блок закрыт уже нарисованным тайлом ближнего слоя
            if(covered != nullptr &&
               covered->contains(key(tile.x * blocks + bj, tile.y * blocks + bi)))
                continue;
            glCallList(lists[bi * blocks + bj]);
        }
    }
    if(textured)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}

void TerrainStreamer::draw()
{
    for(GLuint list : deadLists)
        glDeleteLists(list, 1);
    deadLists.clear();
    if(deadTextures.isEmpty() == false)
        glDeleteTextures(deadTextures.size(), deadTextures.constData());
    deadTextures.clear();

    int budget = MAX_COMPILE_PER_FRAME;
    // ближний слой
    QSet<qint64> drawnNear;
    for(auto it = levels[0].tiles.begin(); it != levels[0].tiles.end(); ++it)
    {
        drawTile(*it, nullptr, budget);
        if(it->state == 2 && it->listColor.isEmpty() == false)
            drawnNear.insert(it.key());
    }
    // дальний слой - только там, где нет ближних тайлов
    for(auto it = levels[1].tiles.begin(); it != levels[1].tiles.end(); ++it)
        drawTile(*it, &drawnNear, budget);
}
