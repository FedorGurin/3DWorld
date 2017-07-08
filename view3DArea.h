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
//! �������� ����������� ����������
typedef struct _TTrajectory
{
    //! ������ � ������������
    QVector<float> x;
    QVector<float> y;
    QVector<float> z;
    //! ���� ����������
    QVector<int>    r;
    QVector<int>    g;
    QVector<int>    b;

}TTrajectory;

//! ��������� ����������� ������ � �������
typedef struct _TDataRow
{
    //! ������������� �������
    int id_obj;
    //! � ����������� �� ���� ����� ������������ 3D ������
    int code;
    //! ������� 3D �������, �/�
    float scale;
    //! �����, ���
    double time;
    //! ����
    int r;
    int g;
    int b;
    //! ���������� �������, �����
    float x_g;
    float y_g;
    float z_g;
    //! ���� ���������� �������, ����
    float psi;
    float gamma;
    float tan;
}TDataRow;

typedef struct
{
    //! ��� �������
    int code;
    //! �������� ����� � ��������
    Lib3dsFile *file;
}T3DObject;
//! ����� 3D ������������
class view3DArea:public QGLViewer
{
    Q_OBJECT;
public:
    //! 3D �������� ������� ������������
    view3DArea();
    //! ��������� ������ ������
    void setCameraToObject(int index);
    //! ��������� ������ � �������
    void cameraToObject();
    //! ��������� ���� � 3D �������
    void loadFile(QString nameFile,Lib3dsFile **file3ds);
    //! ������ ���� �������
    void readAllModels();
    //! ����� ������� �� ����
    T3DObject* findObjByCode(int code);
    //! ������������� �����
    void initScene(Lib3dsFile *file3ds);
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
    //! ��������� �������
    void drawTerra();
    //! ��������� ����
    void drawSky();
    //! ��������� �����
    void drawILS();
    //! ��������� ���������� ��������
    void drawSymbol();
    //! ��������� ����������
    void drawCircle(TAngle *angle,int radius_,int width_);
    //! ��������� �����������
    void drawCross(TAngle* angle,int radius_,int width_);
    //! ��������� ���������� �� ��������
    void drawTrajectory();
    //! ��������� ������ ���������
    void drawStateLine();
    //! ������ ���������� �����
    void parserTXTFile();
    //! ����� ������� �� ���������
    int searchTimeInterval(double time);
    //! ������ �������������� ������� ��������
    glm::mat3 signleCalcMatrix(TSolidObj *solid);
    glm::mat3 signleCalcMatrixPsi(TSolidObj *solid);
    //! ���������� ������ � �� ����
    glm::vec3 vecCameraInTargetSystem;
    glm::vec3 vecCameraInGeoSys;
    //! ������ �����, �����
    double radiusScene;
    //! ����������� �� ���
    LimitPositionKAI *limit;
    //! ��������� �����
    void renderNode(Lib3dsFile *,Lib3dsNode *node);
protected :
    virtual void draw();
    virtual void animate();
    virtual void init();
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);
private:
    //! ���������� ������
    TControlStick stick;
    //! ������� ���������
    TSolidObj sW;
    //! ���������� � �������� ������
    double dpsi_camera;
    double dteta_camera;
    double dgamma_camera;
    //! ��������� �������, �/�
    float multTime;
    //! ������� ������,[0.0;1.0]
    float dFog;
    //! ���������� �����, ���
    double globalTime;
    double globalTimeDebug;
    //! ������ ��������, ��
    int dt;//����
    //! ���������� �������
    int globalCount;
    //! ������� �����
    double rotMap;
    double offsetMapX;
    double offsetMapZ;
    //! ��� ������ TXT ����� � �������
    QFile *file;
    QTextStream *stream;
    QFileDialog *dialog;
    //! ���������� ���������� �����(������ �����)
    QVector<TDataRow> rows;
    //! ������� ������ �� ������� ����������� ������
    TSolidObj *cameraToThisObj;
    //! ������ ������� � �������� ��������� �����
    int cameraToObjIndex;
    //! ������� ��������� ������
    bool fog;
    //! ������ ��� �� �������
    bool cameraToBack;
    //! ������� ��������� ���
    bool ils;
    //! ������� ��������� �����
    bool grid;
    //! ������� ���������� txt ������ �� �����
    bool dataFromTXTFile;
    //! ������� ��������� ������������ �����
    bool vertLine;
    //! ������� ��������� ���������� � �������� ����
    bool info;
    //! ������� ��������� ���������
    bool terra;
    //! ������� ��������� ����
    bool sky;
    //! ������ ����������� ������ ��� ��������
    bool cameraUp;
    //! ������ ��� ��������� ���
    ViewILS *viewILS;

    double curPsi;
    double curGamma;
    double curTeta;
    //! ���������� ������� �������� ������������ ���
    double curVy_g;
    double curVx_g;
    double curVz_g;

    double curWx;
    double curWy;
    double curWz;

    //! ������������ ���
    double curVx_c;
    double curVy_c;
    double curVz_c;
    //! ������ ��������
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
    //! ��������� �����������
    FormSettings *setting;
    //! ������ 3�� ������ ��������
    QVector<T3DObject> list3DObj;
    //! ������ ����������
    QVector<TTrajectory> trs;

    //! ��������� �� ������������ ������������ �����������
    View3DTerrain *terrain;
    //! ��������� �����
    qglviewer::ManipulatedFrame* light1;

    float current_frame_aircraft;
    float current_frame_target;
    float current_frame_rocket;
    //char* camera_name;
    //! ������� ��������������
    VisUDP net;
    //! ��������� �����
    float horStick;
    float verStick;
    float hotStick_tek;
    float verStick_tek;
    //! ������� �� ������ ����������
    FilterAperiodic* horFilter;
    FilterAperiodic* verFilter;
    //! �������������� ���������� ������
    double lam0,fi0;
public slots:
    void slotAccepted();
    void slotFov(double);
    void slotRes(int w,int h);
    void slotReadRes();
    void slotPosWindow(int x,int y);
    //! ��������� ������
    void setFog(bool);
    //! ��������� ������������ �����
    void setVertical(bool);
    //! ������� ��������� ����
    void openTXTFile();
    //! ��������� ���
    void setILS(bool);
    //! ��������� ����������
    void setInfo(bool);
    //! ��������� ���������
    void setTerra(bool value);
    //! ������� ���������
    void setFreq(int);
    //! ��������� �������
    void setMultTime(double);
    //! ������� ������
    void setDensityFog(double);
    //! ������ �����
    void setSizeScene(double);
    //! ��������� �����
    void setGrid(bool);
    void setRotateMap(double);
    void setOffsetMapX(double);
    void setOffsetMapZ(double);
    //! ����� �������� ���������
    void slotReset();
};

#endif // VIEW3DAREA_H
