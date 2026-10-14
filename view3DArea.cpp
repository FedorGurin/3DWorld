#include "view3DArea.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>

#include <QOpenGLFunctions_4_0_Core>

#include "QGLViewer/manipulatedCameraFrame.h"
#include <QMessageBox>
#include <QKeyEvent>
#include <QApplication>
#include <QVector3D>
#include <QDataStream>
#include <QByteArray>

#include "dataPath.h"
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include "terrainColor.h"


using namespace qglviewer;
#define SIZE_GRID 100
#define MAX_Y 20000.0
//#define PERIOD_ANIMATION 50
#define PERIOD_ANIMATION 1000
//! период записи точек траекторий сетевых объектов, с
#define TRAJECTORY_PERIOD 0.5
//#define VERT_GRID

#define USE_3DMODEL
#define FOG_OFF 1
//! максимальное кол-во точек в траектории одного объекта
#define MAX_TRAJECTORY_POINTS 20000
//! минимальная длина траектории при большом числе объектов
#define MIN_TRAJECTORY_POINTS 100
//! общее кол-во точек траекторий всех объектов
#define TRAJECTORY_POINTS_BUDGET 400000


view3DArea::view3DArea():QGLViewer()
{
    glFunc = 0;
    terrainList        = 0;
    terrainListVersion = -1;
    terrainStreaming   = false;
    terrainImagery     = true;
    terrainTexture     = 0;
    terrainListTex     = 0;
    terrainTextureVersion = -1;
    outerGroundColor[0] = 0.3f; outerGroundColor[1] = 0.5f; outerGroundColor[2] = 0.25f;
    outerGroundHeight   = 0.0f;
    curIdMan = 0;
    resize(800,600);
    cameraToThisObj = nullptr;
    cameraToObjIndex= -1;
    cameraObjId     = -1;
    staleTimeoutSec = 10;
    playPosMs       = 0.0;
    playPaused      = false;
    sceneClock.start();
    cameraToBack    = false;
    txtScale        = 1.0f;
    txtSpeed        = 0.0;
    dataFromTXTFile = false;

    dialog  = new QFileDialog(this);
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setNameFilter(("File with Data (*.txt)"));
    dialog->setDirectory(qApp->applicationDirPath());

    file    = new QFile;
    stream  = new QTextStream(file);
    setting = new FormSettings;
    connect(dialog,SIGNAL(accepted()),this,SLOT(slotAccepted()));

    lam0    = gradToRad * 57.0;
    fi0     = gradToRad * 33.0;
    rotMap  = 0.0;

    offsetMapX = 0.0;
    offsetMapZ = 0.0;

    dFog        = 0.5;

    globalCount = 0;
    ils         = true;
    vertLine    = false;
    // признаки отрисовки тумана
    fog     =  false    ;
    info    =  false    ;
    terra   =  true     ;
    sky     =  true     ;
    grid    =  false    ;
    cameraUp=  false    ;

    radiusScene     = 26000.0;
    globalTime      = 0.0;
    globalTimeDebug = 0.0;
    multTime        = 1.0;
    // частота отрисовки по умолчанию 60 Гц
    dt = ((1.0/60)*1000.0);
    trajectoryTimer = 0.0;
    //dt=((1.0/freq)*1000.0);

    // доп. поворот камеры
    dpsi_camera     = 0.0;
    dteta_camera    = 0.0;
    dgamma_camera   = 0.0;

    rows.clear();
    trs.clear();

    current_frame_aircraft=0.0;
    current_frame_target=0.0;

    //camera_name=0;

    // указатель на рельеф
    terrain=new View3DTerrain;
    // подгрузка рельефа по ходу полета
    streamer = new TerrainStreamer(this);

    // ограничения на КАИ
    limit=new LimitPositionKAI;

    horFilter=new FilterAperiodic(0.0,0.8,PERIOD_ANIMATION/1000.0,1.0,-1.0);
    verFilter=new FilterAperiodic(0.0,0.8,PERIOD_ANIMATION/1000.0,1.0,-1.0);

    camera()->setPosition(Vec(0.0,
                              2000.0,
                              0.0));

    vecCameraInTargetSystem=glm::vec3(-1500,100,0);

    connect(setting,SIGNAL(fog(bool)),          this,SLOT(setFog(bool)));
    connect(setting,SIGNAL(grid(bool)),         this,SLOT(setGrid(bool)));
    connect(setting,SIGNAL(sky(bool)),          this,SLOT(setSky(bool)));

    connect(setting,SIGNAL(info(bool)),         this,SLOT(setInfo(bool)));
    connect(setting,SIGNAL(ils(bool)),          this,SLOT(setILS(bool)));
    connect(setting,SIGNAL(vertical(bool)),     this,SLOT(setVertical(bool)));
    connect(setting,SIGNAL(loadLand()),         terrain,SLOT(openFileWithTerrain()));
    connect(setting,SIGNAL(loadTerrainOnline(double,double,double)),
            terrain,SLOT(loadFromInternet(double,double,double)));
    connect(terrain,SIGNAL(status(QString)),    setting,SLOT(setTerrainStatus(QString)));
    connect(streamer,SIGNAL(status(QString)),   setting,SLOT(setTerrainStatus(QString)));
    connect(setting,SIGNAL(smoothing(bool)),    this,SLOT(setSmoothing(bool)));
    connect(setting,SIGNAL(imagery(bool)),      this,SLOT(setImagery(bool)));
    connect(setting,SIGNAL(staleTimeout(int)),  this,SLOT(setStaleTimeout(int)));
    connect(setting,SIGNAL(recording(bool)),    this,SLOT(setRecording(bool)));
    connect(setting,SIGNAL(playRecording()),    this,SLOT(openPlayback()));
    connect(setting,SIGNAL(terrainStreaming(bool,double,double)),
            this,SLOT(setTerrainStreaming(bool,double,double)));
    connect(setting,SIGNAL(loadDataFlight()),   this,SLOT(openTXTFile()));
    connect(setting,SIGNAL(terra(bool)),        this,SLOT(setTerra(bool)));
    connect(setting,SIGNAL(freq(int)),          this,SLOT(setFreq(int)));
    connect(setting,SIGNAL(multTime(double)),   this,SLOT(setMultTime(double)));
    connect(setting,SIGNAL(densityFog(double)), this,SLOT(setDensityFog(double)));
    connect(setting,SIGNAL(sizeScene(double)),  this,SLOT(setSizeScene(double)));
    connect(setting,SIGNAL(offsetMapX(double)), this,SLOT(setOffsetMapX(double)));
    connect(setting,SIGNAL(offsetMapZ(double)), this,SLOT(setOffsetMapZ(double)));
    connect(setting,SIGNAL(rotateMap(double)),  this,SLOT(setRotateMap(double)));
    connect(&net,SIGNAL(resetTrajectory()),this,SLOT(slotReset()));
}
void view3DArea::slotReset()
{
    for(auto it = trs.begin(); it != trs.end(); ++it)
        it->clear();
}
void view3DArea::appendTrajectoryPoint(int id,float x,float y,float z,int r,int g,int b)
{
    // общий бюджет точек на все объекты: чем больше объектов,
    // тем короче хвост у каждого (память и время отрисовки не растут)
    const int objects  = qMax(1, net.getVehicleObj()->size());
    const int maxPoints = qBound(MIN_TRAJECTORY_POINTS,
                                 TRAJECTORY_POINTS_BUDGET / objects,
                                 MAX_TRAJECTORY_POINTS);
    TTrajectory &tr = trs[id];
    if(tr.size() >= maxPoints)
    {
        // удаляем сразу 10% старых точек, чтобы не сдвигать массив на каждой точке
        const int n = tr.size() - maxPoints + qMax(1, maxPoints / 10);
        tr.xyz.remove(0, n * 3);
        tr.rgb.remove(0, n * 3);
    }
    tr.xyz << x << y << z;
    tr.rgb << (unsigned char)r << (unsigned char)g << (unsigned char)b;
}
void view3DArea::setGrid(bool value)
{
    grid=value;
}
void view3DArea::setSky(bool value)
{
    sky=value;
}
void view3DArea::setImagery(bool value)
{
    streamer->setImagery(value);
    terrainImagery = value;
}
void view3DArea::setSmoothing(bool value)
{
    net.smoothing = value;
}
void view3DArea::setStaleTimeout(int seconds)
{
    staleTimeoutSec = seconds;
}
void view3DArea::setRecording(bool value)
{
    if(value)
    {
        QString dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/3DWorld";
        QDir().mkpath(dir);
        startRecording(dir + "/session_" +
                       QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".rec");
    }else
    {
        net.stopRecording();
        setting->setNetStatus(QString::fromUtf8("Запись остановлена"));
    }
}
void view3DArea::startRecording(const QString &fileName)
{
    if(net.startRecording(fileName, sceneClock.elapsed()))
        setting->setNetStatus(QString::fromUtf8("Запись: %1").arg(fileName));
    else
        setting->setNetStatus(QString::fromUtf8("Не удалось открыть %1").arg(fileName));
    setting->setRecordingChecked(net.isRecording());
}
void view3DArea::openPlayback()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/3DWorld";
    QString fileName = QFileDialog::getOpenFileName(this, QString::fromUtf8("Запись сеанса"),
                                                    dir, "3DWorld session (*.rec)");
    if(fileName.isEmpty() == false)
        startPlayback(fileName);
}
void view3DArea::startPlayback(const QString &fileName)
{
    dataFromTXTFile = false;
    slotReset();
    trs.clear();
    if(net.loadPlayback(fileName))
    {
        playPosMs  = 0.0;
        playPaused = false;
        setting->setNetStatus(QString::fromUtf8("Воспроизведение: %1 (%2 с). P - пауза, [ ] - перемотка")
                              .arg(QFileInfo(fileName).fileName())
                              .arg(net.playbackDuration() / 1000.0, 0, 'f', 1));
    }else
        setting->setNetStatus(QString::fromUtf8("Не удалось прочитать запись %1").arg(fileName));
}
void view3DArea::setTerrainStreaming(bool value, double lat, double lon)
{
    terrainStreaming = value;
    if(value)
        streamer->setOrigin(lat, lon);
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
    dFog=value;
}
void view3DArea::setSizeScene(double value)
{
    radiusScene=value;
}
void view3DArea::setFreq(int freq)
{
    if(freq <= 0)
        return;
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

    rows.clear();
    for(long i=0;i<list.size();i++)
    {
        TDataRow row;
        QStringList tempList=list[i].trimmed().split("\t",QString::SkipEmptyParts);
        // в строке должно быть 9 столбцов: код, масштаб, время, x, y, z, psi, tan, gamma
        if(tempList.size() < 9)
            continue;
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
        row.psi=    gradToRadian(tempList[6].toFloat());
        row.tan=    gradToRadian(tempList[7].toFloat());
        row.gamma=  gradToRadian(tempList[8].toFloat());
        rows.push_back(row);
    }
}

void view3DArea::slotAccepted()
{
    QStringList list=dialog->selectedFiles();
    if(list.isEmpty() == false)
        loadTXTFile(list[0]);
}
void view3DArea::loadTXTFile(const QString &fileName)
{
    file->close();
    file->setFileName(fileName);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("3DWorld"),
                             tr("Can't open file %1").arg(fileName));
        return;
    }
    stream->setDevice(file);

    parserTXTFile();
    file->close();
    // для интерполяции нужно минимум две точки
    if(rows.size() < 2)
    {
        QMessageBox::warning(this, tr("3DWorld"),
                             tr("File does not contain enough data rows"));
        dataFromTXTFile = false;
        return;
    }

    globalCount     = 0;
    dataFromTXTFile = true;
    net.setSizeObj(1);
    refreshCameraObject();
    slotReset();
    globalTime=rows[0].time;
}
void view3DArea::statusLoad(bool loadout)
{
    if(loadout == false)
        std::cout << "fail" << std::endl;
    else
        std::cout << "ok\n" << std::endl;
}
void view3DArea::readAllModels()
{
    //! описание моделей: код объекта, файл, реальная длина модели, м
    //! (0 - размеры в файле уже заданы в метрах)
    struct TModelInfo
    {
        int code;
        const char *file;
        float length;
    };
    const TModelInfo models[] = {
        {105, "libs/cone.obj",  0.0f},  // конус
        {101, "libs/f15.obj",  19.4f},  // F-15
        {102, "libs/f15.obj",  19.4f},  // цель (F-15)
        {106, "libs/il76.obj",  0.0f},  // Ил-76
    };

    list3DObj.clear();
    for(const TModelInfo &info : models)
    {
        T3DObject obj;
        obj.code            = info.code;
        obj.displayList     = 0;
        obj.transparentList = 0;
        obj.scale           = 1.0f;
        obj.radius          = 1.0f;
        list3DObj.push_back(obj);
    }
    for(int i = 0; i < list3DObj.size(); i++)
    {
        T3DObject &obj = list3DObj[i];
        bool loadout = obj.file.LoadFile(findDataFile(models[i].file).toStdString());
        statusLoad(loadout);
        // радиус описанной сферы - для отсечения невидимых объектов
        for(const objl::Vertex &v : obj.file.LoadedVertices)
            obj.radius = qMax(obj.radius, glm::length(v.Position));
        if(loadout == false || models[i].length <= 0.0f)
            continue;
        // масштаб по длине модели вдоль продольной оси X
        float minX = 0.0f, maxX = 0.0f;
        bool first = true;
        for(const objl::Vertex &v : obj.file.LoadedVertices)
        {
            if(first || v.Position.x < minX) minX = v.Position.x;
            if(first || v.Position.x > maxX) maxX = v.Position.x;
            first = false;
        }
        if(maxX - minX > 1e-6f)
            obj.scale = models[i].length / (maxX - minX);
    }
}

void view3DArea::init()
{
    glFunc = context()->versionFunctions<QOpenGLFunctions_3_2_Compatibility>();
    glFunc->initializeOpenGLFunctions();

    readAllModels();
    // текстура облаков
    skyView.init();
#ifdef USE_3DMODEL

    setAxisIsDrawn(false);
    setFPSIsDisplayed(true);
    //setFullScreen(true);
#endif
    // период анимации (раньше не устанавливался - работал период QGLViewer 40 мс)
    setAnimationPeriod(dt);
    frameClock.start();
    // открыть рельеф по умолчанию
    // (если карта уже не запрошена из командной строки)
    if(terrain->isRequested() == false)
        terrain->openTerrainMap("defaultTerrain.asc");
    this->setBackgroundColor(QColor(81,168,255));
    setSceneRadius(radiusScene);
    //установить период анимации
    //setAnimationPeriod(PERIOD_ANIMATION);
    startAnimation();
    camera()->setFieldOfView(gradToRadian(45));
    camera()->setZNearCoefficient(0.0001);
    // начальное положение камеры: обзор сцены сверху-сбоку
    camera()->setPosition(Vec(-radiusScene*0.6, radiusScene*0.4, radiusScene*0.6));
    camera()->setUpVector(Vec(0.0, 1.0, 0.0));
    camera()->lookAt(Vec(0.0, 0.0, 0.0));

//    GLenum li=GL_LIGHT1;
//    GLfloat pos[] = {1.0, 1.0, 0.0, 0.0};

//    glEnable(li);
//    glLightfv(li, GL_POSITION, pos);

}

void view3DArea::draw()
{
    double d=0;
    glDisable(GL_LIGHTING);


    // время сцены: реальные часы или позиция воспроизведения записи
    qint64 nowMs;
    if(net.isPlaying())
    {
        nowMs = qint64(playPosMs);
        net.advancePlayback(nowMs);
        if(dataFromTXTFile == false)
            globalTime = net.synch_time;
    }else
    {
        nowMs = sceneClock.elapsed();
        if(net.checkDatagrams(nowMs)==true && dataFromTXTFile == false)
            globalTime = net.synch_time;
    }
    if(dataFromTXTFile == false)
    {
        // плавное движение между пакетами
        net.updateDisplay(nowMs);
        // удаление объектов, от которых давно нет данных
        const QList<int> removed = net.removeStale(nowMs, qint64(staleTimeoutSec) * 1000);
        for(int id : removed)
            trs.remove(id);
    }
    // список объектов мог измениться - обновим объект, к которому привязана камера
    refreshCameraObject();
    // если модель передает географическую точку начала координат (рад),
    // подгружаемый рельеф привязывается к ней
    if(terrainStreaming == true && net.getVehicleObj()->isEmpty() == false)
    {
        const TSendVehicleVisSimple &first = net.getVehicleObj()->first();
        if(first.fi0_geo != 0.0 || first.lam0_geo != 0.0)
            streamer->setOrigin(radianToGrad(first.fi0_geo), radianToGrad(first.lam0_geo));
    }


    if(cameraToThisObj != nullptr)
    {
        d = 6371000.0*atan(sqrt(cameraToThisObj->cg_y*(2*6371000.0+cameraToThisObj->cg_y))/6371000.0);
        setSceneRadius(d);
    }else
    {
        setSceneRadius(radiusScene);
        d = radiusScene;
    }
    ////////////////////////////////////////////////
    GLfloat fogColor[4];//= {0.1f, 0.1f, 0.5f, 1.0f}; // Цвет тумана

    QColor c(81,168,255);
    //QColor c(Qt::cyan);
    fogColor[0] = c.redF();
    fogColor[1] = c.greenF();
    fogColor[2] = c.blueF();
    fogColor[3] = 1.0;
    if(sky == true)
    {
        // при отрисовке неба туман совпадает с цветом горизонта
        glm::vec3 horizon = skyView.skyColor(0.0f, camera()->position().y);
        fogColor[0] = horizon.r;
        fogColor[1] = horizon.g;
        fogColor[2] = horizon.b;
    }

    if(fog == true)
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

    // отрисовка неба - фон для всей сцены
    if(sky == true)
        drawSky();
    // сначала трехмерная сцена с проверкой глубины, затем экранная символика
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_BLEND);
    // отрисовка подстилающей поверхности
    if(terra == true)
        drawTerra();
    // отрисовка сетки
    if(grid == true)
        drawGrid(d);
    // отрисовка траектории
    drawTrajectory();
#ifdef USE_3DMODEL
    // отрисовка трехмерных объектов
    drawSolidObjects();
#endif
    drawVecLine();
    // облака полупрозрачные - рисуются после непрозрачных объектов
    if(sky == true)
        drawClouds();
    // отрисовка ИЛС
    if(ils == true)
        drawILS();
    //////////////////////////
    glDisable(GL_LIGHTING);
    //glEnable(GL_BLEND);
    // Полная яркость, 50% альфа (НОВОЕ)
    glColor4f(1.0f,1.0f,1.0f,0.7f);
    // Функция смешивания для непрозрачности
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);

    drawStateLine();
    if(info == true)
    {
        drawText(10,60,"[+] zoom UP");
        drawText(10,80,"[-] zoom DOWN");
        drawText(10,100,"Current Scale="+QString::number(d)+" meters");
        drawText(10,150,"Psi="+QString::number(curPsi));
        drawText(100,150,"Gamma="+QString::number(curGamma));
        drawText(190,150,"Teta="+QString::number(curTeta));

        drawText(10,170,"W.x="+QString::number(curWx));
        drawText(100,170,"W.y="+QString::number(curWy));
        drawText(190,170,"W.z="+QString::number(curWz));

        drawText(10,190,"Vx_c="+QString::number(curVx_c));
        drawText(100,190,"Vy_c="+QString::number(curVy_c));
        drawText(190,190,"Vz_c="+QString::number(curVz_c));

        drawText(10,210,"Vx_g="+QString::number(curVx_g));
        drawText(100,210,"Vy_g="+QString::number(curVy_g));
        drawText(190,210,"Vz_g="+QString::number(curVz_g));

        drawText(10,230,"x="+QString::number(curX));
        drawText(100,230,"y="+QString::number(curY));
        drawText(190,230,"z="+QString::number(curZ));

        drawText(10,250,"V="+QString::number(curV));
        drawText(10,270,"alfa="+QString::number(curAlfa));

        drawText(10,290, "nxa="+QString::number(curNxa));
        drawText(100,290,"nya="+QString::number(curNya));
        drawText(180,290,"nza="+QString::number(curNza));

        drawText(10,310,"fi="+QString::number(curFi));
        drawText(180,310,"unt="+QString::number(curUnt));

        drawText(10,120,"HorFilter="+QString::number(horFilter->curValue()));
        drawText(10,140,"VerFilter="+QString::number(verFilter->curValue()));

        QString nameMan="Unknown";


        switch(curIdMan)
        {
            case 1000:{nameMan="Homing";break;}
            case 1   :{nameMan="left_turn";break;}
            case 2   :{nameMan="right_turn";break;}
            case 5   :{nameMan="halfcross";break;}
            case 7   :{nameMan="gorka";break;}
            case 9   :{nameMan="prividenie";break;}
            case 10  :{nameMan="dive";break;}
            case 12  :{nameMan="battle_allign";break;}
            case 14  :{nameMan="none";break;}

        };
        drawText(10,350,"man="+nameMan);

        drawText(10,390,"Vy_c="+QString::number(curVy_c));

        drawText(90,270,"beta="+QString::number(curBeta));
    }
    //drawLight(GL_LIGHT0);

}
void view3DArea::drawGrid(double d)
{
    glPushMatrix();
    //glDisable(GL_DEPTH_TEST);
    ////////Нарисуем плоскость OXZ
    //glScalef(radiusScene,radiusScene,radiusScene);
    glScalef(d,d,d);
    glEnable(GL_DEPTH_TEST);
    glColor3f(1.0f,1.0f,1.0f);
    //glDisable(GL_LIGHTING);//Отключим свет
    glBegin(GL_LINES);

        float stepGrid=2.0/SIZE_GRID;//Шаг сетки
        for(int i=0;i<=SIZE_GRID;i++)
        {
            /////////сетка при Z=0.0//////////////
            //qglColor(QColor(170,200,160,255));
            //qglColor(QColor(11,11,11));

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
        ///            /////glEnable(GL_BLEND);
        ///    //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR);
        /*    qglColor(QColor(Qt::gray));
        ///    glBegin(GL_QUADS);
        ///    //qglColor(QColor(229,210,175,220));


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
void view3DArea::drawTrajectory()
{





    glPointSize(3.0);
    if(dataFromTXTFile==true)
    {
        //! отрисовка точек
        glBegin(GL_POINTS);
        //! если прочитали файл
        for(long i=0;i<rows.size();i++)
        {
            glColor3ub(rows[i].r,rows[i].g,rows[i].b);
            glVertex3f(rows[i].x_g,rows[i].y_g,rows[i].z_g);
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
    }else
    {
        // отрисовка массивами вершин - быстро даже для сотен объектов
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        for(const TTrajectory &tr : trs)
        {
            if(tr.size() == 0)
                continue;
            glVertexPointer(3, GL_FLOAT, 0, tr.xyz.constData());
            glColorPointer(3, GL_UNSIGNED_BYTE, 0, tr.rgb.constData());
            glDrawArrays(GL_POINTS, 0, tr.size());
            // проекция траектории на землю (без рельефа)
            if(terra == false)
            {
                glPushMatrix();
                glScalef(1.0f, 0.0f, 1.0f);
                glDrawArrays(GL_POINTS, 0, tr.size());
                glPopMatrix();
            }
        }
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        if(vertLine==true)
        {
            //! отрисовка вертикальных линий
            glBegin(GL_LINES);
            for(const TTrajectory &tr : trs)
            {
                for(int j = 0; j < tr.size(); j += 5)
                {
                    const float *p = tr.xyz.constData() + j * 3;
                    glVertex3f(p[0], 0.0f, p[2]);
                    glVertex3f(p[0], p[1], p[2]);
                }
            }
            glEnd();
        }
    }
    glColor3f(1.0f,1.0f,1.0f);
}

void view3DArea::animate()
{
    // время сцены идет по реальным часам: не зависит от частоты кадров
    // и не отстает, если кадр отрисовывался дольше периода анимации
    double frameSec = frameClock.restart() * 0.001;
    frameSec = qBound(0.0, frameSec, 0.25);
    //! счетчик времени, сек
    globalTime=globalTime+frameSec*multTime;
    // продвижение воспроизведения записи (множитель времени - скорость)
    if(net.isPlaying() && playPaused == false)
    {
        playPosMs += frameSec * multTime * 1000.0;
        if(playPosMs >= net.playbackDuration())
        {
            playPosMs  = net.playbackDuration();
            playPaused = true;
        }
    }
    globalTimeDebug=globalTimeDebug+frameSec*multTime;
    //! если данные загружены из файла
    if(dataFromTXTFile==true && rows.size() >= 2)
    {
        // по окончании данных начинаем воспроизведение сначала
        if(globalTime>=(rows.back().time) || globalTime<rows.first().time)
        {
            globalCount=0;
            globalTime=rows.first().time;
            slotReset();
        }

        int index = searchTimeInterval(globalTime);
        // индекс правой границы интервала: [index-1; index]
        if(index < 1)
            index = (index == 0) ? 1 : rows.size() - 1;
        //! записываем параметры
        QVector<TSendVehicleVisSimple> *list = net.getVehicleObj();
        TSendVehicleVisSimple *solid = &((*list)[0]);
        solid->code = rows[index].code;
        txtScale    = rows[index].scale > 0.0f ? rows[index].scale : 1.0f;
        // скорость на текущем интервале файла
        {
            const TDataRow &r0 = rows[index-1], &r1 = rows[index];
            const double dtRow = r1.time - r0.time;
            const glm::vec3 d(r1.x_g - r0.x_g, r1.y_g - r0.y_g, r1.z_g - r0.z_g);
            txtSpeed = dtRow > 1e-9 ? glm::length(d) / dtRow : 0.0;
        }


        //! эти параметры нужно прогнать через блок интерполяции
        solid->cg_x = linearInterpolation(globalTime,
                                      rows[index].x_g,
                                      rows[index-1].x_g,
                                      rows[index].time,
                                      rows[index-1].time);

        solid->cg_y = linearInterpolation(globalTime,
                                      rows[index].y_g,
                                      rows[index-1].y_g,
                                      rows[index].time,
                                      rows[index-1].time);

        solid->cg_z = linearInterpolation(globalTime,
                                      rows[index].z_g,
                                      rows[index-1].z_g,
                                      rows[index].time,
                                      rows[index-1].time);

        solid->psi = linearInterpolation(globalTime,
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
        appendTrajectoryPoint(0,
                              solid->cg_x,
                              solid->cg_y,
                              solid->cg_z,
                              rows[index].r,
                              rows[index].g,
                              rows[index].b);
    }else
    {
        // точки траекторий сетевых объектов - раз в TRAJECTORY_PERIOD секунд
        trajectoryTimer += frameSec;
        if(trajectoryTimer >= TRAJECTORY_PERIOD)
        {
            trajectoryTimer = 0.0;
            QVector<TSendVehicleVisSimple> *list = net.getVehicleObj();
            for(const auto &i:*list)
            {
                //! координаты объектов и цвет траектории
                appendTrajectoryPoint(i.id,i.cg_x,i.cg_y,i.cg_z,80,223,72);
            }
        }
    }

    horFilter->refresh();
    verFilter->refresh();
    refreshCameraObject();
    //! установить камеру на объект
    cameraToObject();


    // отправка органов управления
    if(horFilter->delta()>10e-3 || verFilter->delta()>10e-3)
    {
        if(cameraToThisObj != nullptr)
        {
            float x = horFilter->curValue();
            float y = verFilter->curValue();

            stick.gamma_rus = x;
            stick.tan_rus   = y;
            stick.rud       = 0;
            stick.idObj     = cameraToThisObj->id;
            net.sendData(CONTROL_STICK,(char*)&stick,sizeof(TControlStick));
        }
    }
}

T3DObject* view3DArea::findObjByCode(int code)
{
    for(int i = 0;i < list3DObj.size();i++)
    {
        if(list3DObj[i].code == code)
            return &list3DObj[i];
    }
    return nullptr;
}
int view3DArea::searchTimeInterval(double time)
{
    for(int i = 0;i < rows.size();i++)
    {
        if(rows[i].time > time)
        {
           return i;
        }
    }
    return -1;
}
void view3DArea::drawArrayObjects()
{
//    QVector<TSendArrayVisSimple> *list = net.getArrayObj();
//    for(auto i: *list)
//    {
//        TSendArrayVisSimple *array = &((*list)[i]);
//        TSendVehicleVisSimple *solidCenter = &(list->first());
//        objl::Loader *model = 0;
//        if(findObjByCode(solid->code) != 0)
//            model = &(findObjByCode(solid->code)->file);
//        if(model != 0)
//        {
//        for (size_t j = 0; j<i.num; j++)
//        {
//            convertSphereToDekart(solidCenter->lam0_geo,
//                                  solidCenter->fi0_geo,
//                                  solid->lam_geo,
//                                  solid->fi_geo,
//                                  solid->cg_x,
//                                  solid->cg_z);
//            drawObject(model,
//                       solid->cg_x,
//                       solid->cg_y,
//                       solid->cg_z,
//                       solid->psi,
//                       solid->gamma,
//                       solid->tan);
//        }
//        }
//    }
}

void view3DArea::drawTestSolidObj()
{
    objl::Loader *model = nullptr;
    TSendVehicleVisSimple solid;
    memset((void*)&solid,0,sizeof(solid));
    if( findObjByCode(101) != nullptr)
        model = &(findObjByCode(101)->file);
    if(model == nullptr)
        return;

//    drawObject(model,
//               solid.cg_x,
//               solid.cg_y,
//               solid.cg_z,
//               solid.psi,
//               solid.gamma,
//               solid.tan);
}
bool view3DArea::objectVisible(const glm::vec3 &p, float r, float &pixelRadius)
{
    const Vec c = camera()->cameraCoordinatesOf(Vec(p.x, p.y, p.z));
    // камера смотрит вдоль -z
    const double depth = -c.z;
    if(depth < -r)
        return false;
    const double tanV = tan(camera()->fieldOfView() / 2.0);
    const double tanH = tan(camera()->horizontalFieldOfView() / 2.0);
    const double d = qMax(depth, 1e-3);
    if(fabs(c.x) - r > d * tanH || fabs(c.y) - r > d * tanV)
        return false;
    pixelRadius = float(r / d * (height() / 2.0) / tanV);
    return true;
}
void view3DArea::drawSolidObjects()
{
    QVector<TSendVehicleVisSimple> *list = net.getVehicleObj();
    if(list->isEmpty())
        return;
    // уровни отображения: крупные на экране объекты - полной моделью
    // (не более MAX_FULL_MODELS ближайших), мелкие - метками-точками
    const int   MAX_FULL_MODELS = 64;
    const float FULL_MODEL_PIXELS = 6.0f;
    struct TCandidate {int idx; float pixels; float scale;};
    QVector<TCandidate> full;
    // метки: x, y, z, размер точки
    QVector<float> marks;
    const TSendVehicleVisSimple *solidCenter = &(list->first());
    for(int idx = 0; idx < list->size(); idx++)
    {
        TSendVehicleVisSimple &i = (*list)[idx];
        // если переданы географические координаты - пересчитываем их
        // в декартовые относительно первого объекта
        if(i.lam_geo != 0.0 || i.fi_geo != 0.0)
        {
            convertSphereToDekart(solidCenter->lam0_geo,
                                  solidCenter->fi0_geo,
                                  i.lam_geo,
                                  i.fi_geo,
                                  i.cg_x,
                                  i.cg_z);
        }
        T3DObject *model = findObjByCode(i.code);
        const bool hasModel = model != nullptr && model->file.LoadedMeshes.empty() == false;

        // при воспроизведении файла учитываем масштаб из файла
        float scale = hasModel ? model->scale : 1.0f;
        if(dataFromTXTFile == true && idx == 0)
            scale *= txtScale;
        const float r = hasModel ? model->radius * scale : 10.0f;
        const glm::vec3 p(i.cg_x, i.cg_y, i.cg_z);

        float pixelRadius = 0.0f;
        if(objectVisible(p, r, pixelRadius) == false)
            continue;
        if(hasModel && pixelRadius >= FULL_MODEL_PIXELS)
        {
            full.append({idx, pixelRadius, scale});
            continue;
        }
        // объект, к которому привязана камера, изнутри не рисуем меткой
        if(idx == cameraToObjIndex && hasModel == false)
            continue;
        marks << p.x << p.y << p.z << qBound(3.0f, pixelRadius * 1.5f, 9.0f);
    }
    // ближайшие (самые крупные на экране) - полными моделями, остальные - метками
    std::sort(full.begin(), full.end(),
              [](const TCandidate &a, const TCandidate &b) {return a.pixels > b.pixels;});
    for(int k = 0; k < full.size(); k++)
    {
        const TSendVehicleVisSimple &i = (*list)[full[k].idx];
        if(k >= MAX_FULL_MODELS)
        {
            marks << i.cg_x << i.cg_y << i.cg_z << 9.0f;
            continue;
        }
        drawObject(findObjByCode(i.code),
                   i.cg_x,
                   i.cg_y,
                   i.cg_z,
                   i.psi,
                   i.gamma,
                   i.tan,
                   full[k].scale);
    }
    if(marks.isEmpty() == false)
    {
        glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT | GL_CURRENT_BIT);
        glDisable(GL_LIGHTING);
        glEnable(GL_POINT_SMOOTH);
        glColor3f(1.0f, 0.9f, 0.3f);
        // точки группируются по размеру (glPointSize нельзя менять внутри glBegin)
        for(int size = 3; size <= 9; size += 2)
        {
            glPointSize(float(size));
            glBegin(GL_POINTS);
            for(int k = 0; k + 3 < marks.size(); k += 4)
            {
                const int sz = int(marks[k + 3]);
                if(sz >= size && sz < size + 2)
                    glVertex3f(marks[k], marks[k + 1], marks[k + 2]);
            }
            glEnd();
        }
        glPopAttrib();
    }
}

void view3DArea::compileObject(T3DObject *obj)
{
    // если модель содержит несколько уровней детализации (части с именами
    // lod0, lod1, ...), рисуем только самый детальный - иначе они
    // накладываются друг на друга
    int minLod = -1;
    for(const objl::Mesh &mesh : obj->file.LoadedMeshes)
    {
        QString name = QString::fromStdString(mesh.MeshName).toLower();
        if(name.startsWith("lod"))
        {
            int lod = name.mid(3, 1).toInt();
            if(minLod < 0 || lod < minLod)
                minLod = lod;
        }
    }
    // непрозрачные части - в первый список, прозрачные (d < 1) - во второй
    for(int pass = 0; pass < 2; pass++)
    {
        GLuint list = glGenLists(1);
        if(pass == 0)
            obj->displayList = list;
        else
            obj->transparentList = list;

        glNewList(list, GL_COMPILE);
        for(const objl::Mesh &curMesh : obj->file.LoadedMeshes)
        {
            QString name = QString::fromStdString(curMesh.MeshName).toLower();
            if(minLod >= 0 && name.startsWith("lod") && name.mid(3, 1).toInt() != minLod)
                continue;

            const objl::Material &mat = curMesh.MeshMaterial;
            // d = 0 - значение по умолчанию, если прозрачность не задана в файле
            const float alpha = (mat.d > 0.0f && mat.d < 1.0f) ? mat.d : 1.0f;
            if((alpha < 1.0f) != (pass == 1))
                continue;

            // glMaterialfv ожидает 4 компоненты цвета (RGBA);
            // фоновый цвет берем из Kd: экспорт Blender задает Ka = 1,
            // из-за чего модель выглядит белой
            const GLfloat ka[] = {mat.Kd.x, mat.Kd.y, mat.Kd.z, alpha};
            const GLfloat kd[] = {mat.Kd.x, mat.Kd.y, mat.Kd.z, alpha};
            const GLfloat ks[] = {mat.Ks.x, mat.Ks.y, mat.Ks.z, 1.0f};
            // показатель блеска: без него блик заливает всю освещенную сторону
            const GLfloat shininess = mat.Ns > 0.0f ? qMin(mat.Ns, 128.0f) : 32.0f;
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  ka);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  kd);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
            glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, shininess);

            glBegin(GL_TRIANGLES);
            for (size_t j = 0; j + 2 < curMesh.Indices.size(); j += 3)
            {
                for(size_t k = 0; k < 3; k++)
                {
                    const objl::Vertex &v = curMesh.Vertices[curMesh.Indices[j + k]];
                    glNormal3f(v.Normal.x, v.Normal.y, v.Normal.z);
                    glVertex3f(v.Position.x, v.Position.y, v.Position.z);
                }
            }
            glEnd();
        }
        glEndList();
    }
}
void view3DArea::drawObject(T3DObject *obj,
                            double pos_x,
                            double pos_y,
                            double pos_z,
                            double psi_rad,
                            double gamma_rad,
                            double tan_rad,
                            float scale)
{
    glPushMatrix();
    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    // освещаем обе стороны граней: у части моделей нормали направлены внутрь
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    // направленный источник света - солнце
    const glm::vec3 sun = skyView.sunDirection();
    GLfloat pos[] = {sun.x, sun.y, sun.z, 0.0f};
    GLfloat ambLight[]  = {0.35f, 0.35f, 0.38f, 1.0f};
    GLfloat difLight[]  = {0.85f, 0.85f, 0.80f, 1.0f};
    GLfloat specLight[] = {0.6f, 0.6f, 0.6f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, difLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specLight);

    glTranslatef(pos_x,
                 pos_y,
                 pos_z);

    glRotated(radToGrad * psi_rad,0.0,1.0,0.0);
    glRotated(radToGrad * tan_rad,0.0,0.0,1.0);
    glRotated(radToGrad * gamma_rad,1.0,0.0,0.0);

    glScalef(scale, scale, scale);
    // при масштабировании нормали нужно нормировать заново
    glEnable(GL_NORMALIZE);

    // геометрия модели не меняется - формируем списки отображения один раз
    if(obj->displayList == 0)
        compileObject(obj);
    glCallList(obj->displayList);

    // прозрачные части (остекление) - с полупрозрачностью, без записи глубины
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glCallList(obj->transparentList);

    glPopAttrib();
    glPopMatrix();
}
//! отрисовка ИЛС (HUD)
void view3DArea::drawILS()
{
    // HUD имеет смысл только для камеры, привязанной к объекту
    if(cameraToThisObj == nullptr || cameraUp == true)
        return;

    THudData data;
    data.psi      = cameraToThisObj->psi;
    data.tan      = cameraToThisObj->tan;
    data.gamma    = cameraToThisObj->gamma;
    data.alfa     = cameraToThisObj->alfa;
    data.beta     = cameraToThisObj->beta;
    data.v        = cameraToThisObj->vc;
    data.vx       = cameraToThisObj->vc_x;
    data.vy       = cameraToThisObj->vc_y;
    data.vz       = cameraToThisObj->vc_z;
    data.altitude = cameraToThisObj->cg_y;
    data.ny       = cameraToThisObj->ny;
    data.position = glm::vec3(cameraToThisObj->cg_x,
                              cameraToThisObj->cg_y,
                              cameraToThisObj->cg_z);
    // при воспроизведении файла скорость считаем по точкам файла
    if(dataFromTXTFile == true)
        data.v = txtSpeed;

    // цели - все остальные объекты
    QVector<THudTarget> targets;
    QVector<TSendVehicleVisSimple> *list = net.getVehicleObj();
    for(const auto &i : *list)
    {
        if(i.id == cameraToThisObj->id)
            continue;
        THudTarget t;
        t.id       = i.id;
        t.position = glm::vec3(i.cg_x, i.cg_y, i.cg_z);
        targets.push_back(t);
    }
    hud.draw(this, data, targets);
}
void view3DArea::drawStateLine()
{
    QString prefix;
    if(cameraToObjIndex == 0)
        prefix=tr("our aircraft");
    else if(cameraToObjIndex>0)
        prefix=tr("target #")+QString::number(cameraToObjIndex);
    else
        prefix=tr("no select");
    if(cameraToThisObj != nullptr)
        prefix += QString(" (id %1)").arg(cameraToThisObj->id);
    prefix += QString(", objects: %1").arg(net.getVehicleObj()->size());
    if(net.isRecording())
        prefix += "  [REC]";
    if(net.isPlaying())
        prefix += QString("  [%1 %2 / %3 s]")
                .arg(playPaused ? "||" : ">")
                .arg(playPosMs / 1000.0, 0, 'f', 1)
                .arg(net.playbackDuration() / 1000.0, 0, 'f', 1);
    drawText((width()/2)-20,height()-20,"Current Object="+prefix);

    drawText((width()/2)-20,height()-40,"Time="+QString::number(globalTime));
    // подпись источника спутниковых снимков (условие лицензии)
    if(terra == true && ((terrainStreaming && streamer->hasImagery()) ||
                         (terrainStreaming == false && terrainTexture != 0 && terrainImagery)))
    {
        QFont small;
        small.setPointSize(7);
        drawText(4, height() - 4, streamer->imageryAttribution, small);
    }
    drawText((width()/2)-20,height()-10,"Time="+QString::number(globalTimeDebug));
}
void view3DArea::drawVecLine()
{
    if(net.getVecObj()->isEmpty())
        return;
    for(int i = 1; i < net.getVecObj()->size();i++)
    {
        glm::vec3 vec, vec_1;
        vec = (*net.getVecObj())[i];
        vec_1 = (*net.getVecObj())[i -1 ];
        glBegin(GL_LINES);
        glVertex3f(vec.x, vec.y,vec.z);
        glVertex3f(vec_1.x, vec_1.y,vec_1.z);
        glEnd();
    }
}
// отрисовка земли
void view3DArea::drawTerra()
{
    if(terrainStreaming == false && terrain->loadCompleate==false)
        return;

    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_FOG_BIT);
    // воздушная дымка: дальний рельеф растворяется в цвете горизонта
    // (если туман не включен в настройках)
    if(sky == true && fog == false)
    {
        const double h = qMax(100.0, double(camera()->position().y));
        // дальность видимого горизонта для данной высоты
        const double horizon = sqrt(2.0 * 6371000.0 * h);
        const glm::vec3 hc = skyView.skyColor(0.0f, camera()->position().y);
        const GLfloat hazeColor[4] = {hc.r, hc.g, hc.b, 1.0f};
        glEnable(GL_FOG);
        glFogi(GL_FOG_MODE, GL_LINEAR);
        glFogfv(GL_FOG_COLOR, hazeColor);
        glFogf(GL_FOG_START, 0.05 * horizon);
        glFogf(GL_FOG_END, 1.6 * horizon);
    }
    glPushMatrix();
    glRotated(rotMap,0.0,1.0,0.0);
    glTranslatef(offsetMapX,0.0,offsetMapZ);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

    // освещение солнцем, цвет вершин задает материал
    const glm::vec3 sun = skyView.sunDirection();
    GLfloat pos[]       = {sun.x, sun.y, sun.z, 0.0f};
    GLfloat ambLight[]  = {0.38f, 0.40f, 0.45f, 1.0f};
    GLfloat difLight[]  = {0.85f, 0.83f, 0.76f, 1.0f};
    GLfloat noSpec[]    = {0.0f, 0.0f, 0.0f, 1.0f};
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  difLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, noSpec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, noSpec);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    if(terrainStreaming == true)
    {
        // подгрузка рельефа вокруг камеры: точка фокуса в координатах карты
        // (обратное преобразование к повороту и смещению карты)
        const Vec cp = camera()->position();
        const double a = -rotMap * M_PI / 180.0;
        const double lx =  cp.x * cos(a) + cp.z * sin(a) - offsetMapX;
        const double lz = -cp.x * sin(a) + cp.z * cos(a) - offsetMapZ;
        streamer->update(lx, lz);
        streamer->draw();
        // земля за подгруженной областью
        const bool isWater = streamer->waterFraction() > 0.5f;
        terrainColor(isWater ? 0.0f : 150.0f, 1.0f, outerGroundColor);
        outerGroundHeight = isWater ? 0.0f : streamer->minHeight() - 50.0f;
    }else
    {
        // рельеф не меняется между кадрами - формируем список отображения
        // заново только после загрузки новой карты
        if(terrainList == 0 || terrainListVersion != terrain->version)
        {
            terrainListVersion = terrain->version;

            TASCII_MAP *map = terrain->mapH;
            // земля за картой: вода, если край карты в основном водный
            {
                int water = 0, count = 0;
                double sumH = 0.0;
                for(int i = 0; i < map->nrows; i++)
                {
                    for(int j = 0; j < map->ncols; j += (i == 0 || i == map->nrows - 1) ? 1 : map->ncols - 1)
                    {
                        const double h = map->value(j, i).y();
                        if(h < 3.0) water++;
                        sumH += h;
                        count++;
                    }
                }
                const bool isWater = water * 2 > count;
                terrainColor(isWater ? 0.0f : float(sumH / qMax(count, 1)), 1.0f, outerGroundColor);
                outerGroundHeight = isWater ? 0.0f : float(map->minHeight) - 30.0f;
            }
            // два варианта: окрашенный по высоте и со спутниковым снимком
            for(int textured = 0; textured < 2; textured++)
            {
                GLuint &list = textured ? terrainListTex : terrainList;
                if(list == 0)
                    list = glGenLists(1);
                glNewList(list, GL_COMPILE);
                for(int i=0;i<map->nrows-1;i++)
                {
                    glBegin(GL_TRIANGLE_STRIP);
                    for(int j=0;j<map->ncols;j++)
                    {
                        for(int k = 0; k < 2; k++)
                        {
                            const QVector3D v = map->value(j, i + k);
                            const QVector3D n = map->normal(j, i + k);
                            if(textured)
                            {
                                // снимок - регулярная сетка по широте и долготе, как и карта
                                glColor3f(1.0f, 1.0f, 1.0f);
                                glTexCoord2f(float(j) / (map->ncols - 1), float(i + k) / (map->nrows - 1));
                            }else
                            {
                                float c[3];
                                terrainColor(v.y(), n.y(), c);
                                glColor3fv(c);
                            }
                            glNormal3f(n.x(), n.y(), n.z());
                            glVertex3f(v.x(), v.y(), v.z());
                        }
                    }
                    glEnd();
                }
                glEndList();
            }
            // текстура снимка
            if(terrainTexture != 0)
                glDeleteTextures(1, &terrainTexture);
            terrainTexture = 0;
            if(map->texture.isNull() == false)
            {
                const QImage img = map->texture.convertToFormat(QImage::Format_RGBA8888);
                glGenTextures(1, &terrainTexture);
                glBindTexture(GL_TEXTURE_2D, terrainTexture);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);   // GL_CLAMP_TO_EDGE
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, 0x8191, GL_TRUE);             // GL_GENERATE_MIPMAP
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0,
                             GL_RGBA, GL_UNSIGNED_BYTE, img.constBits());
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }
        if(terrainImagery && terrainTexture != 0)
        {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, terrainTexture);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glCallList(terrainListTex);
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
        }else
            glCallList(terrainList);
    }
    glPopMatrix();

    // за пределами карты продолжаем землю плоскостью до горизонта,
    // чуть ниже самой низкой точки карты, чтобы не было наложения
    {
        const float r = camera()->zFar() * 0.9f;
        const Vec cp  = camera()->position();
        const float y = outerGroundHeight;
        glColor3fv(outerGroundColor);
        glNormal3f(0.0f, 1.0f, 0.0f);
        // кольца вокруг камеры: туман считается в вершинах, поэтому
        // плоскость разбивается, чтобы дымка нарастала с расстоянием плавно
        const int rings = 24, slices = 48;
        for(int i = 0; i < rings; i++)
        {
            const float r0 = r * powf(float(i) / rings, 2.0f);
            const float r1 = r * powf(float(i + 1) / rings, 2.0f);
            glBegin(GL_QUAD_STRIP);
            for(int j = 0; j <= slices; j++)
            {
                const float a = 2.0f * float(M_PI) * j / slices;
                const float ca = cosf(a), sa = sinf(a);
                glVertex3f(cp.x + r0 * ca, y, cp.z + r0 * sa);
                glVertex3f(cp.x + r1 * ca, y, cp.z + r1 * sa);
            }
            glEnd();
        }
    }
    glPopAttrib();
}
//! отрисовка неба
void view3DArea::drawSky()
{
    // купол должен лежать между ближней и дальней плоскостями отсечения
    const float zNear = camera()->zNear();
    const float zFar  = camera()->zFar();
    const float radius = qMax(zNear * 4.0f, zFar * 0.5f);
    const Vec pos = camera()->position();
    skyView.drawDome(glm::vec3(pos.x, pos.y, pos.z), radius);
}
//! отрисовка облаков
void view3DArea::drawClouds()
{
    const float zFar   = camera()->zFar();
    const float radius = qMin(zFar * 0.8f, 80000.0f);
    const Vec pos = camera()->position();
    skyView.drawClouds(glm::vec3(pos.x, pos.y, pos.z), radius, globalTimeDebug);
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
        case Qt::Key_Plus:
        {
            if(cameraUp==true)
                vecCameraInTargetSystem.y+=250;
            else
                vecCameraInTargetSystem.x+=50;
            break;
        }
        case Qt::Key_Minus:
        {
            if(cameraUp==true)
                vecCameraInTargetSystem.y-=250;
            else
                vecCameraInTargetSystem.x-=50;

            break;
        }

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
    // управление воспроизведением записи
    case Qt::Key_P:
    {
        if(net.isPlaying())
        {
            if(playPaused && playPosMs >= net.playbackDuration())
                playPosMs = 0.0;
            playPaused = !playPaused;
        }
        break;
    }
    case Qt::Key_BracketLeft:
    case Qt::Key_BracketRight:
    {
        if(net.isPlaying())
        {
            playPosMs += (e->key() == Qt::Key_BracketRight ? 10000.0 : -10000.0);
            playPosMs  = qBound(0.0, playPosMs, double(net.playbackDuration()));
        }
        break;
    }
    case Qt::Key_Home:
    {
        if(net.isPlaying())
            playPosMs = 0.0;
        break;
    }
    // перебор объектов по кругу (для любого их числа)
    case Qt::Key_PageDown:
    case Qt::Key_PageUp:
    {
        const int n = net.getVehicleObj()->size();
        if(n > 0)
        {
            int next = cameraToObjIndex + (e->key() == Qt::Key_PageDown ? 1 : -1);
            next = (next % n + n) % n;
            setCameraToObject(next);
        }
        break;
    }
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
        // включение/выключение HUD
        ils = !ils;
        break;
    }
    case Qt::Key_U:
    {
        if(cameraUp==false)
        {
            cameraUp=true;
            vecCameraInTargetSystem.y=30000.0;
            ils=false;
        }
        else
        {
            cameraUp=false;
            ils=true;
            vecCameraInTargetSystem=glm::vec3(-1500,100,0);
        }
        break;
    }
    case Qt::Key_Z:
    {
        setting->show();
        break;
    }
    case Qt::Key_K:
    {
        sky = !sky;
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
    case Qt::Key_S:
        {
            break;
        }
    case Qt::Key_Q:
    {

        dpsi_camera=0.0;
        dteta_camera=0.0;
        dgamma_camera=0.0;

        vecCameraInTargetSystem=glm::vec3(-1500,100,0);

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
//void view3DArea::loadFile(QString nameFile,Lib3dsFile **file3ds)
//{

//    //! отрисовка сетки
//    setGridIsDrawn(false);
//    //! отрисовка осей
//    setAxisIsDrawn(false);
//    //! Путь к 3D модели
//    QString name=qApp->applicationDirPath()+nameFile;

//    *file3ds = lib3ds_file_open(name.toLatin1().constData());

//    if (!(*file3ds))
//    {
//        QMessageBox(QMessageBox::Information,"Information","3ds file not found");
//        qWarning("Error : Unable to open file ");
//        exit(1);
//    }

////    if ((*file3ds)->cameras)
////        camera_name = (*(*file3ds)->cameras)->name;
////    else
////        camera_name = NULL;

//    lib3ds_file_eval((*file3ds),0);

//    initScene(*file3ds);
//}
void view3DArea::setCameraToObject(int num)
{
    cameraToBack=false;
    // камера привязывается к идентификатору объекта: номер объекта в списке
    // меняется при удалении "замолчавших" объектов
    QVector<TSendVehicleVisSimple> *list = net.getVehicleObj();
    if(num >= 0 && num < list->size())
        cameraObjId = (*list)[num].id;
    else
        cameraObjId = -1;
    refreshCameraObject();
}
void view3DArea::refreshCameraObject()
{
    // указатель на элемент QVector становится недействительным при изменении
    // списка, поэтому он каждый раз заново ищется по идентификатору
    QVector<TSendVehicleVisSimple> *list = net.getVehicleObj();
    const int idx = cameraObjId >= 0 ? net.testFlightObjInList(cameraObjId) : -1;
    if(idx >= 0 && idx < list->size() && (*list)[idx].id == cameraObjId)
    {
        cameraToObjIndex = idx;
        cameraToThisObj  = &(*list)[idx];
    }else
    {
        cameraToObjIndex = -1;
        cameraToThisObj  = nullptr;
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
glm::mat3 view3DArea::signleCalcMatrixPsi(TSendVehicleVisSimple *solid)
{
    double psi=solid->psi;

    glm::mat3 matrixPsi1;

    matrixPsi1[0][0]=cos(psi);
    matrixPsi1[0][1]=0.0;
    matrixPsi1[0][2]=-sin(psi);

    matrixPsi1[1][0]=0.0;
    matrixPsi1[1][1]=1.0;
    matrixPsi1[1][2]=0.0;

    matrixPsi1[2][0]=sin(psi);
    matrixPsi1[2][1]=0.0;
    matrixPsi1[2][2]=cos(psi);

    return matrixPsi1;
}

glm::mat3 view3DArea::signleCalcMatrix(TSendVehicleVisSimple *solid)
{
    double psi=solid->psi;
    double gamma=solid->gamma;
    double tan=solid->tan;

    glm::mat3 matrixPsi1;
    glm::mat3 matrixGamma1;
    glm::mat3 matrixTan1;

    matrixPsi1[0][0]=cos(psi);
    matrixPsi1[0][1]=0.0;
    matrixPsi1[0][2]=-sin(psi);

    matrixPsi1[1][0]=0.0;
    matrixPsi1[1][1]=1.0;
    matrixPsi1[1][2]=0.0;

    matrixPsi1[2][0]=sin(psi);
    matrixPsi1[2][1]=0.0;
    matrixPsi1[2][2]=cos(psi);

    //===================================================
    matrixGamma1[0][0]=1.0;
    matrixGamma1[0][1]=0.0;
    matrixGamma1[0][2]=0.0;

    matrixGamma1[1][0]=0.0;
    matrixGamma1[1][1]=cos(gamma);
    matrixGamma1[1][2]=sin(gamma);

    matrixGamma1[2][0]=0.0;
    matrixGamma1[2][1]=-sin(gamma);
    matrixGamma1[2][2]=cos(gamma);
    //==================================================
    matrixTan1[0][0]=cos(tan);
    matrixTan1[0][1]=sin(tan);
    matrixTan1[0][2]=0.0;

    matrixTan1[1][0]=-sin(tan);
    matrixTan1[1][1]=cos(tan);
    matrixTan1[1][2]=0.0;

    matrixTan1[2][0]=0.0;
    matrixTan1[2][1]=0.0;
    matrixTan1[2][2]=1.0;

    glm::mat3 tM=matrixPsi1*matrixTan1;
    glm::mat3 vec=tM*matrixGamma1;

    return vec;
}
void view3DArea::cameraToObject()
{
    if(cameraToThisObj == nullptr)
        return;

    glm::mat3 matr=signleCalcMatrix(cameraToThisObj);
    glm::vec3 vec1(cameraToThisObj->cg_x,
                   cameraToThisObj->cg_y,
                   cameraToThisObj->cg_z);

    if(cameraUp==true)
    {
        vecCameraInTargetSystem.x=0.0;
        //vecCameraInTargetSystem.y=30000.0;
        vecCameraInTargetSystem.z=0.0;

        matr=signleCalcMatrixPsi(cameraToThisObj);
    }
    glm::vec3 vecInObj=vecCameraInTargetSystem;
    vecCameraInGeoSys=vec1+(vecInObj*glm::transpose(matr));


    camera()->setPosition(Vec(vecCameraInGeoSys.x,
                              vecCameraInGeoSys.y,
                              vecCameraInGeoSys.z));

    setSceneCenter(Vec(vecCameraInGeoSys.x,
                       0.0,
                       vecCameraInGeoSys.z));

    curGamma    = radToGrad * cameraToThisObj->gamma;
    curPsi      = radToGrad * cameraToThisObj->psi;
    curTeta     = radToGrad * cameraToThisObj->tan;

    curAlfa=    radToGrad * cameraToThisObj->alfa;
    curBeta=    radToGrad * cameraToThisObj->beta;
    curVx_g=      cameraToThisObj->vg_x;
    curVy_g=      cameraToThisObj->vg_y;
    curVz_g=      cameraToThisObj->vg_z;

    curVx_c=    cameraToThisObj->vc_x;
    curVy_c=    cameraToThisObj->vc_y;
    curVz_c=    cameraToThisObj->vc_z;

    curNxa=     cameraToThisObj->nx;
    curNya=     cameraToThisObj->ny;
    curNza=     cameraToThisObj->nz;

    curWx=     radianToGrad(cameraToThisObj->omega_x);
    curWy=     radianToGrad(cameraToThisObj->omega_y);
    curWz=     radianToGrad(cameraToThisObj->omega_z);


    curV = cameraToThisObj->vc;
    curY = cameraToThisObj->cg_y;
    curX = cameraToThisObj->cg_x;
    curZ = cameraToThisObj->cg_z;

    curFi= radianToGrad(cameraToThisObj->fi);
    curUnt= radianToGrad(cameraToThisObj->unt);

    // углы повортов камеры в радианах
    double tan_tar_radian=  cameraToThisObj->tan;
    double gamma_tar_radian=cameraToThisObj->gamma;
    double psi_tar_radian=  cameraToThisObj->psi;
    double d_psi_tar_rad=0.0;
    if(cameraToBack==true)      d_psi_tar_rad=M_PI;

    // кватернион поворотов
    Quaternion qPI_2(   Vec(0.0, 1.0, 0.0),-M_PI/2.0);
    Quaternion qPsi(    Vec(0.0, 1.0, 0.0),psi_tar_radian+d_psi_tar_rad+dpsi_camera);
    Quaternion qTeta(   Vec(0.0, 0.0, 1.0),-gamma_tar_radian+dgamma_camera);
    Quaternion qGamma(  Vec(1.0, 0.0, 0.0),tan_tar_radian+dteta_camera);

    if(cameraUp==true)
    {
        Quaternion tempG(Vec(1.0, 0.0, 0.0),-M_PI/2);
        qGamma=tempG;
        Quaternion tempTan(Vec(0.0, 0.0, 1.0),0);
        qTeta=tempTan;
    }
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
        teta_radian=gradToRadian(aircraft.teta+teta0);
        gamma_radian=gradToRadian(aircraft.gamma+gamma0);
        psi_radian=KursToPsi(gradToRadian(aircraft.psi+psi0));

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
//void view3DArea::initScene(Lib3dsFile *file3ds)
//{
//    camera()->setFieldOfView(gradToRadian(45));
//    camera()->setZNearCoefficient(0.0001);
//    if (!file3ds)   return;

//    // Lights
//    // GLfloat amb[] = {0.5, 0.5, 0.5, 1.0};
//    // GLfloat dif[] = {0.0, 0.0, 0.0, 0.0};
//    // GLfloat spe[] = {1.0, 1.0, 1.0, 1.0};
//    GLfloat pos[] = {1.0, 1.0, 0.0, 0.0};
//    int li=GL_LIGHT0;

//   // for (Lib3dsLight* l=*((*file3ds).lights); l; l=l->next)
//     for (int i=0; i<file3ds->nlights;i++)
//    {
//        glEnable(li);
//        glLightfv(li, GL_POSITION, pos);
//    }
//    Quaternion qPI_2( Vec(0.0, 1.0, 0.0),M_PI/2.0);
//    camera()->frame()->setTranslation(Vec(0.0, 0.0, 1.0));
//    camera()->frame()->setRotation(qPI_2);
//    // Camera
//    Lib3dsCamera* camera3ds = NULL;
//    if(file3ds->ncameras!=0)
//        camera3ds = file3ds->cameras[0];
////    Lib3dsNode* c = lib3ds_file_node_by_name(file3ds, camera_name, LIB3DS_NODE_CAMERA);
////    Lib3dsNode* t = lib3ds_file_node_by_name(file3ds, camera_name, LIB3DS_NODE_CAMERA_TARGET);

//    if (camera3ds==NULL)
//        return;


//    camera()->setPosition(Vec(camera3ds->position));

//    camera()->lookAt(Vec(camera3ds->target));
//    //camera()->lookAt(Vec(t->data.target.pos));
//    Vec up=camera()->frame()->transformOf(Vec(0.0, 0.0, 1.0));
//    float angle=atan2(up.x, up.y);
//    Quaternion q(Vec(0.0, 0.0, 1.0), camera3ds->roll-angle);
//    camera()->frame()->rotate(q);
//    camera()->setFieldOfView(M_PI/180.0*camera3ds->fov);
//    camera()->setZNearCoefficient(0.0);

//    setSceneRadius(radiusScene);
//    setFPSIsDisplayed(true);

//}

//void view3DArea::renderNode(Lib3dsFile *file,Lib3dsNode *node)
//{
//    for (Lib3dsNode* p=node->childs; p!=0; p=p->next)
//        renderNode(file,p);

////    if (node->type == LIB3DS_OBJECT_NODE)
////    {
////        if (strcmp(node->name,"$$$DUMMY")==0)
////            return;

////        if (!node->user_ptr)
////        {
////            Lib3dsMesh *mesh=lib3ds_file_mesh_by_name(file, node->name);
////            if (!mesh)
////                return;

////            node->user.d = glGenLists(1);
////            glNewList(node->user.d, GL_COMPILE);

////            Lib3dsVector *normalL = new Lib3dsVector[3*mesh->faces];

////            Lib3dsMatrix M;

////            lib3ds_matrix_copy(M, mesh->matrix);
////            lib3ds_matrix_inv(M);

////            glMultMatrixf(&M[0][0]);

////            lib3ds_mesh_calculate_normals(mesh, normalL);

////            for (unsigned int p=0; p<mesh->faces; ++p)
////            {
////                Lib3dsFace *f=&mesh->faceL[p];
////                Lib3dsMaterial *mat=0;

////                if (material[0])
////                    mat=lib3ds_file_material_by_name(file, material);

////                if (mat)
////                {
////                    static GLfloat a[4]={0,0,0,1};
////                    float s;

////                    glMaterialfv(GL_FRONT, GL_AMBIENT, a);//
////                    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
////                    //  glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);

////                    s = pow(2, 10.0*mat->shininess);
////                    if (s>128.0)
////                        s=128.0;
////                    glMaterialf(GL_FRONT, GL_SHININESS, s);
////                }
////                else
////                {
////                    Lib3dsRgba a={0.2, 0.2, 0.2, 1.0};
////                    Lib3dsRgba d={0.8, 0.8, 0.8, 1.0};
////                    //Lib3dsRgba s={1.0, 1.0, 1.0, 1.0};
////                    glMaterialfv(GL_FRONT, GL_AMBIENT, a);
////                    glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
////                    //	glMaterialfv(GL_FRONT, GL_SPECULAR, s);
////                }

////                glBegin(GL_TRIANGLES);
////                glNormal3fv(normal);
////                for (int i=0; i<3; ++i)
////                {
////                    glNormal3fv(normalL[3*p+i]);
////                    glVertex3fv(mesh->pointL[points[i]].pos);
////                }
////                glEnd();
////            }
////            delete[] normalL;
////            glEndList();
////        }
////        if (node->user_ptr)
////        {
////            glPushMatrix();

////            Lib3dsObjectData* d = &node->data.object;
////            glMultMatrixf(&node->matrix[0][0]);
////            glTranslatef(-d->pivot[0], -d->pivot[1], -d->pivot[2]);
////            glCallList(node->user.d);
////            glPopMatrix();
////        }
////    }
//}

void view3DArea::slotRes(int w,int h)
{
    resize(w,h);
}
void view3DArea::slotPosWindow(int x,int y)
{
    move(x,y);
}


