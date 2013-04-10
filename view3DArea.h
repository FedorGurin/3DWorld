#ifndef VIEW3DAREA_H
#define VIEW3DAREA_H

#include <QGLViewer/qglviewer.h>


#include <lib3ds/file.h>
#include <lib3ds/node.h>

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QLocale>
#include <math.h>

#include "view3dTerrain.h"
#include "AligmentForm.h"
#include "Matrix3D_D.h"
#include "Vector3D_D.h"
#include "visudp.h"
#include "viewILS.h"


typedef struct _TTrajectory
{
    _TTrajectory()
    {
        position.clear();
        setBindingCamera=false;
        setGorizontCamera=false;
    }

    QVector<TVis> position;
    bool setBindingCamera;
    bool setGorizontCamera;
}TTrajectory;

typedef struct
{
    //! код объекта
    int code;
    //! загрузка файла с объектом
    Lib3dsFile *file;
}T3DObject;
class view3DArea:public QGLViewer
{
    Q_OBJECT;
public:
    //! 3D просмотр области визуализации
    view3DArea();
    //! установка номера камеры
    void setCameraToObject(int index);
    //! расчитать
    void cameraToObject();
    //! Загрузить файл с 3D моделью
    void loadFile(QString nameFile,Lib3dsFile **file3ds);
    //! чтение всех моделей
    void readAllModels();
    //! поиск объекта по коду
    T3DObject* findObjByCode(int code);

    //! инициализация сцены
    void initScene(Lib3dsFile *file3ds);
    //! отрисовка воздушных объектов

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
    //! загрузить рельеф
    void drawTerra();
    //! отрисовка сетки
    void drawILS();
    //! отрисовка прицельной сиволики
    void drawSymbol();
    //! отрисовка окружности
    void drawCircle(TAngle *angle,int radius_);
    //! расчет результирующей матрицы поворота
    Matrix3D_D signleCalcMatrix(TVis *solid);
    //! координаты камеры в СК цели
    Vector3D_D vecCameraInTargetSystem;
    Vector3D_D vecCameraInGeoSys;
    //! размер сцены
    double radiusScene;
    LimitPositionKAI *limit;

    QString nameFileAirCraft;
    QString nameFileTarget;

    void renderNode(Lib3dsFile *,Lib3dsNode *node);
protected :
    virtual void draw();
    virtual void animate();
    virtual void init();
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
private:
    //! текущий объект на котором установлена камера
    TVis *cameraToThisObj;
    //! индекс объекта к которому привязана каера
    int cameraToObjIndex;
    //! задний вид из объекта
    bool cameraToBack;
    //! признак отрисовки ИЛС
    bool ils;
    bool grid;
    //! объект для отрисовки ИЛС
    ViewILS *viewILS;
    //! текущие параметры
    /*double teta;
    double psi;
    double gamma;

    double teta_radian;
    double gamma_radian;
    double psi_radian;

    double x;
    double y;
    double z;

    double x_target;
    double y_target;
    double z_target;

    double fov;
    double hfov;

    //! калибровочные параметры
    double teta0;
    double psi0;
    double gamma0;*/

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


    AligmentForm *alignmentForm;

    QVector<T3DObject> list3DObj;
//    //! 3D объект нашего самолета
//    Lib3dsFile *fileBAircraft;
//    //! 3D объект для цели
//    Lib3dsFile *fileRAircraft;
//    //! 3D объект для ракеты
//    Lib3dsFile *fileRocket;
//    //! 3D объект для наземной цели
//    Lib3dsFile *fileGroundTarget;
//    //! 3D объект
    //! для отладки
    //Lib3dsFile *file;
    //! указатель на визуализатор подстилающей поверхности
    View3DTerrain *terrain;
    qglviewer::ManipulatedFrame* light1;

    float current_frame_aircraft;
    float current_frame_target;
    float current_frame_rocket;
    char* camera_name;

    VisUDP net;

    float horStick;
    float verStick;
    float hotStick_tek;
    float verStick_tek;

    FilterAperiodic* horFilter;
    FilterAperiodic* verFilter;

    double lam0,fi0;
public slots:
    //! реакция на форму
    //void slotTeta0(double);
    //void slotPsi0(double);
    //void slotGamma0(double);
    void slotFov(double);
    void slotRes(int w,int h);
    void slotReadRes();
    void slotPosWindow(int x,int y);



    //void slotCoord(double,double,double);
    //! слот при обновление данных носителя
    //void slotSignalAircraft();
    //! слот при обновлении данных воздушых целей
    //void slotSignalAirTarget();
    //! слот при обновлении данных наземных целей
    //void slotSignalGroundTarget();

    //void slotReciveData(char*);
};

#endif // VIEW3DAREA_H
