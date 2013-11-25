#ifndef VIEW3DAREA_H
#define VIEW3DAREA_H

#include <GL\glew.h>
#include <QtOpenGL\QGLWidget>
#include <QGLViewer\qglviewer.h>
#include <GL\GL.h>


#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QLocale>
#include <math.h>

#include "view3dTerrain.h"

#include "Matrix3D_D.h"
#include "Vector3D_D.h"
#include "visudp.h"
#include "viewILS.h"
#include "formsettings.h"
#include "TemplateNASP.hpp"
#include <QtOpenGL\QGLShaderProgram>
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
    //Lib3dsFile *file;
}T3DObject;
//! класс 3D визуализации
class view3DArea:public QGLViewer
{
    QGLShaderProgram *prg;
    quint32 vb;
    //GLuint theProgram;
    
    GLuint offsetUniform;
    GLuint frustumScaleUnif;
    GLuint zNearUnif, zFarUnif;
    //GLuint vertexBufferObject;
    GLuint vao;
    GLuint perspectiveMatrixUnif;

    Q_OBJECT;
public:
    //! 3D просмотр области визуализации
    view3DArea();
    //! установка номера камеры
    void setCameraToObject(int index);
    //! расчитать
    void cameraToObject();
    //! Загрузить файл с 3D моделью
    //void loadFile(QString nameFile,Lib3dsFile **file3ds);
    //! чтение всех моделей
    void readAllModels();
    //! поиск объекта по коду
    T3DObject* findObjByCode(int code);
    //! инициализация сцены
    //void initScene(Lib3dsFile *file3ds);
    //! отрисовка объектов
    //void drawSolidObjects();
    //! отрисовка объекта
    /*void drawObject(Lib3dsFile *obj,
                    double pos_x,
                    double pos_y,
                    double pos_z,
                    double psi_grad,
                    double gamma_grad,
                    double tan_grad);*/
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
    Matrix3D_D signleCalcMatrix(TVis *solid);
    //! координаты камеры в СК цели
    Vector3D_D vecCameraInTargetSystem;
    Vector3D_D vecCameraInGeoSys;
    //! размер сцены, метры
    double radiusScene;
    //! ограничение на КАИ
    LimitPositionKAI *limit;
    //! отрисовка узлов
    //void renderNode(Lib3dsFile *,Lib3dsNode *node);
    void computePositionOffsets(float &fXOffset, float &fYOffset);
protected :
    virtual void draw();
    virtual void animate();
    virtual void init();
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
private:
    //! множитель времени, б/р
    float multTime;
    //! глобальное время, сек
    double globalTime;
    //! период анимации, Гц
    int dt;//мсек
    //! глобальный счетчик
    int globalCount;
    //! для чтения TXT файла с данными
    QFile *file;
    QTextStream *stream;
    QFileDialog *dialog;
    //! содержимое текстового файла(список строк)
    QVector<TDataRow> rows;
    //! текущий объект на котором установлена камера
    TVis *cameraToThisObj;
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
    //! объект для отрисовки ИЛС
    ViewILS *viewILS;

    double curPsi;
    double curGamma;
    double curTeta;
    double curVy;
    double curV;
    double curY;
    double curAlfa;
    double curNya;
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
    char* camera_name;
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
};

#endif // VIEW3DAREA_H
