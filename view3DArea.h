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
    //! ��� �������
    int code;
    //! �������� ����� � ��������
    Lib3dsFile *file;
}T3DObject;
class view3DArea:public QGLViewer
{
    Q_OBJECT;
public:
    //! 3D �������� ������� ������������
    view3DArea();
    //! ��������� ������ ������
    void setCameraToObject(int index);
    //! ���������
    void cameraToObject();
    //! ��������� ���� � 3D �������
    void loadFile(QString nameFile,Lib3dsFile **file3ds);
    //! ������ ���� �������
    void readAllModels();
    //! ����� ������� �� ����
    T3DObject* findObjByCode(int code);

    //! ������������� �����
    void initScene(Lib3dsFile *file3ds);
    //! ��������� ��������� ��������

    //! ��������� ��������
    void drawSolidObjects();
    //! ��������� �������
    void drawObject(Lib3dsFile *obj,
                    double pos_x,
                    double pos_y,
                    double pos_z,
                    double psi_grad,
                    double gamma_grad,
                    double tan_grad);
    //! ������� �������� � �������
    void gradToPixel(double xGrad,
                     double yGrad,
                     int &xPixel,
                     int &yPixel);
    //! ��������� ������
    void drawTerra();
    //! ��������� �����
    void drawILS();
    //! ��������� ���������� ��������
    void drawSymbol();
    //! ��������� ����������
    void drawCircle(TAngle *angle,int radius_);
    //! ������ �������������� ������� ��������
    Matrix3D_D signleCalcMatrix(TVis *solid);
    //! ���������� ������ � �� ����
    Vector3D_D vecCameraInTargetSystem;
    Vector3D_D vecCameraInGeoSys;
    //! ������ �����
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
    //! ������� ������ �� ������� ����������� ������
    TVis *cameraToThisObj;
    //! ������ ������� � �������� ��������� �����
    int cameraToObjIndex;
    //! ������ ��� �� �������
    bool cameraToBack;
    //! ������� ��������� ���
    bool ils;
    bool grid;
    //! ������ ��� ��������� ���
    ViewILS *viewILS;
    //! ������� ���������
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

    //! ������������� ���������
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
//    //! 3D ������ ������ ��������
//    Lib3dsFile *fileBAircraft;
//    //! 3D ������ ��� ����
//    Lib3dsFile *fileRAircraft;
//    //! 3D ������ ��� ������
//    Lib3dsFile *fileRocket;
//    //! 3D ������ ��� �������� ����
//    Lib3dsFile *fileGroundTarget;
//    //! 3D ������
    //! ��� �������
    //Lib3dsFile *file;
    //! ��������� �� ������������ ������������ �����������
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
    //! ������� �� �����
    //void slotTeta0(double);
    //void slotPsi0(double);
    //void slotGamma0(double);
    void slotFov(double);
    void slotRes(int w,int h);
    void slotReadRes();
    void slotPosWindow(int x,int y);



    //void slotCoord(double,double,double);
    //! ���� ��� ���������� ������ ��������
    //void slotSignalAircraft();
    //! ���� ��� ���������� ������ �������� �����
    //void slotSignalAirTarget();
    //! ���� ��� ���������� ������ �������� �����
    //void slotSignalGroundTarget();

    //void slotReciveData(char*);
};

#endif // VIEW3DAREA_H
