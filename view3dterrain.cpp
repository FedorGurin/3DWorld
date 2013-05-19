#include "view3DTerrain.h"
#include <QApplication>
View3DTerrain::View3DTerrain(QWidget *parent):QGLWidget(parent)
{
    dialog=new QFileDialog(this);
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setNameFilter(("File with Height (*.asc)"));
    dialog->setDirectory(qApp->applicationDirPath());
    file=new QFile;
    mapH=new TASCII_MAP;
    stream=new QTextStream(file);
    threadParser=new ThreadParser;
    loadCompleate=false;
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
    dialog->selectFile(qApp->applicationDirPath()+nameFile);
    slotAccepted();
    return true;
}

void View3DTerrain::slotAccepted()
{
    QStringList list=dialog->selectedFiles();
    if(list.size()!=0)
    {
        QString fileName=list[0];

        file->setFileName(fileName);
        if (!file->open(QIODevice::ReadOnly | QIODevice::Text))   return;
        threadParser->setParserFile(stream,mapH);
        //! загружаем карту цветов


        loadCompleate=false;
        threadParser->start(QThread::IdlePriority);
    }
}
void View3DTerrain::slotThreadFinished()
{
    loadCompleate=true;
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
    QStringList list=(out->readAll()).split("\n",QString::SkipEmptyParts);
    printf("Size=%d\n",list.size());

    QStringList tempList;
    tempList=list[0].split(" ",QString::SkipEmptyParts);
    mapH->ncols=tempList[1].toDouble()/2;

    tempList=list[1].split(" ",QString::SkipEmptyParts);
    mapH->nrows=tempList[1].toDouble()/2;

    tempList=list[2].split(" ",QString::SkipEmptyParts);
    mapH->xllcorner=tempList[1].toDouble();

    tempList=list[3].split(" ",QString::SkipEmptyParts);
    mapH->yllcorner=tempList[1].toDouble();

    tempList=list[4].split(" ",QString::SkipEmptyParts);
    mapH->cellsize=tempList[1].toDouble();

    for(long i=5;i<list.size();i++)
    {
        QVector<double> vec;
        tempList=list[i].split(" ",QString::SkipEmptyParts);
        for(int j=0;j<tempList.size();j++)
        {
            vec.push_back(tempList[j].toDouble());
        }
        mapH->data.push_back(vec);
    }
    for(long i=0;i<mapH->ncols;i++)
    {
        QVector<QVector3D> vec;
        for(long j=0;j<mapH->nrows;j++)
        {
            double x,z;
            convertSphereToDekart(GradToRadian(mapH->yllcorner+mapH->ncols/2*mapH->cellsize),
                                  GradToRadian(mapH->xllcorner+mapH->nrows/2*mapH->cellsize),
                                  GradToRadian(mapH->yllcorner+i*mapH->cellsize),
                                  GradToRadian(mapH->xllcorner+j*mapH->cellsize),
                                  x,
                                  z);
            vec.push_back(QVector3D(x,mapH->data[i][j],z));
        }
        mapH->dataVectors.push_back(vec);

    }
    //персчет в декартовые координаты

}
