#include "view3DTerrain.h"
#include <QApplication>
#include "dataPath.h"
View3DTerrain::View3DTerrain(QWidget *parent):QGLWidget(parent)
{
    dialog=new QFileDialog(this);
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setNameFilter(("File with Height (*.asc)"));
    dialog->setDirectory(qApp->applicationDirPath());
    file    = new QFile;
    mapH    = new TASCII_MAP;
    stream  = new QTextStream(file);
    threadParser  = new ThreadParser;
    loadCompleate = false;
    version       = 0;
    connect(dialog,SIGNAL(accepted()),this,SLOT(slotAccepted()));
    connect(threadParser,SIGNAL(finished()),this,SLOT(slotThreadFinished()));
}
void View3DTerrain::openFileWithTerrain()
{
    dialog->exec();
}
void openFileWithMapColor()
{

}
bool View3DTerrain::openTerrainMap(QString nameFile)
{
    QString path = findDataFile(nameFile);
    if(QFileInfo::exists(path) == false)
    {
        qWarning("Terrain file not found: %s", qPrintable(nameFile));
        return false;
    }
    dialog->selectFile(path);
    slotAccepted();
    return true;
}

void View3DTerrain::slotAccepted()
{
    QStringList list = dialog->selectedFiles();
    if(list.isEmpty() == false)
    {
        QString fileName = list[0];
        // не начинаем новую загрузку, пока не закончилась предыдущая
        if(threadParser->isRunning())
            return;

        file->close();
        file->setFileName(fileName);
        if (!file->open(QIODevice::ReadOnly | QIODevice::Text))   return;
        stream->setDevice(file);
        threadParser->setParserFile(stream,mapH);
        // загружаем карту цветов


        loadCompleate=false;
        threadParser->start(QThread::IdlePriority);
    }
}
void View3DTerrain::slotThreadFinished()
{
    file->close();
    loadCompleate = (mapH->dataVectors.isEmpty() == false);
    version++;
}

View3DTerrain::~View3DTerrain()
{
     delete dialog;
}
ThreadParser::ThreadParser()
{

}
void ThreadParser::setParserFile(QTextStream *out_,TASCII_MAP *mapH_)
{
   mapH=mapH_;
   out=out_;
}
void ThreadParser::run()
{
    mapH->data.clear();
    mapH->dataVectors.clear();
    mapH->ncols = 0;
    mapH->nrows = 0;

    QStringList list=(out->readAll()).split("\n",QString::SkipEmptyParts);
    // заголовок ESRI ASCII Grid: ncols, nrows, xllcorner, yllcorner, cellsize, nodata_value
    if(list.size() < 6)
    {
        qWarning("Terrain file: wrong header");
        return;
    }
    double header[5];
    for(int i = 0; i < 5; i++)
    {
        QStringList tempList = list[i].split(" ",QString::SkipEmptyParts);
        if(tempList.size() < 2)
        {
            qWarning("Terrain file: wrong header");
            return;
        }
        header[i] = tempList[1].toDouble();
    }
    mapH->ncols     = header[0];
    mapH->nrows     = header[1];
    mapH->xllcorner = header[2];
    mapH->yllcorner = header[3];
    mapH->cellsize  = header[4];

    for(long i=6;i<list.size();i++)
    {
        QVector<double> vec;
        QStringList tempList=list[i].split(" ",QString::SkipEmptyParts);
        for(int j=0;j<tempList.size();j++)
        {
            vec.push_back(tempList[j].toDouble());
        }
        mapH->data.push_back(vec);
    }
    // файл может быть короче, чем указано в заголовке
    if(mapH->data.size() < mapH->ncols)
        mapH->ncols = mapH->data.size();
    for(long i=0;i<mapH->ncols;i++)
    {
        if(mapH->data[i].size() < mapH->nrows)
            mapH->nrows = mapH->data[i].size();
    }
    //персчет в декартовые координаты
    for(long i=0;i<mapH->ncols;i++)
    {
        QVector<QVector3D> vec;
        for(long j=0;j<mapH->nrows;j++)
        {
            double x,z;
            convertSphereToDekart(gradToRadian(mapH->yllcorner+mapH->ncols/2*mapH->cellsize),
                                  gradToRadian(mapH->xllcorner+mapH->nrows/2*mapH->cellsize),
                                  gradToRadian(mapH->yllcorner+i*mapH->cellsize),
                                  gradToRadian(mapH->xllcorner+j*mapH->cellsize),
                                  x,
                                  z);
            vec.push_back(QVector3D(5*x,mapH->data[i][j],500*z));
        }
        mapH->dataVectors.push_back(vec);
    }
}
