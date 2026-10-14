#ifndef VIEW3DAREA_H
#define VIEW3DAREA_H

#include <QGLViewer/qglviewer.h>

#include <QFile>
#include <QHash>
#include <QElapsedTimer>
#include <QTextStream>
#include <QTextCodec>
#include <QLocale>
#include <QOpenGLFunctions>
#include <QFileDialog>
#include <math.h>
#include <QOpenGLFunctions_3_2_Compatibility>

#include "view3DTerrain.h"
#include "view3DSky.h"
#include "view3DHud.h"
#include "terrainStreamer.h"
#include "OBJ_Loader.h"
#include "math_func.h"
#include "visudp.h"
//#include "viewILS.h"
#include "formsettings.h"
#include "TemplateNASP.hpp"
#include "glm/gtx/projection.hpp"
//! струтура описывающая траекторию
typedef struct _TTrajectory
{
    //! координаты точек: x, y, z подряд (для отрисовки массивом вершин)
    QVector<float> xyz;
    //! цвет точек: r, g, b подряд
    QVector<unsigned char> rgb;
    //! кол-во точек
    int size() const {return xyz.size() / 3;}
    void clear() {xyz.clear(); rgb.clear();}
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
    objl::Loader file;
    //! списки отображения OpenGL с непрозрачной и прозрачной геометрией
    //! модели (0 - еще не созданы)
    GLuint displayList;
    GLuint transparentList;
    //! масштаб модели к реальным размерам, б/р
    float scale;
    //! радиус описанной сферы модели (в единицах файла)
    float radius;
}T3DObject;
//! класс 3D визуализации
class view3DArea:public QGLViewer
{
    Q_OBJECT
public:

    //! 3D просмотр области визуализации
    view3DArea();
    //! установка номера камеры
    void setCameraToObject(int index);
    //! обновление указателя на объект камеры (после изменения списка объектов)
    void refreshCameraObject();
    //! добавление точки в траекторию объекта
    void appendTrajectoryPoint(int id,float x,float y,float z,int r,int g,int b);
    //! запись сеанса в файл и воспроизведение записи
    void startRecording(const QString &fileName);
    void startPlayback(const QString &fileName);
    //! привязать камеру к объекту
    void cameraToObject();
    //! Загрузить файл с 3D моделью
    //void loadFile(QString nameFile,Lib3dsFile **file3ds);
    //! чтение всех моделей
    void readAllModels();
    //! поиск объекта по коду
    T3DObject* findObjByCode(int code);
    //! инициализация сцены
//    void initScene(Lib3dsFile *file3ds);
    //! отрисовка объектов
    void drawSolidObjects();
    //! объект виден камерой (сфера с центром p и радиусом r);
    //! pixelRadius - примерный радиус на экране, пиксели
    bool objectVisible(const glm::vec3 &p, float r, float &pixelRadius);
    //! отрисовка тестового объекта
    void drawTestSolidObj();
    //! отрисовка состовного объекта
    void drawArrayObjects();
    //! отрисовка объекта
    void drawObject(T3DObject *obj,
                    double pos_x,
                    double pos_y,
                    double pos_z,
                    double psi_grad,
                    double gamma_grad,
                    double tan_grad,
                    float scale = 1.0f);
    //! формирование списков отображения модели
    void compileObject(T3DObject *obj);
    //! отрисовка рельефа
    void drawTerra();
    //! отрисовка неба (купол и солнце)
    void drawSky();
    //! отрисовка облаков
    void drawClouds();
    //! отрисовка ИЛС
    void drawILS();
    //! отрисовка сетки на плоскости OXZ размером d
    void drawGrid(double d);
    //! отрисовка траектории за объектом
    void drawTrajectory();
    //! отрисовка строки состояния
    void drawStateLine();
    void drawVecLine();
    //! разбор текстового файла
    void parserTXTFile();
    //! загрузка траектории из текстового файла
    void loadTXTFile(const QString &fileName);
    //! загрузка рельефа из интернета
    void loadTerrainOnline(double lat, double lon, double sizeKm)
    {
        terrain->loadFromInternet(lat, lon, sizeKm);
    }
    //! включение подгрузки рельефа из командной строки
    void startTerrainStreaming(double lat, double lon)
    {
        setting->setTerrainStreamingChecked(true, lat, lon);
        setTerrainStreaming(true, lat, lon);
    }
    //! статус загрузки модели из файла
    void statusLoad(bool);
    //! поиск времени на интервале
    int searchTimeInterval(double time);
    //! расчет результирующей матрицы поворота
    glm::mat3 signleCalcMatrix(TSendVehicleVisSimple *solid);
    glm::mat3 signleCalcMatrixPsi(TSendVehicleVisSimple *solid);
    //! координаты камеры в СК цели
    glm::vec3 vecCameraInTargetSystem;
    glm::vec3 vecCameraInGeoSys;
    //! размер сцены, метры
    double radiusScene;
    //! ограничение на КАИ
    LimitPositionKAI *limit;
   QOpenGLFunctions_3_2_Compatibility *glFunc;
    //! отрисовка узлов
    //void renderNode(Lib3dsFile *,Lib3dsNode *node);
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
    TSendVehicleVisSimple sW;
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
    //! часы для измерения длительности кадра
    QElapsedTimer frameClock;
    //! время с последней записи точек траекторий, с
    double trajectoryTimer;
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
    //! масштаб объекта из текстового файла (столбец "масштаб")
    float txtScale;
    //! скорость объекта из текстового файла, м/с
    double txtSpeed;
    //! текущий объект на котором установлена камера
    TSendVehicleVisSimple *cameraToThisObj;
    //! индекс объекта к которому привязана каера
    int cameraToObjIndex;
    //! идентификатор объекта, к которому привязана камера (-1 - нет)
    int cameraObjId;
    //! удаление объектов без данных через, с (0 - не удалять)
    int staleTimeoutSec;
    //! часы сцены, позиция воспроизведения записи, мс, пауза
    QElapsedTimer sceneClock;
    double playPosMs;
    bool playPaused;
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
    //ViewILS *viewILS;

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
    //! траектории по идентификатору объекта
    QHash<int, TTrajectory> trs;

    //! указатель на визуализатор подстилающей поверхности
    View3DTerrain *terrain;
    //! небо: купол, солнце, облака
    View3DSky skyView;
    //! индикатор на лобовом стекле
    View3DHud hud;
    //! подгрузка рельефа вокруг камеры по ходу полета
    TerrainStreamer *streamer;
    //! признак подгрузки рельефа (вместо карты из файла)
    bool terrainStreaming;
    //! наложение спутниковых снимков на рельеф
    bool terrainImagery;
    //! текстура снимка карты из файла (0 - нет) и номер карты, для которой она создана
    GLuint terrainTexture;
    //! список отображения рельефа с текстурными координатами снимка
    GLuint terrainListTex;
    int terrainTextureVersion;
    //! список отображения OpenGL с рельефом
    GLuint terrainList;
    //! номер карты, для которой построен список отображения
    int terrainListVersion;
    //! цвет и высота земли за пределами карты (по краю карты)
    float outerGroundColor[3];
    float outerGroundHeight;
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
    //! отрисовка неба
    void setSky(bool);
    //! сглаживание движения сетевых объектов
    void setSmoothing(bool);
    //! наложение спутниковых снимков
    void setImagery(bool);
    //! удаление объектов без данных через seconds секунд (0 - не удалять)
    void setStaleTimeout(int seconds);
    //! запись принимаемых пакетов в файл
    void setRecording(bool);
    //! выбор и воспроизведение записи
    void openPlayback();
    //! подгрузка рельефа по ходу полета с началом координат lat, lon (град)
    void setTerrainStreaming(bool value, double lat, double lon);
    void setRotateMap(double);
    void setOffsetMapX(double);
    void setOffsetMapZ(double);
    //! сброс текущего состояния
    void slotReset();
};

#endif // VIEW3DAREA_H
