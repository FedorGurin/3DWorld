#include "view3DArea.h"

#include <lib3ds/camera.h>
#include <lib3ds/mesh.h>
#include <lib3ds/material.h>
#include <lib3ds/matrix.h>
#include <lib3ds/vector.h>
#include <lib3ds/light.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glu.h>

#include <QMessageBox>
#include <QKeyEvent>
#include <QApplication>
#include <QVector3D>
#include <QDataStream>
#include <QByteArray>

using namespace qglviewer;
#define SIZE_GRID 10
#define MAX_Y 20000.0
//#define PERIOD_ANIMATION 50
#define PERIOD_ANIMATION 1000
#define DIVIDE 10
//#define VERT_GRID

#define USE_3DMODEL
#define FOG_OFF 1

extern "C" { LIB3DSAPI void lib3ds_file_bounding_box(Lib3dsFile *fileAirCraft, Lib3dsVector min, Lib3dsVector max); }

view3DArea::view3DArea():QGLViewer()
{
    resize(800,600);
    cameraToThisObj=0;
    cameraToObjIndex=-1;
    cameraToBack=false;
    dataFromTXTFile=false;

    dialog=new QFileDialog(this);
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setNameFilter(("File with Data (*.txt)"));
    dialog->setDirectory(qApp->applicationDirPath());

    file=new QFile;
    stream=new QTextStream(file);
    setting=new FormSettings;
    connect(dialog,SIGNAL(accepted()),this,SLOT(slotAccepted()));

    lam0=57.0;
    fi0=33.0;
    rotMap=90.0;
    offsetMapX=0.0;
    offsetMapZ=0.0;
    dFog=0.5;
    globalCount=0;
    ils=true;
    vertLine=false;
    fog=false;
    info=false;
    terra=true;
    sky=false;
    grid=true;
    radiusScene=60000.0;
    globalTime=0.0;
    multTime=1.0;
    dt=((1.0/20)*1000.0);;

    dpsi_camera=0.0;
    dteta_camera=0.0;
    dgamma_camera=0.0;

    rows.clear();
    trs.clear();
    trs.resize(1);

    current_frame_aircraft=0.0;
    current_frame_target=0.0;

    camera_name=0;

    terrain=new View3DTerrain;

    limit=new LimitPositionKAI;
    //viewILS=new ViewILS(&net);

    horFilter=new FilterAperiodic(0.0,1.0,PERIOD_ANIMATION/1000.0,1.0,-1.0);
    verFilter=new FilterAperiodic(0.0,1.0,PERIOD_ANIMATION/1000.0,1.0,-1.0);

    camera()->setPosition(Vec(0.0,
                              2000.0,
                              0.0));

    vecCameraInTargetSystem=Vector3D_D::zero;
    vecCameraInTargetSystem.y=100;
    vecCameraInTargetSystem.x=-1500;

    connect(setting,SIGNAL(fog(bool)),this,SLOT(setFog(bool)));
    connect(setting,SIGNAL(grid(bool)),this,SLOT(setGrid(bool)));

    connect(setting,SIGNAL(info(bool)),this,SLOT(setInfo(bool)));
    connect(setting,SIGNAL(ils(bool)),this,SLOT(setILS(bool)));
    connect(setting,SIGNAL(vertical(bool)),this,SLOT(setVertical(bool)));
    connect(setting,SIGNAL(loadLand()),terrain,SLOT(openFileWithTerrain()));
    connect(setting,SIGNAL(loadDataFlight()),this,SLOT(openTXTFile()));
    connect(setting,SIGNAL(terra(bool)),this,SLOT(setTerra(bool)));
    connect(setting,SIGNAL(freq(int)),this,SLOT(setFreq(int)));
    connect(setting,SIGNAL(multTime(double)),this,SLOT(setMultTime(double)));
    connect(setting,SIGNAL(densityFog(double)),this,SLOT(setDensityFog(double)));
    connect(setting,SIGNAL(sizeScene(double)),this,SLOT(setSizeScene(double)));
    connect(setting,SIGNAL(offsetMapX(double)),this,SLOT(setOffsetMapX(double)));
    connect(setting,SIGNAL(offsetMapZ(double)),this,SLOT(setOffsetMapZ(double)));
    connect(setting,SIGNAL(rotateMap(double)),this,SLOT(setRotateMap(double)));


    init();
}
void view3DArea::setGrid(bool value)
{
    grid=value;
}

void view3DArea::openTXTFile()
{
    dialog->exec();
}
void view3DArea::setFog(bool value)
{
    fog=value;
}
void view3DArea::setTerra(bool value)
{
    terra=value;
}
void view3DArea::setInfo(bool value)
{
    info=value;
}
void view3DArea::setMultTime(double value)
{

    multTime=value;
}
void view3DArea::setDensityFog(double value)
{
   // qDebug("setDensityFog");
    dFog=value;
}
void view3DArea::setSizeScene(double value)
{
    radiusScene=value;
}

void view3DArea::setFreq(int freq)
{
    //qDebug("catch\n");
    dt=((1.0/freq)*1000.0);
    setAnimationPeriod(dt);
    stopAnimation();
    startAnimation();
}
void view3DArea::setRotateMap(double value)
{
    rotMap=value;
}
void view3DArea::setOffsetMapX(double value)
{
    offsetMapX=value;
}

void view3DArea::setOffsetMapZ(double value)
{
    offsetMapZ=value;
}

void view3DArea::setILS(bool value)
{
    ils=value;
}
void view3DArea::setVertical(bool value)
{
    vertLine=value;
}
void view3DArea::parserTXTFile()
{
    QStringList list=(stream->readAll()).split("\n",QString::SkipEmptyParts);

    for(long i=0;i<list.size();i++)
    {
        TDataRow row;
        QStringList tempList=list[i].split("\t",QString::SkipEmptyParts);
        //! код, б/р
        row.code=   tempList[0].toInt();
        //! масштаб, б/р
        row.scale=  tempList[1].toFloat();
        //! время, сек
        row.time=   tempList[2].toFloat();
        //! цвет
        row.r=      80;
        row.g=      223;
        row.b=      72;
        //! считывание координат, м
        row.x_g=    tempList[3].toFloat();
        row.y_g=    tempList[4].toFloat();
        row.z_g=    tempList[5].toFloat();
        //! считывание углов ориентации объекта, град
        row.psi=    GradToRadian(tempList[6].toFloat());
        row.tan=    GradToRadian(tempList[7].toFloat());
        row.gamma=  GradToRadian(tempList[8].toFloat());
        rows.push_back(row);
    }

}

void view3DArea::slotAccepted()
{
    QStringList list=dialog->selectedFiles();
    if(list.size()!=0)
    {
        QString fileName=list[0];

        file->setFileName(fileName);
        if (!file->open(QIODevice::ReadOnly | QIODevice::Text))   return;

        globalCount=0;
        dataFromTXTFile=true;
        net.setSizeObj(1);
        parserTXTFile();
        globalTime=rows[0].time;

    }
}
void view3DArea::readAllModels()
{
    list3DObj.resize(6);
    list3DObj[0].code=100;
    list3DObj[1].code=101;
    list3DObj[2].code=102;
    list3DObj[3].code=103;
    list3DObj[4].code=104;
    list3DObj[5].code=105;
    //! чтение моделей из 3ds файлов
    loadFile("./3dmodels/aircraft.3ds",  &(list3DObj[0].file));
    loadFile("./3dmodels/target.3ds",    &(list3DObj[1].file));
    loadFile("./3dmodels/x55.3ds",       &(list3DObj[2].file));
    loadFile("./3dmodels/WATER_T2.3ds",  &(list3DObj[3].file));
    loadFile("./3dmodels/bomb.3ds",      &(list3DObj[4].file));
    loadFile("./3dmodels/buran.3ds",     &(list3DObj[5].file));
}

void view3DArea::init()
{
#ifdef USE_3DMODEL
    readAllModels();
    setAxisIsDrawn(false);
    setFPSIsDisplayed(true);
    //setFullScreen(true);
    //setAnimationPeriod(dt);
#endif
    //! открыть рельеф по умолчанию
    terrain->openTerrainMap("/defaultTerrain.asc");
    QColor c(81,168,255);
    this->setBackgroundColor(c);
    startAnimation();
}

void view3DArea::draw()
{
    double d=0;
    glDisable(GL_LIGHTING);
    net.checkDatagrams();
    if(cameraToThisObj!=0)
    {
        d=6371000.0*atan(sqrt(cameraToThisObj->yg*(2*6371000.0+cameraToThisObj->yg))/6371000.0);
        setSceneRadius(d);
    }else
    {
        setSceneRadius(radiusScene);
        d=radiusScene;
    }



    ////////////////////////////////////////////////
    GLfloat fogColor[4];//= {0.1f, 0.1f, 0.5f, 1.0f}; // Цвет тумана

    QColor c(81,168,255);
    //QColor c(Qt::cyan);
    fogColor[0]=c.redF();
    fogColor[1]=c.greenF();
    fogColor[2]=c.blueF();
    fogColor[3]=1.0;

    if(fog==true)
    {
        glEnable(GL_FOG);               // Включает туман (GL_FOG)
        glFogi(GL_FOG_MODE, GL_LINEAR); // Выбираем тип тумана
        glFogfv(GL_FOG_COLOR, fogColor);// Устанавливаем цвет тумана
        glFogf(GL_FOG_DENSITY, dFog);   // Насколько густым будет туман
        glHint(GL_FOG_HINT, GL_NICEST); // Вспомогательная установка тумана
        glFogf(GL_FOG_START, d/2);      // Глубина, с которой начинается туман
        glFogf(GL_FOG_END, d);          // Глубина, где туман заканчивается.
    }else
    {
        glDisable(GL_FOG);
    }

    glScalef(1.0,1.0,1.0);

    glBegin(GL_POINTS);
    qglColor(QColor(Qt::red));

    glEnd();

    drawTrajectory();



#ifdef USE_3DMODEL
    //! отрисовка трехмерных объектов
    drawSolidObjects();
#endif
    //! отрисовка подстилающей поверхности
    if(terra==true)
        drawTerra();
    //! отрисовка ИЛС
    if(ils==true)
        drawILS();
    if(sky==true)
        drawSky();
    //////////////////////////
    glDisable(GL_LIGHTING);
    //glEnable(GL_BLEND);
    //! Полная яркость, 50% альфа (НОВОЕ)
    glColor4f(1.0f,1.0f,1.0f,0.7f);
    //! Функция смешивания для непрозрачности
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);

    //if(grid==true) drawGrid();
    drawStateLine();
    if(info==true)
    {
        drawText(10,60,"[+] zoom UP");
        drawText(10,80,"[-] zoom DOWN");
        drawText(10,100,"Current Scale="+QString::number(d)+" meters");
        drawText(10,150,"Psi="+QString::number(curPsi));
        drawText(10,170,"Gamma="+QString::number(curGamma));
        drawText(10,190,"Teta="+QString::number(curTeta));
        drawText(10,210,"Vy="+QString::number(curVy));
        drawText(10,230,"y="+QString::number(curY));
        drawText(10,250,"V="+QString::number(curV));
        drawText(10,270,"alfa="+QString::number(curAlfa));
        drawText(10,290,"nya="+QString::number(curNya));
        drawText(10,310,"fi="+QString::number(curFi));
        drawText(10,330,"unt="+QString::number(curUnt));

        drawText(10,120,"HorFilter="+QString::number(horFilter->curValue()));
        drawText(10,140,"VerFilter="+QString::number(verFilter->curValue()));

        QString nameMan="Unknown";

        switch(curIdMan)
        {
            case 1000:{nameMan="Homing";break;}
            case 1:{nameMan="left_turn";break;}
            case 2:{nameMan="right_turn";break;}
            case 5:{nameMan="halfcross";break;}
            case 7:{nameMan="gorka";break;}
            case 9:{nameMan="prividenie";break;}
            case 10:{nameMan="dive";break;}
            case 12:{nameMan="battle_allign";break;}
            case 14:{nameMan="none";break;}

        };
        drawText(10,350,"man="+nameMan);
    }
    //drawLight(GL_LIGHT0);

    if(grid==true)
    {
    glPushMatrix();
    //glDisable(GL_DEPTH_TEST);
    ////////Нарисуем плоскость OXZ
    //glScalef(radiusScene,radiusScene,radiusScene);
    glScalef(d,d,d);
    //glDisable(GL_LIGHTING);//Отключим свет
    glBegin(GL_LINES);

        float stepGrid=2.0/SIZE_GRID;//Шаг сетки
        for(int i=0;i<=SIZE_GRID;i++)
        {
            /////////сетка при Z=0.0//////////////
            qglColor(QColor(170,200,160,255));
            //qglColor(QColor(11,11,11));

            double dY=1.0/MAX_Y;

#ifdef VERT_GRID
            glVertex3f(1.0-i*stepGrid,0.0,0.0);
            glVertex3f(1.0-i*stepGrid,1.0,0.0);
            glVertex3f(-1.0,fabs(1.0-i*stepGrid),0.0);
            glVertex3f(1.0,fabs(1.0-i*stepGrid),0.0);
            //////////////////////////////////////

            ///////сетка при X=0.0///////////////
            glVertex3f(0.0,0.0,1.0-i*stepGrid);
            glVertex3f(0.0,1.0,1.0-i*stepGrid);
            glVertex3f(0.0,fabs(1.0-i*stepGrid),-1.0);
            glVertex3f(0.0,fabs(1.0-i*stepGrid),1.0);
            ////////////////////////////////////
#endif
            /////////сетка при Y=0.0///////////////
            glVertex3f(-1.0,0.001,1.0-i*stepGrid);
            glVertex3f(1.0,0.001,1.0-i*stepGrid);

            glVertex3f(1.0-i*stepGrid,0.001,-1.0);
            glVertex3f(1.0-i*stepGrid,0.001,1.0);
        }

    glDisable(GL_BLEND);//Уберем прозрачность
    glEnd();
    //////////////////////////////////////////
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
/*    qglColor(QColor(Qt::gray));
    glBegin(GL_QUADS);
    //qglColor(QColor(229,210,175,220));


    glVertex3f(1.0,0.0,-1.0);
    glVertex3f(-1.0,0.0,-1.0);
    glVertex3f(-1.0,0.0,1.0);
    glVertex3f(1.0,0.0,1.0);
    glEnd();*/

    glDisable(GL_BLEND);//Уберем прозрачность

    glLineWidth(1.0);
    glPointSize(4.0);
    glPopMatrix();


}
}
void view3DArea::drawTrajectory()
{
    //! отрисовка точек
    glBegin(GL_POINTS);

    /*for(long i=0;i<trs[0].x.size();i++)
    {
        qglColor(QColor(trs[0].r[i],trs[0].g[i],trs[0].b[i]));
        glVertex3f(trs[0].x[i],trs[0].y[i],trs[0].z[i]);
        glVertex3f(trs[0].x[i],0,trs[0].z[i]);
    }
    glEnd();
    if(vertLine==true)
    {
        //! отрисовка вертикальных линий
        glBegin(GL_LINES);
        for(long i=0;i<trs[0].x.size();i=i+5)
        {
            glVertex3f(trs[0].x[i],0.0,trs[0].z[i]);
            glVertex3f(trs[0].x[i],trs[0].y[i],trs[0].z[i]);
        }
        glEnd();
    }*/
    for(long i=0;i<rows.size();i++)
    {
        qglColor(QColor(80,223,72));
        glVertex3f(rows[i].x_g,rows[i].y_g,rows[i].z_g);
        //glVertex3f(rows[i].x_g,0,rows[i].z_g);
    }
    glEnd();
    if(vertLine==true)
    {
        //! отрисовка вертикальных линий
        glBegin(GL_LINES);
        for(long i=0;i<rows.size();i=i+5)
        {
            glVertex3f(rows[i].x_g,0.0,rows[i].z_g);
            glVertex3f(rows[i].x_g,rows[i].y_g,rows[i].z_g);
        }
        glEnd();
    }
}

void view3DArea::animate()
{
    //! счетчик времени, сек
    globalTime=globalTime+dt*0.001*multTime;
    //! если данные загружены из файла
    if(dataFromTXTFile==true)
    {
        if(globalTime>=(rows.back().time))
        {
            globalCount=0;
            globalTime=0.0;
            trs[0].x.clear();
            trs[0].y.clear();
            trs[0].z.clear();
        }

        int index=searchTimeInterval(globalTime);
        //! записываем параметры
        QVector<TVis> *list=net.getObjects();

        TVis *solid=&((*list)[0]);
        solid->code=rows[index].code;


        //! эти параметры нужно прогнать через блок интерполяции
        solid->xg=linearInterpolation(globalTime,
                                      rows[index].x_g,
                                      rows[index-1].x_g,
                                      rows[index].time,
                                      rows[index-1].time);

        solid->yg=linearInterpolation(globalTime,
                                      rows[index].y_g,
                                      rows[index-1].y_g,
                                      rows[index].time,
                                      rows[index-1].time);

        solid->zg=linearInterpolation(globalTime,
                                      rows[index].z_g,
                                      rows[index-1].z_g,
                                      rows[index].time,
                                      rows[index-1].time);

        solid->psi=linearInterpolation(globalTime,
                                       rows[index].psi,
                                       rows[index-1].psi,
                                       rows[index].time,
                                       rows[index-1].time);

        solid->tan=linearInterpolation(globalTime,
                                       rows[index].tan,
                                       rows[index-1].tan,
                                       rows[index].time,
                                       rows[index-1].time);

        solid->gamma=linearInterpolation(globalTime,
                                         rows[index].gamma,
                                         rows[index-1].gamma,
                                         rows[index].time,
                                         rows[index-1].time);

        //! заполнение траектории
        trs[0].x.push_back(solid->xg);
        trs[0].y.push_back(solid->yg);
        trs[0].z.push_back(solid->zg);
        //! цвет траектории
        trs[0].r.push_back(rows[index].r);
        trs[0].g.push_back(rows[index].g);
        trs[0].b.push_back(rows[index].b);
    }

    horFilter->refresh();
    verFilter->refresh();
    //! установить камеру на объект
    cameraToObject();


    //! отправка органов управления
    if(horFilter->delta()>10e-3 || verFilter->delta()>10e-3)
    {
        if(cameraToThisObj!=0)
        {
            QByteArray array;
            QDataStream in(&array,QIODevice::WriteOnly);


            float x=horFilter->curValue();
            float y=verFilter->curValue();

            in<<x;
            in<<y;

            net.sendData(cameraToThisObj->id,CONTROL_STICK,array);
        }
    }
}

T3DObject* view3DArea::findObjByCode(int code)
{
    for(int i=0;i<list3DObj.size();i++)
    {
        if(list3DObj[i].code==code)
            return &list3DObj[i];
    }
    return 0;
}
int view3DArea::searchTimeInterval(double time)
{
    for(int i=0;i<rows.size();i++)
    {
        if(rows[i].time>time)
        {
           return i;
        }
    }
}
void view3DArea::drawSolidObjects()
{
    Lib3dsFile* file3DS=0;
    QVector<TVis> *list=net.getObjects();
    //qDebug("obj=%d\n",net.getObjects()->size());
    for(int i=0;i<list->size();i++)
    {
        TVis *solid=&((*list)[i]);
        if(findObjByCode(solid->code)!=0)
            file3DS=findObjByCode(solid->code)->file;

        if(file3DS!=0)
        {
//            convertSphereToDekart(lam0,
//                                  fi0,
//                                  solid->lam_geo,
//                                  solid->fi_geo,
//                                  solid->x,
//                                  solid->z);


                drawObject(file3DS,
                           solid->xg,
                           solid->yg,
                           solid->zg,
                           solid->psi,
                           solid->gamma,
                           solid->tan);

        }

    }
}

void view3DArea::drawObject(Lib3dsFile *obj,
                            double pos_x,
                            double pos_y,
                            double pos_z,
                            double psi_rad,
                            double gamma_rad,
                            double tan_rad)
{
    glPushMatrix();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);


    glTranslatef(pos_x,
                 pos_y,
                 pos_z);

    glRotated(RadianToGrad(psi_rad),0.0,1.0,0.0);
    glRotated(RadianToGrad(tan_rad),0.0,0.0,1.0);
    glRotated(RadianToGrad(gamma_rad),1.0,0.0,0.0);

    glColor4f(1.0f,1.0f,1.0f,1.0f);
    for (Lib3dsNode* p=obj->nodes; p!=0; p=p->next)
        renderNode(obj,p);

    glDisable(GL_LIGHTING);
    glPopMatrix();

}
//! отрисовка ИЛС
void view3DArea::drawILS()
{
    if(cameraToThisObj==0)
        return;

    /////////////////////////////////////////
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width(), height(), 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /////////////////////////////////////////
    //! отрисовка ИЛС линий
    qglColor(QColor(Qt::green));
    glLineWidth(5.0);
    glDisable(GL_BLEND);//Уберем прозрачность
    glTranslatef(width()/2.0,height()/2.0,0);
    glRotated(-cameraToThisObj->gamma,0.0,0.0,1.0);

    for(int i=-10;i<10;i++)
    {
        if(i==0)
            glLineWidth(5.0);
        else
            glLineWidth(2.0);

        double hILS=(cameraToThisObj->tan+GradToRadian(10*i))*(height()/camera()->fieldOfView());


        drawText((width()/2.0)-((width()/5.0)),height()/2.0+hILS,QString::number(-i*10));
        drawText((width()/2.0)+((width()/5.0)),height()/2.0+hILS,QString::number(-i*10));

        glBegin(GL_LINES);

            glVertex2i(-(width()/5.0),hILS);
            glVertex2i((width()/5.0),hILS);

        glEnd();
    }
    drawSymbol();

    glTranslatef(0,0,0);
    /////////////////////////////////////////
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
    /////////////////////////////////////////
}
//! отрисовка прицельной символики
void view3DArea::drawSymbol()
{
    Matrix3D_D m=signleCalcMatrix(cameraToThisObj);

    QVector<TVis> *list=net.getObjects();
    for(int i=0;i<list->size();i++)
    {
        TVis *solid=&((*list)[i]);
        if(solid!=cameraToThisObj)
        {
            Vector3D_D vecTarget(solid->xg,
                                solid->yg,
                                solid->zg);
            TAngle angle;
            earthToSGF(vecTarget,vecCameraInGeoSys,m,&angle);

            drawCircle(&angle,15,2);
            drawCross(&angle,15,2);
        }
    }
}
void view3DArea::gradToPixel(double xRad,
                             double yRad,
                             int &xPixel,
                             int &yPixel)
{
    xPixel=xRad*(height()/camera()->fieldOfView());
    yPixel=yRad*(height()/camera()->fieldOfView());
}
void view3DArea::drawStateLine()
{
    if(cameraToThisObj==0)
        drawText((width()/2)-20,height()-20,"Current Object="+QString::number(cameraToObjIndex));

    drawText((width()/2)-20,height()-40,"Time="+QString::number(globalTime));
}
void view3DArea::drawCross(TAngle* angle,int radius_,int width_)
{
    //! координаты центра
    int xCenter=0;
    int yCenter=0;
    glPointSize(width_);
    //! перевод
    TAngle an=limit->limitAngle(*angle,RadianToGrad(camera()->fieldOfView())/2.0);
    //! перевод градусов в пиксели
    gradToPixel(-an.sigma,
                -an.tau,
                xCenter,
                yCenter);
    qglColor(Qt::red);//цвет окружности
    //! отрисовка точки в центре окружности
    glBegin(GL_POINTS);
        glVertex2i(xCenter,yCenter);
        glEnd();
    //! толщина пикселей окружности
    glBegin(GL_LINES);
        glVertex2i(xCenter+radius_/2.0,yCenter);
        glVertex2i(xCenter-radius_/2.0,yCenter);

        glVertex2i(xCenter,yCenter+radius_/2.0);
        glVertex2i(xCenter,yCenter-radius_/2.0);
    glEnd();
}

void view3DArea::drawCircle(TAngle *angle,int radius_,int width_)
{
    int x=0;
    int y=radius_;
    int delta=1-1*radius_;
    int error=0;
    //! координаты центра
    int xCenter=0;
    int yCenter=0;
    glPointSize(width_);
    //! перевод
    TAngle an=limit->limitAngle(*angle,RadianToGrad(camera()->fieldOfView())/2.0);
    //! перевод градусов в пиксели
    gradToPixel(-an.sigma,
                -an.tau,
                xCenter,
                yCenter);
    qglColor(Qt::red);//цвет окружности
    //! отрисовка точки в центре окружности
    glBegin(GL_POINTS);
        glVertex2i(xCenter,yCenter);
        glEnd();
    //! толщина пикселей окружности
    glBegin(GL_POINTS);

    while(y>=0)
    {
        glVertex2i(xCenter+x,yCenter+y);
        glVertex2i(xCenter+x,yCenter-y);
        glVertex2i(xCenter-x,yCenter+y);
        glVertex2i(xCenter-x,yCenter-y);

        error= 2*(delta+y)-1;
        if(delta<0 && error<=0)
        {
            ++x;
            delta +=2*x+1;
            continue;
        }
        error = 2* (delta - x)-1;
        if(delta > 0 && error > 0)
        {
            --y;
            delta +=1-2*y;
            continue;
        }
        ++x;
        delta += 2*(x-y);
        --y;
    }
    glEnd();

}

//! отрисовка земли
void view3DArea::drawTerra()
{
    if(terrain->loadCompleate==true)
    {
        int step=1;

        /*glPushMatrix();
        //glEnable(GL_BLEND);
        //glEnable(GL_AUTO_NORMAL);
        //glEnable(GL_NORMALIZE);
        GLfloat pos[4]={0.0,5000.0,0.0,1.0};
        Lib3dsRgba a={0.2, 0.2, 0.2, 1.0};
        Lib3dsRgba d={0.8, 0.8, 0.8, 1.0};
        Lib3dsRgba s={1.0, 1.0, 1.0, 1.0};
        glLightfv (GL_LIGHT0, GL_AMBIENT, a);
        glLightfv (GL_LIGHT0, GL_DIFFUSE, d);
        glLightfv (GL_LIGHT0, GL_SPECULAR, s);

        glRotated (90.0, 3500.0,0.0, 0.0);
        glLightfv (GL_LIGHT0, GL_POSITION, pos);

        glEnable(GL_LIGHTING);

        glPopMatrix();*/

        glPushMatrix();
        glRotated(rotMap,0.0,1.0,0.0);
        glShadeModel(GL_SMOOTH);
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
        glBegin(GL_QUADS);

//        if(camera()->position().y>25000.0)
//            step=10;

//        else if(camera()->position().y<7000)
//            step=1;
//        else
//            step=limitMinMax(((int)floor(linearInterpolation(camera()->position().y,10,1,25000,7000))),1,10);

        for(int i=0;i<terrain->mapH->nrows-step;i+=step)
        {
            for(int j=0;j<terrain->mapH->ncols-step;j+=step)
            {


                QVector3D vec=terrain->mapH->value(j,i);
                QVector3D vec1=terrain->mapH->value(j,i+step);
                QVector3D vec1N=vec-vec1;

                QVector3D vec2=terrain->mapH->value(j+step,i);
                QVector3D vec2N=vec2-vec;
                QVector3D norm;norm.normal(vec2N,vec1N);

                if(vec.y()<100)
                     glColor3f(0.0f,0.0f,vec.y()/100);
                else
                    glColor3f(0.0f,vec.y()/1500.0,0.0);
                glVertex3f(vec.x()+offsetMapX,vec.y(),vec.z()+offsetMapZ);

                vec=terrain->mapH->value(j,i+step);

                if(vec.y()<100)
                    glColor3f(0.0f,0.0f,vec.y()/100);
                else
                    glColor3f(0.0f,vec.y()/1500.0,0.0);

                glVertex3f(vec.x()+offsetMapX,vec.y(),vec.z()+offsetMapZ);

                vec=terrain->mapH->value(j+step,i+step);
                if(vec.y()<100)
                      glColor3f(0.0f,0.0f,vec.y()/100);
                else
                    glColor3f(0.0f,vec.y()/1500.0,0.0);

                glVertex3f(vec.x()+offsetMapX,vec.y(),vec.z()+offsetMapZ);

                vec=terrain->mapH->value(j+step,i);
                if(vec.y()<100)
                     glColor3f(0.0f,0.0f,vec.y()/100);
                else
                    glColor3f(0.0f,vec.y()/1500.0,0.0);

                glVertex3f(vec.x()+offsetMapX,vec.y(),vec.z()+offsetMapZ);

            }
        }
        glEnd();
        glPopMatrix();
    }
}
//! отрисовка неба
void view3DArea::drawSky()
{

}
void view3DArea::keyReleaseEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Right: {horFilter->setFilterValue(0.0);   break;}
        case Qt::Key_Left:  {horFilter->setFilterValue(0.0);   break;}
        case Qt::Key_Up:    {verFilter->setFilterValue(0.0);   break;}
        case Qt::Key_Down:  {verFilter->setFilterValue(0.0);   break;}
        case Qt::Key_A:     {vecCameraInTargetSystem.z-=50;    break;}
        case Qt::Key_D:     {vecCameraInTargetSystem.z+=50;    break;}
        case Qt::Key_W:     {vecCameraInTargetSystem.y-=50;    break;}
        case Qt::Key_S:     {vecCameraInTargetSystem.y+=50;    break;}
        case Qt::Key_Plus:  {vecCameraInTargetSystem.x+=50;    break;}
        case Qt::Key_Minus:  {vecCameraInTargetSystem.x-=50;    break;}

        default:{QGLViewer::keyReleaseEvent(e);}
    };

}


void view3DArea::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Plus : {radiusScene*=2.0; break;}
    case Qt::Key_Minus: {radiusScene/=2.0; break;}
    case Qt::Key_A:     {break;}
    case Qt::Key_0    : {setCameraToObject(-1);break;}
    case Qt::Key_1    : {setCameraToObject(0);break;}
    case Qt::Key_2    : {setCameraToObject(1);break;}
    case Qt::Key_3    : {setCameraToObject(2);break;}
    case Qt::Key_4    : {setCameraToObject(3);break;}
    case Qt::Key_5    : {setCameraToObject(4);break;}
    case Qt::Key_6    : {setCameraToObject(5);break;}
    case Qt::Key_7    : {setCameraToObject(6);break;}
    case Qt::Key_8    : {setCameraToObject(7);break;}
    case Qt::Key_9    : {setCameraToObject(8);break;}
    case Qt::Key_Insert:{cameraToBack=true;break;}
    case Qt::Key_T:     {dteta_camera+=0.01;break;}
    case Qt::Key_G:     {dteta_camera-=0.01;break;}
    case Qt::Key_F:     {dpsi_camera+=0.01;break;}
    case Qt::Key_H:     {dpsi_camera-=0.01;break;}

    case Qt::Key_Right: {horFilter->setFilterValue(1.0);   break;}
    case Qt::Key_Left:  {horFilter->setFilterValue(-1.0);   break;}
    case Qt::Key_Up:    {verFilter->setFilterValue(-1.0);   break;}
    case Qt::Key_Down:  {verFilter->setFilterValue(1.0);   break;}
    case Qt::Key_I:
    {
       /* if(ils==true) ils=false;
        else ils=true;
        break;*/
    }

    case Qt::Key_S:
    {
        setting->show();
        break;
    }
    /*case Qt::Key_F1    :
        {
            fov+=M_PI/180;
            camera()->setFieldOfView(fov);
            break;
        }
    case Qt::Key_F2    :
        {
            fov-=M_PI/180;
            camera()->setFieldOfView(fov);
            break;
        }
    case Qt::Key_F3    :
        {
            hfov+=M_PI/180;
            camera()->setHorizontalFieldOfView(hfov);
            break;
        }
    case Qt::Key_F4    :
        {
            hfov-=M_PI/180;
            camera()->setHorizontalFieldOfView(hfov);
            break;
        }
    case Qt::Key_I:
        {
            if(ils==true) ils=false;
            else ils=true;
            break;
        }*/
    case Qt::Key_O:
        {
            //! открыть файл
            //openTXTFile();
            break;
        }
    /*case Qt::Key_G:
        {
            break;
        }*/
    case Qt::Key_Z:
        {
            //net.sendData(curTarget,7);
            break;
        }
    case Qt::Key_L    :
        {
            //terrain->openFileWithTerrain();
            //alignmentForm->show();
            //stopAnimation();
            //setFPSIsDisplayed(true);
            break;}
    default:
        {
            QGLViewer::keyPressEvent(e);}
    };
}
void view3DArea::loadFile(QString nameFile,Lib3dsFile **file3ds)
{
    //!установить период анимации
    //setAnimationPeriod(PERIOD_ANIMATION);
    //! отрисовка сетки
    setGridIsDrawn(false);
    //! отрисовка осей
    setAxisIsDrawn(false);
    //! Путь к 3D модели
    QString name=qApp->applicationDirPath()+nameFile;

    *file3ds = lib3ds_file_load(name.toLatin1().constData());

    if (!(*file3ds))
    {
        QMessageBox(QMessageBox::Information,"Information","3ds file not found");
        qWarning("Error : Unable to open file ");
        exit(1);
    }

    if ((*file3ds)->cameras) camera_name = (*file3ds)->cameras->name;
    else camera_name = NULL;

    lib3ds_file_eval((*file3ds),0);

    initScene(*file3ds);
}
void view3DArea::setCameraToObject(int num)
{
    cameraToBack=false;
    if(num==-1)
    {
        cameraToThisObj=0;
        cameraToObjIndex=-1;
    }

    if(num>=0)
    {
        QVector<TVis> *list=net.getObjects();

        if(num<list->size())
        {
            cameraToThisObj=&(*list)[num];
            cameraToObjIndex=num;
        }
        else
            cameraToThisObj=0;
            cameraToObjIndex=-1;
    }

}
void view3DArea::slotFov(double value)
{
    //fov=value;
    //camera()->setFieldOfView(fov*M_PI/180.0);
}
void view3DArea::slotReadRes()
{
    QSize s=this->size();

}
Matrix3D_D view3DArea::signleCalcMatrix(TVis *solid)
{
    Matrix3D_D matrixPsi;
    Matrix3D_D matrixGamma;
    Matrix3D_D matrixTan;
    //==================================================
    double psi=solid->psi;
    //double gamma=GradToRadian(0.0);
    double gamma=solid->gamma;
    double tan=solid->tan;

    matrixPsi.x[0][0]=cos(psi);
    matrixPsi.x[0][1]=0.0;
    matrixPsi.x[0][2]=-sin(psi);

    matrixPsi.x[1][0]=0.0;
    matrixPsi.x[1][1]=1.0;
    matrixPsi.x[1][2]=0.0;

    matrixPsi.x[2][0]=sin(psi);
    matrixPsi.x[2][1]=0.0;
    matrixPsi.x[2][2]=cos(psi);

    //===================================================
    matrixGamma.x[0][0]=1.0;
    matrixGamma.x[0][1]=0.0;
    matrixGamma.x[0][2]=0.0;

    matrixGamma.x[1][0]=0.0;
    matrixGamma.x[1][1]=cos(gamma);
    matrixGamma.x[1][2]=sin(gamma);

    matrixGamma.x[2][0]=0.0;
    matrixGamma.x[2][1]=-sin(gamma);
    matrixGamma.x[2][2]=cos(gamma);
    //==================================================
    matrixTan.x[0][0]=cos(tan);
    matrixTan.x[0][1]=sin(tan);
    matrixTan.x[0][2]=0.0;

    matrixTan.x[1][0]=-sin(tan);
    matrixTan.x[1][1]=cos(tan);
    matrixTan.x[1][2]=0.0;

    matrixTan.x[2][0]=0.0;
    matrixTan.x[2][1]=0.0;
    matrixTan.x[2][2]=1.0;
    //===================================================

    Matrix3D_D tempMatrix=matrixTan*matrixPsi;
    return matrixGamma*tempMatrix;
}
void view3DArea::cameraToObject()
{

    if(cameraToThisObj==0)
    {

        return;
    }
    Matrix3D_D matr=signleCalcMatrix(cameraToThisObj);
    Vector3D_D vec1(cameraToThisObj->xg,
                    cameraToThisObj->yg,
                    cameraToThisObj->zg);

    vecCameraInGeoSys=matr.transpose()*vecCameraInTargetSystem+vec1;
    camera()->setPosition(Vec(vecCameraInGeoSys.x,
                              vecCameraInGeoSys.y,
                              vecCameraInGeoSys.z));

    setSceneCenter(Vec(vecCameraInGeoSys.x,
                       0.0,
                       vecCameraInGeoSys.z));

    //camera()->showEntireScene();
    curGamma=   RadianToGrad(cameraToThisObj->gamma);
    curPsi=     RadianToGrad(cameraToThisObj->psi);
    curTeta=    RadianToGrad(cameraToThisObj->tan);
    curVy=      cameraToThisObj->vyg;
    curY=       cameraToThisObj->yg;
    curV=       cameraToThisObj->vc;
    curNya=     cameraToThisObj->nya;
    curAlfa=    RadianToGrad(cameraToThisObj->alfa);
    curFi=      RadianToGrad(cameraToThisObj->fi);
    curUnt=     RadianToGrad(cameraToThisObj->unt);
    curIdMan=   cameraToThisObj->idManevr;

    //! углы повортов камеры в радианах
    double tan_tar_radian=  cameraToThisObj->tan;
    double gamma_tar_radian=cameraToThisObj->gamma;
    double psi_tar_radian=  cameraToThisObj->psi;
    double d_psi_tar_rad=0.0;
    if(cameraToBack==true)      d_psi_tar_rad=M_PI;

    //! кватернион поворотов
    Quaternion qPI_2(   Vec(0.0, 1.0, 0.0),-M_PI/2.0);
    Quaternion qPsi(    Vec(0.0, 1.0, 0.0),psi_tar_radian+d_psi_tar_rad+dpsi_camera);
    Quaternion qTeta(   Vec(0.0, 0.0, 1.0),-gamma_tar_radian+dgamma_camera);
    Quaternion qGamma(  Vec(1.0, 0.0, 0.0),tan_tar_radian+dteta_camera);

    //! единичный кватернион
    Quaternion identity;
    camera()->frame()->setOrientation(identity);

    camera()->frame()->rotate(qPI_2);
    camera()->frame()->rotate(qPsi);
    camera()->frame()->rotate(qGamma);
    camera()->frame()->rotate(qTeta);
}

/*void view3DArea::setCameraToAircraft()
{
    if(airCraft.setBindingCamera==true)
    {
        //qDebug("x=%f\n",aircraft.x);
        //! положение камеры
        camera()->setPosition(Vec(aircraft.x,aircraft.y,aircraft.z));

        //! углы повортов камеры в радианах
        teta_radian=GradToRadian(aircraft.teta+teta0);
        gamma_radian=GradToRadian(aircraft.gamma+gamma0);
        psi_radian=KursToPsi(GradToRadian(aircraft.psi+psi0));

        //! кватернион поворотов
        Quaternion qPI_2(   Vec(0.0, 1.0, 0.0),-M_PI/2.0);
        Quaternion qPsi(    Vec(0.0, 1.0, 0.0),psi_radian);
        Quaternion qTeta(   Vec(0.0, 0.0, 1.0),-gamma_radian);
        Quaternion qGamma(  Vec(1.0, 0.0, 0.0),teta_radian);

        //! единичный кватернион
        Quaternion identity;
        camera()->frame()->setOrientation(identity);

        camera()->frame()->rotate(qPI_2);
        camera()->frame()->rotate(qPsi);
        camera()->frame()->rotate(qGamma);
        camera()->frame()->rotate(qTeta);

        //camera()->setFieldOfView(fov*M_PI/180.0);
        //camera()->setHorizontalFieldOfView(hfov);
    }
}
*/
void view3DArea::initScene(Lib3dsFile *file3ds)
{
    camera()->setFieldOfView(GradToRadian(45));
    camera()->setZNearCoefficient(0.0001);
    if (!file3ds)   return;

    // Lights
    // GLfloat amb[] = {0.5, 0.5, 0.5, 1.0};
    // GLfloat dif[] = {0.0, 0.0, 0.0, 0.0};
    // GLfloat spe[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat pos[] = {1.0, 1.0, 0.0, 0.0};
    int li=GL_LIGHT0;

    for (Lib3dsLight* l=file3ds->lights; l; l=l->next)
    {
        glEnable(li);
        glLightfv(li, GL_POSITION, pos);
    }
   /* Quaternion qPI_2( Vec(0.0, 1.0, 0.0),M_PI/2.0);
    camera()->frame()->transformOf(Vec(0.0, 0.0, 1.0));
    camera()->frame()->rotate(qPI_2);*/
    // Camera
    Lib3dsNode* c = lib3ds_file_node_by_name(file3ds, camera_name, LIB3DS_CAMERA_NODE);
    Lib3dsNode* t = lib3ds_file_node_by_name(file3ds, camera_name, LIB3DS_TARGET_NODE);

    if (!c || !t) return;

    camera()->setPosition(Vec(c->data.camera.pos));
    camera()->lookAt(Vec(t->data.target.pos));
    Vec up=camera()->frame()->transformOf(Vec(0.0, 0.0, 1.0));
    float angle=atan2(up.x, up.y);
    Quaternion q(Vec(0.0, 0.0, 1.0), c->data.camera.roll-angle);
    camera()->frame()->rotate(q);
    camera()->setFieldOfView(M_PI/180.0*c->data.camera.fov);
    camera()->setZNearCoefficient(0.0);

    setSceneRadius(radiusScene);
    setFPSIsDisplayed(true);

}

void view3DArea::renderNode(Lib3dsFile *file,Lib3dsNode *node)
{
    for (Lib3dsNode* p=node->childs; p!=0; p=p->next)
        renderNode(file,p);

    if (node->type == LIB3DS_OBJECT_NODE)
    {
        if (strcmp(node->name,"$$$DUMMY")==0)
            return;

        if (!node->user.d)
        {
            Lib3dsMesh *mesh=lib3ds_file_mesh_by_name(file, node->name);
            if (!mesh)
                return;

            node->user.d = glGenLists(1);
            glNewList(node->user.d, GL_COMPILE);

            Lib3dsVector *normalL = new Lib3dsVector[3*mesh->faces];

            Lib3dsMatrix M;

            lib3ds_matrix_copy(M, mesh->matrix);
            lib3ds_matrix_inv(M);

            glMultMatrixf(&M[0][0]);

            lib3ds_mesh_calculate_normals(mesh, normalL);

            for (unsigned int p=0; p<mesh->faces; ++p)
            {
                Lib3dsFace *f=&mesh->faceL[p];
                Lib3dsMaterial *mat=0;

                if (f->material[0])
                    mat=lib3ds_file_material_by_name(file, f->material);

                if (mat)
                {
                    static GLfloat a[4]={0,0,0,1};
                    float s;

                    glMaterialfv(GL_FRONT, GL_AMBIENT, a);//
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
                    //  glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);

                    s = pow(2, 10.0*mat->shininess);
                    if (s>128.0)
                        s=128.0;
                    glMaterialf(GL_FRONT, GL_SHININESS, s);
                }
                else
                {
                    Lib3dsRgba a={0.2, 0.2, 0.2, 1.0};
                    Lib3dsRgba d={0.8, 0.8, 0.8, 1.0};
                    //Lib3dsRgba s={1.0, 1.0, 1.0, 1.0};
                    glMaterialfv(GL_FRONT, GL_AMBIENT, a);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
                    //	glMaterialfv(GL_FRONT, GL_SPECULAR, s);
                }

                glBegin(GL_TRIANGLES);
                glNormal3fv(f->normal);
                for (int i=0; i<3; ++i)
                {
                    glNormal3fv(normalL[3*p+i]);
                    glVertex3fv(mesh->pointL[f->points[i]].pos);
                }
                glEnd();
            }
            delete[] normalL;
            glEndList();
        }
        if (node->user.d)
        {
            glPushMatrix();

            Lib3dsObjectData* d = &node->data.object;
            glMultMatrixf(&node->matrix[0][0]);
            glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
            glCallList(node->user.d);
            glPopMatrix();
        }
    }
}

void view3DArea::slotRes(int w,int h)
{
    resize(w,h);
}
void view3DArea::slotPosWindow(int x,int y)
{
    move(x,y);
}
