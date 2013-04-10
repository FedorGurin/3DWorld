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
#define SIZE_GRID 50
#define MAX_Y 20000.0
#define PERIOD_ANIMATION 50
//#define VERT_GRID

#define USE_3DMODEL


extern "C" { LIB3DSAPI void lib3ds_file_bounding_box(Lib3dsFile *fileAirCraft, Lib3dsVector min, Lib3dsVector max); }

view3DArea::view3DArea():QGLViewer()
{
    resize(800,600);
    cameraToThisObj=0;
    cameraToObjIndex=-1;
    cameraToBack=false;

    lam0=57.0;
    fi0=33.0;

    ils=true;
    radiusScene=20000.0;

    current_frame_aircraft=0.0;
    current_frame_target=0.0;

    camera_name=0;

    terrain=new View3DTerrain;
    alignmentForm=new AligmentForm;
    limit=new LimitPositionKAI;
    //viewILS=new ViewILS(&net);

    horFilter=new FilterAperiodic(0.0,1.0,PERIOD_ANIMATION/1000.0,1.0,1-1.0);
    verFilter=new FilterAperiodic(0.0,1.0,PERIOD_ANIMATION/1000.0,1.0,-1.0);

    camera()->setPosition(Vec(0.0,
                              2000.0,
                              0.0));

    vecCameraInTargetSystem=Vector3D_D::zero;
    vecCameraInTargetSystem.y=100;
    vecCameraInTargetSystem.x=-500;

    init();
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
    loadFile("./aircraft.3ds",  &(list3DObj[0].file));
    loadFile("./target.3ds",    &(list3DObj[1].file));
    loadFile("./x55.3ds",       &(list3DObj[2].file));
    loadFile("./WATER_T2.3ds",  &(list3DObj[3].file));
    loadFile("./bomb.3ds",      &(list3DObj[4].file));
    loadFile("./ofz30p.3ds",    &(list3DObj[5].file));
}

void view3DArea::init()
{
#ifdef USE_3DMODEL

    readAllModels();
    setAxisIsDrawn(false);
    setFPSIsDisplayed(true);
    //setFullScreen(true);
    setAnimationPeriod(PERIOD_ANIMATION);

#endif
#ifndef USE_3DMODEL

#endif
    QColor c(81,168,255);
    this->setBackgroundColor(c);
    startAnimation();
}

void view3DArea::draw()
{
    double d=20000.0;

    drawText(10,120,"HorFilter="+QString::number(horFilter->curValue()));
    drawText(10,140,"VerFilter="+QString::number(verFilter->curValue()));

    if(cameraToThisObj!=0)
    {
        drawText(width()-100,
                 10,
                 QString("camera to:")+QString::number(cameraToObjIndex));
    }else
    {
        drawText(width()-100,
                 10,
                 QString("camera to: no object"));
    }

    if(cameraToThisObj!=0)
    {
        d=6371000.0*atan(sqrt(cameraToThisObj->yg*(2*6371000.0+cameraToThisObj->yg))/6371000.0);
    }

    glDisable(GL_LIGHTING);
    setSceneRadius(d);
    ////////////////////////////////////////////////
    GLfloat fogColor[4];//= {0.1f, 0.1f, 0.5f, 1.0f}; // Цвет тумана

    QColor c(81,168,255);
    //QColor c(Qt::cyan);
    fogColor[0]=c.redF();
    fogColor[1]=c.greenF();
    fogColor[2]=c.blueF();
    fogColor[3]=1.0;

    glEnable(GL_FOG);               // Включает туман (GL_FOG)
    glFogi(GL_FOG_MODE, GL_LINEAR); // Выбираем тип тумана
    glFogfv(GL_FOG_COLOR, fogColor);// Устанавливаем цвет тумана
    glFogf(GL_FOG_DENSITY, 0.5f);   // Насколько густым будет туман
    glHint(GL_FOG_HINT, GL_NICEST); // Вспомогательная установка тумана
    glFogf(GL_FOG_START, d/2);      // Глубина, с которой начинается туман
    glFogf(GL_FOG_END, d);          // Глубина, где туман заканчивается.
    ///////////////////////////////////////////////////////////////
    glScalef(1.0,1.0,1.0);

    glBegin(GL_POINTS);
    qglColor(QColor(Qt::red));

    glEnd();

    glPushMatrix();
    ////////Нарисуем плоскость OXZ
    //glScalef(radiusScene,radiusScene,radiusScene);
    glScalef(d,d,d);
    //glDisable(GL_LIGHTING);//Отключим свет
    glBegin(GL_LINES);

    float stepGrid=2.0/SIZE_GRID;//Шаг сетки
    for(int i=0;i<=SIZE_GRID;i++)
    {
        /////////сетка при Z=0.0//////////////
        qglColor(QColor(170,200,160));
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
    qglColor(QColor(Qt::gray));
    glBegin(GL_QUADS);
    //qglColor(QColor(229,210,175,220));


    glVertex3f(1.0,0.0,-1.0);
    glVertex3f(-1.0,0.0,-1.0);
    glVertex3f(-1.0,0.0,1.0);
    glVertex3f(1.0,0.0,1.0);
    glEnd();

    glDisable(GL_BLEND);//Уберем прозрачность
    glPopMatrix();

    glLineWidth(1.0);
    glPointSize(4.0);

#ifdef USE_3DMODEL
    drawSolidObjects();
#endif
    //! отрисовка подстилающей поверхности
    drawTerra();
    //! отрисовка ИЛС
    if(ils==true)
        drawILS();

    //////////////////////////
    glDisable(GL_LIGHTING);
    //glEnable(GL_BLEND);
    //! Полная яркость, 50% альфа (НОВОЕ)
    glColor4f(1.0f,1.0f,1.0f,0.7f);
    //! Функция смешивания для непрозрачности
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);

    //if(grid==true) drawGrid();

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
    drawLight(GL_LIGHT0);
}

void view3DArea::animate()
{
    horFilter->refresh();
    verFilter->refresh();

    cameraToObject();

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


/*
//! отрисовка сетки
void view3DArea::drawGrid()
{
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
    //gluOrtho2D(0, width(), height(), 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /////////////////////////////////////////
    //! отрисовка ИЛС линий
    qglColor(QColor(Qt::darkGray));
    glLineWidth(1.0);
    glDisable(GL_BLEND);//Уберем прозрачность
    //glTranslatef(width()/2.0,height()/2.0,0);
    //glRotated(-aircraft.gamma,0.0,0.0,1.0);

    //! горизонтальные линии
    for(int i=0;i<100;i++)
    {
        glBegin(GL_LINES);
        glVertex2i((width()/100)*i,height()/100);
        glVertex2i((width()/100)*i,height());
        glEnd();
    }
    /////////////////////////////////////////
    //! вертикальные линии
    //! горизонтальные линии
    for(int i=0;i<100;i++)
    {
        glBegin(GL_LINES);
        glVertex2i(width()/100,(height()/100)*i);
        glVertex2i(width(),(height()/100)*i);
        glEnd();
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
    /////////////////////////////////////////
}*/
T3DObject* view3DArea::findObjByCode(int code)
{
    for(int i=0;i<list3DObj.size();i++)
    {
        if(list3DObj[i].code==code)
            return &list3DObj[i];
    }
    return 0;
}

void view3DArea::drawSolidObjects()
{
    Lib3dsFile* file3DS=0;
    QVector<TVis> *list=net.getObjects();
    for(int i=0;i<list->size();i++)
    {
        TVis *solid=&((*list)[i]);

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
                            double psi_grad,
                            double gamma_grad,
                            double tan_grad)
{
    glPushMatrix();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(pos_x,
                 pos_y,
                 pos_z);

    glRotated(RadianToGrad(psi_grad),0.0,1.0,0.0);
    glRotated(RadianToGrad(tan_grad),0.0,0.0,1.0);
    glRotated(RadianToGrad(gamma_grad),1.0,0.0,0.0);

    glColor4f(1.0f,1.0f,1.0f,1.0f);
    for (Lib3dsNode* p=obj->nodes; p!=0; p=p->next)
        renderNode(obj,p);

    glPopMatrix();
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
            drawCircle(&angle,15);
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

void view3DArea::drawCircle(TAngle *angle,int radius_)
{
    int x=0;
    int y=radius_;
    int delta=2-2*radius_;
    int error=0;
    //! координаты центра
    int xCenter=0,yCenter=0;
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
    glPointSize(5);
        glVertex2i(xCenter,yCenter);
    glEnd;
    //! толщина пикселей окружности
    glBegin(GL_POINTS);
    glPointSize(1);
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

        glPushMatrix();
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

        glRotated (90.0, 1.0,0.0, 0.0);
        glLightfv (GL_LIGHT0, GL_POSITION, pos);

        glEnable(GL_LIGHTING);

        glPopMatrix();

        glPushMatrix();
       // glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
        qglColor(QColor(229,210,175,220));
        //glPointSize(1.0);

        for(int i=0;i<terrain->mapH->nrows-step;i+=step)
        {
            for(int j=0;j<terrain->mapH->ncols-step;j+=step)
            {
                glBegin(GL_QUADS);

                QVector3D vec=terrain->mapH->value(j,i);
                QVector3D vec1=terrain->mapH->value(j,i+step);
                QVector3D vec1N=vec-vec1;

                QVector3D vec2=terrain->mapH->value(j+step,i);
                QVector3D vec2N=vec2-vec;
                QVector3D norm;norm.normal(vec2N,vec1N);

                Lib3dsRgba a={0.2, 0.2, 0.2, 1.0};
                Lib3dsRgba d={0.8, 0.8, 0.8, 1.0};
                Lib3dsRgba s={1.0, 1.0, 1.0, 1.0};

                glMaterialfv(GL_FRONT, GL_AMBIENT, a);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
                glMaterialfv(GL_FRONT, GL_SPECULAR, s);

                glNormal3f(norm.x(),norm.y(),norm.z());
                glVertex3f(vec.x(),vec.y(),vec.z());


                glMaterialfv(GL_FRONT, GL_AMBIENT, a);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
                glMaterialfv(GL_FRONT, GL_SPECULAR, s);

                vec=terrain->mapH->value(j,i+step);
                glVertex3f(vec.x(),vec.y(),vec.z());

                glMaterialfv(GL_FRONT, GL_AMBIENT, a);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
                glMaterialfv(GL_FRONT, GL_SPECULAR, s);

                vec=terrain->mapH->value(j+step,i+step);
                glVertex3f(vec.x(),vec.y(),vec.z());

                glMaterialfv(GL_FRONT, GL_AMBIENT, a);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
                glMaterialfv(GL_FRONT, GL_SPECULAR, s);

                vec=terrain->mapH->value(j+step,i);
                glVertex3f(vec.x(),vec.y(),vec.z());
                glEnd();

            }

        }

        //glDisable(GL_BLEND);//Уберем прозрачность
        glDisable(GL_AUTO_NORMAL);
        glDisable(GL_NORMALIZE);
        glPopMatrix();

    }
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

        default:{QGLViewer::keyPressEvent(e);}
    };

}


void view3DArea::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    case Qt::Key_Plus : {radiusScene*=2.0; break;}
    case Qt::Key_Minus: {radiusScene/=2.0; break;}
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

    case Qt::Key_Right: {horFilter->setFilterValue(1.0);   break;}
    case Qt::Key_Left:  {horFilter->setFilterValue(-1.0);   break;}
    case Qt::Key_Up:    {verFilter->setFilterValue(-1.0);   break;}
    case Qt::Key_Down:  {verFilter->setFilterValue(1.0);   break;}
    case Qt::Key_I:
    {
        if(ils==true) ils=false;
        else ils=true;
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
    case Qt::Key_G:
        {
            //net.sendData(curTarget,8);
            /*if(grid==true) grid=false;
            else grid=true;*/
            break;
        }
    case Qt::Key_Z:
        {
            //net.sendData(curTarget,7);
            break;
        }
    case Qt::Key_L    :
        {
            terrain->openFileWithTerrain();
            //alignmentForm->show();
            //stopAnimation();
            //setFPSIsDisplayed(true);
            break;}
    }
    //QGLViewer::keyPressEvent(e);
}
void view3DArea::loadFile(QString nameFile,Lib3dsFile **file3ds)
{
    //!установить период анимации
    setAnimationPeriod(PERIOD_ANIMATION);
    //! отрисовка сетки
    setGridIsDrawn(true);
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
    alignmentForm->setCurrentResolution(s.width(),s.height());
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
    Quaternion qPsi(    Vec(0.0, 1.0, 0.0),psi_tar_radian+d_psi_tar_rad);
    Quaternion qTeta(   Vec(0.0, 0.0, 1.0),-gamma_tar_radian);
    Quaternion qGamma(  Vec(1.0, 0.0, 0.0),tan_tar_radian);

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
