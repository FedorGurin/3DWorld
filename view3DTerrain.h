#ifndef VIEW3DTERRAIN_H
#define VIEW3DTERRAIN_H

#include <QGLWidget>
#include <QFileDialog>
#include <QVector>
#include <QThread>
#include <QVector3D>



#include <QString>
#include <QTextStream>


#include "math_func.h"
#include "terrainDownloader.h"
typedef struct
{
    long ncols;
    long nrows;

    //! долгота и широта левого нижнего угла карты, град
    double xllcorner;
    double yllcorner;

    double cellsize;//шаг сетки в градусах
    //! высоты, м: data[строка][столбец], строка 0 - северный край карты
    QVector<QVector<double> >data;
    //! координаты узлов в земной СК, м: x - на север, y - вверх, z - на восток
    QVector<QVector<QVector3D> > dataVectors;
    //! нормали в узлах сетки
    QVector<QVector<QVector3D> > normals;
    //! минимальная и максимальная высота, м
    double minHeight;
    double maxHeight;
    QVector3D value(int col,int row)
    {
        return dataVectors[row][col];
    }
    QVector3D normal(int col,int row)
    {
        return normals[row][col];
    }
}TASCII_MAP;

class ThreadParser : public QThread
 {
 public:
    ThreadParser();
    void setParserFile(QTextStream *out,TASCII_MAP *mapH_);
    void run();
 private:

    TASCII_MAP *mapH;
    QTextStream *out;
 };

class View3DTerrain : public QGLWidget
{
    Q_OBJECT;
public:
    View3DTerrain(QWidget *parent=0);
    //! открыть файл с катрой цветов
    void openFileWithMapColor();

    //! открыть ранее сохраненную карту
    bool openTerrainMap(QString nameFile);
    //! процесс загрузки карты высот в реальном времени
    void processLoading();
    bool loadCompleate;
    //! номер загруженной карты (увеличивается после каждой загрузки)
    int version;
    ~View3DTerrain();
    TASCII_MAP *mapH;
    //! загрузка карты высот из интернета
    TerrainDownloader *downloader;
private:

    QFile *file;
    QTextStream *stream;
    QFileDialog *dialog;
    ThreadParser *threadParser;
    //! карта, которую нужно загрузить после окончания текущей загрузки
    QString pendingFile;
    //! загрузка карты из файла
    void loadFile(const QString &fileName);

public slots:
    void slotAccepted();
    //! загрузка потока
    void slotThreadFinished();
    //! открыть файл с ландшафтом
    void openFileWithTerrain();
    //! загрузить из интернета область с центром lat, lon (град) и стороной sizeKm
    void loadFromInternet(double lat, double lon, double sizeKm);
    //! карта из интернета сохранена в файл
    void slotDownloadFinished(const QString &fileName);
signals:
    //! состояние загрузки рельефа (для окна настроек)
    void status(const QString &text);
private slots:
    void slotDownloadProgress(int done, int total);
    void slotDownloadFailed(const QString &message);
};

#endif // VIEW3DTERRAIN_H
