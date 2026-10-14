#ifndef TERRAINSTREAMER_H
#define TERRAINSTREAMER_H

#include <QObject>
#include <QHash>
#include <QList>
#include <QString>
#include <QVector>
#include <QtGui/qopengl.h>

class QNetworkAccessManager;
class QNetworkReply;
class QImage;

//! подгрузка рельефа вокруг движущейся точки (камеры/объекта):
//! сцена не ограничена размерами одной карты - тайлы высот загружаются
//! из интернета (AWS Terrain Tiles) по мере полета и выгружаются позади
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

    //! уровень детализации тайлов
    int zoom;
    //! радиус области вокруг точки, в тайлах
    int radius;
    //! шаг сетки внутри тайла, пиксели (256 / step ячеек на тайл)
    int step;
    //! адрес тайлов: {z}, {x}, {y} заменяются номерами тайла
    QString urlTemplate;
    //! каталог кэша тайлов
    QString cacheDir;

    //! минимальная высота загруженных тайлов, м
    float minHeight() const {return minH;}
    //! доля воды среди загруженных точек
    float waterFraction() const;
    //! кол-во загруженных и ожидаемых тайлов
    int readyCount() const;
    int pendingCount() const {return inFlight + queue.size();}
signals:
    //! состояние подгрузки
    void status(const QString &text);
private slots:
    void replyFinished(QNetworkReply *reply);
private:
    struct TTile
    {
        int x, y;
        //! 0 - в очереди, 1 - загружается, 2 - готов (данные), 3 - ошибка
        int state;
        //! вершины: позиция, нормаль, цвет (9 float на вершину)
        QVector<float> vertices;
        int gridSize;
        GLuint list;
        int water, count;
    };
    static qint64 key(int x, int y) {return (qint64(x) << 32) | qint64(quint32(y));}
    QString tilePath(int x, int y) const;
    //! запуск загрузки следующих тайлов из очереди
    void pump();
    //! построение сетки тайла из изображения высот
    void buildTile(TTile &tile, const QImage &image);
    //! компиляция списка отображения тайла
    void compileTile(TTile &tile);

    //! освобождение списка отображения (откладывается до отрисовки)
    void releaseList(GLuint list);

    QNetworkAccessManager *manager;
    QHash<qint64, TTile> tiles;
    //! списки отображения, которые нужно удалить при наличии контекста OpenGL
    QVector<GLuint> deadLists;
    QList<qint64> queue;
    int inFlight;
    double lat0, lon0;
    float minH;
    //! тайл, в котором находится точка фокуса
    int centerX, centerY;
};

#endif // TERRAINSTREAMER_H
