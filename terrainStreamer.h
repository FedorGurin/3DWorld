#ifndef TERRAINSTREAMER_H
#define TERRAINSTREAMER_H

#include <QObject>
#include <QHash>
#include <QImage>
#include <QList>
#include <QSet>
#include <QString>
#include <QVector>
#include <QtGui/qopengl.h>

class QNetworkAccessManager;
class QNetworkReply;

//! подгрузка рельефа вокруг движущейся точки (камеры/объекта):
//! сцена не ограничена размерами одной карты - тайлы высот (AWS Terrain
//! Tiles) и спутниковых снимков загружаются по мере полета и выгружаются
//! позади. Два уровня детализации: подробные тайлы рядом и крупные вдали
class TerrainStreamer : public QObject
{
    Q_OBJECT
public:
    explicit TerrainStreamer(QObject *parent = nullptr);
    ~TerrainStreamer();

    //! географическая точка начала координат сцены, град
    void setOrigin(double lat, double lon);
    double originLat() const {return lat0;}
    double originLon() const {return lon0;}
    //! обновление набора тайлов вокруг точки (x - на север, z - на восток, м)
    void update(double focusX, double focusZ);
    //! отрисовка загруженных тайлов (вызывается с контекстом OpenGL)
    void draw();
    //! удаление всех тайлов
    void clear();
    //! наложение спутниковых снимков
    void setImagery(bool value);
    bool imagery() const {return imageryEnabled;}

    //! адрес тайлов высот и снимков: {z}, {x}, {y} заменяются номерами тайла
    QString urlTemplate;
    QString imageryUrl;
    //! подпись источника снимков (выводится на экран)
    QString imageryAttribution;
    //! каталог кэша тайлов
    QString cacheDir;

    //! минимальная высота загруженных тайлов, м
    float minHeight() const {return minH;}
    //! доля воды среди загруженных точек дальнего уровня
    float waterFraction() const;
    //! кол-во загруженных и ожидаемых тайлов
    int readyCount() const;
    int pendingCount() const {return inFlight + heightQueue.size() + imageQueue.size();}
    //! есть ли на экране тайлы со снимками
    bool hasImagery() const;
signals:
    //! состояние подгрузки
    void status(const QString &text);
private slots:
    void replyFinished(QNetworkReply *reply);
private:
    struct TTile
    {
        int level, x, y;
        //! высоты: 0 - в очереди, 1 - загружается, 2 - готов, 3 - ошибка
        int state;
        //! вершины: позиция, нормаль, цвет (9 float на вершину), сетка g x g
        QVector<float> vertices;
        int g;
        //! списки отображения: окрашенный по высоте и со снимком
        //! (ближний уровень - один блок, дальний - 4x4 блока)
        QVector<GLuint> listColor;
        QVector<GLuint> listTex;
        //! снимок: осталось загрузить частей (-1 - не запрашивался), изображение, текстура
        int imagePending;
        bool imageFailed;
        QImage image;
        GLuint texture;
        int water, count;
    };
    struct TLevel
    {
        int zoom;
        int radius;
        int step;
        //! блоков на сторону тайла (совпадают с тайлами предыдущего уровня)
        int blocks;
        QHash<qint64, TTile> tiles;
        int centerX, centerY;
    };
    struct TRequest
    {
        int level;
        qint64 key;
        //! -1 - высоты, 0..3 - часть снимка
        int part;
    };
    static qint64 key(int x, int y) {return (qint64(x) << 32) | qint64(quint32(y));}
    QString tilePath(int zoom, int x, int y) const;
    QString imagePath(int zoom, int x, int y) const;
    QString makeUrl(const QString &tmpl, int zoom, int x, int y) const;
    //! обновление набора тайлов одного уровня
    void updateLevel(int levelIndex, double lat, double lon);
    //! запуск загрузки следующих тайлов из очереди
    void pump();
    void startRequest(const TRequest &r);
    //! построение сетки тайла из изображения высот
    void buildTile(TTile &tile, const QImage &image);
    //! часть снимка готова
    void imagePart(TTile &tile, int part, const QImage &image);
    //! компиляция блока сетки [i0;i1] x [j0;j1]
    GLuint compileRegion(const TTile &tile, int i0, int i1, int j0, int j1, bool textured);
    void compileTile(TTile &tile, bool textured);
    //! освобождение ресурсов OpenGL (откладывается до отрисовки)
    void releaseTile(TTile &tile);
    //! отрисовка тайла (блоки, закрытые тайлами ближнего уровня, пропускаются)
    void drawTile(TTile &tile, const QSet<qint64> *covered, int &compileBudget);

    QNetworkAccessManager *manager;
    QVector<TLevel> levels;
    QList<TRequest> heightQueue;
    QList<TRequest> imageQueue;
    QVector<GLuint> deadLists;
    QVector<GLuint> deadTextures;
    int inFlight;
    double lat0, lon0;
    float minH;
    bool imageryEnabled;
};

#endif // TERRAINSTREAMER_H
