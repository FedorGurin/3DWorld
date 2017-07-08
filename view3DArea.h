#ifndef VIEW3DAREA_H
#define VIEW3DAREA_H

#include <QGLViewer/qglviewer.h>


#include <lib3ds/lib3ds.h>

//#include <lib3ds/node.h>

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QLocale>
#include <math.h>

#include "view3dTerrain.h"

#include "math_func.h"

#include "visudp.h"
#include "viewILS.h"
#include "formsettings.h"
#include "TemplateNASP.hpp"
//! струтура описывающая траекторию
typedef struct _TTrajectory
{
    //! массив с координатами
    QVector<float> x;
    QVector<float> y;
    QVector<float> z;
    //! цвет траектории
    QVector<int>    r;
    QVector<int>    g;
    QVector<int>    b;

}TTrajectory;

//! структура описывающая строку с данными
typedef struct _TDataRow
{
    //! идентификатор объекта
    int id_obj;
    //! в зависимости от кода будет отображаться 3D модель
    int code;
    //! масштаб 3D объекта, б/р
    float scale;
    //! время, сек
    double time;
    //! цвет
    int r;
    int g;
    int b;
    //! координаты объекта, метры
    float x_g;
    float y_g;
    float z_g;
    //! углы ориентации объекта, град
    float psi;
    float gamma;
    float tan;
}TDataRow;

typedef struct
{
    //! код объекта
    int code;
    //! загрузка файла с объектом
    Lib3dsFile *file;
}T3DObject;
//! класс 3D визуализации
class view3DArea:public QGLViewer
{
    Q_OBJECT;
public:
    //! 3D просмотр области визуализации
    view3DArea();
    //! установка номера камеры
    void setCameraToObject(int index);
    //! привязать камеру к объекту
    void cameraToObject();
    //! Загрузить файл с 3D моделью
    void loadFile(QString nameFile,Lib3dsFile **file3ds);
    //! чтение всех моделей
    void readAllModels();
    //! поиск объекта по коду
    T3DObject* findObjByCode(int code);
    //! инициализация сцены
    void initScene(Lib3dsFile *file3ds);
    //! отрисовка объектов
    void drawSolidObjects();
    //! отрисовка объекта
    void drawObject(Lib3dsFile *obj,
                    double pos_x,
                    double pos_y,
                    double pos_z,
                    double psi_grad,
                    double gamma_grad,
                    double tan_grad);
    //! перевод градусов в пиксели
    void gradToPixel(double xGrad,
                     double yGrad,
                     int &xPixel,
                     int &yPixel);
    //! отрисовка рельефа
    void drawTerra();
    //! отрисовка неба
    void drawSky();
    //! отрисовка сетки
    void drawILS();
    //! отрисовка прицельной сиволики
    void drawSymbol();
    //! отрисовка окружности
    void drawCircle(TAngle *angle,int radius_,int width_);
    //! отрисовка перекрестья
    void drawCross(TAngle* angle,int radius_,int width_);
    //! отрисовка траектории за объектом
    void drawTrajectory();
    //! отрисовка строки состояния
    void drawStateLine();
    //! разбор текстового файла
    void parserTXTFile();
    //! поиск времени на интервале
    int searchTimeInterval(double time);
    //! расчет результирующей матрицы поворота
    glm::mat3 signleCalcMatrix(TSolidObj *solid);
    glm::mat3 signleCalcMatrixPsi(TSolidObj *solid);
    //! координаты камеры в СК цели
    glm::vec3 vecCameraInTargetSystem;
    glm::vec3 vecCameraInGeoSys;
    //! размер сцены, метры
    double radiusScene;
    //! ограничение на КАИ
    LimitPositionKAI *limit;
    //! отрисовка узлов
    void renderNode(Lib3dsFile *,Lib3dsNode *node);
protected :
    virtual void draw();
    virtual void animate();
    virtual void init();
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
private:
    //! управление ручкой
    TControlStick stick;
    //! рабочие параметры
    TSolidObj sW;
    //! приращения к повороту камеры
    double dpsi_camera;
    double dteta_camera;
    double dgamma_camera;
    //! множитель времени, б/р
    float multTime;
    //! густота тумана,[0.0;1.0]
    float dFog;
    //! глобальное время, сек
    double globalTime;
    double globalTimeDebug;
    //! период анимации, Гц
    int dt;//мсек
    //! глобальный счетчик
    int globalCount;
    //! поворот карты
    double rotMap;
    double offsetMapX;
    double offsetMapZ;
    //! для чтения TXT файла с данными
    QFile *file;
    QTextStream *stream;
    QFileDialog *dialog;
    //! содержимое текстового файла(список строк)
    QVector<TDataRow> rows;
    //! текущий объект на котором установлена камера
    TSolidObj *cameraToThisObj;
    //! индекс объекта к которому привязана каера
    int cameraToObjIndex;
    //! признак отрисовки тумана
    bool fog;
    //! задний вид из объекта
    bool cameraToBack;
    //! признак отрисовки ИЛС
    bool ils;
    //! признак отрисовки сетки
    bool grid;
    //! признак считывания txt данных из файла
    bool dataFromTXTFile;
    //! признак отрисовки вертикальных линий
    bool vertLine;
    //! признак отрисовки информации в основном окне
    bool info;
    //! признак отрисовки ландшафта
    bool terra;
    //! признак отрисовки неба
    bool sky;
    //! камеры установлена сверху над объектом
    bool cameraUp;
    //! объект для отрисовки ИЛС
    ViewILS *viewILS;

    double curPsi;
    double curGamma;
    double curTeta;
    //! компаненты вектора скорости относительно ЗСК
    double curVy_g;
    double curVx_g;
    double curVz_g;

    double curWx;
    double curWy;
    double curWz;

    //! относительно ССК
    double curVx_c;
    double curVy_c;
    double curVz_c;
    //! модуль скорости
    double curV;

    double curY;
    double curX;
    double curZ;

    double curAlfa;
    double curBeta;

    double curNxa;
    double curNya;
    double curNza;

    double curFi;
    double curUnt;
    int curIdMan;
    int curTarget;
    int indexCamForTarget;
    //! настройки отображения
    FormSettings *setting;
    //! список 3ех мерных объектов
    QVector<T3DObject> list3DObj;
    //! список траекторий
    QVector<TTrajectory> trs;

    //! указатель на визуализатор подстилающей поверхности
    View3DTerrain *terrain;
    //! отрисовка света
    qglviewer::ManipulatedFrame* light1;

    float current_frame_aircraft;
    float current_frame_target;
    float current_frame_rocket;
    //char* camera_name;
    //! сетевое взаимодействие
    VisUDP net;
    //! положение ручек
    float horStick;
    float verStick;
    float hotStick_tek;
    float verStick_tek;
    //! фильтры на органы управления
    FilterAperiodic* horFilter;
    FilterAperiodic* verFilter;
    //! географические координаты старта
    double lam0,fi0;
public slots:
    void slotAccepted();
    void slotFov(double);
    void slotRes(int w,int h);
    void slotReadRes();
    void slotPosWindow(int x,int y);
    //! отрисовка тумана
    void setFog(bool);
    //! отрисовка вертикальных линий
    void setVertical(bool);
    //! открыть текстовый файл
    void openTXTFile();
    //! отрисовка ИЛС
    void setILS(bool);
    //! отрисовка информации
    void setInfo(bool);
    //! отрисовка ландшафта
    void setTerra(bool value);
    //! частота отриосвки
    void setFreq(int);
    //! множитель времени
    void setMultTime(double);
    //! глубина тумана
    void setDensityFog(double);
    //! размер сцены
    void setSizeScene(double);
    //! отрисовка сетки
    void setGrid(bool);
    void setRotateMap(double);
    void setOffsetMapX(double);
    void setOffsetMapZ(double);
    //! сброс текущего состояния
    void slotReset();
};

#endif // VIEW3DAREA_H
