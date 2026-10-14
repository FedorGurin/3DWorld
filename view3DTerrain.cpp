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
    mapH->normals.clear();
    mapH->ncols = 0;
    mapH->nrows = 0;
    mapH->minHeight = 0.0;
    mapH->maxHeight = 0.0;

    QStringList list=(out->readAll()).split("\n",QString::SkipEmptyParts);
    // заголовок ESRI ASCII Grid: ncols, nrows, xllcorner, yllcorner, cellsize[, nodata_value]
    if(list.size() < 6)
    {
        qWarning("Terrain file: wrong header");
        return;
    }
    long   ncols = 0, nrows = 0;
    double nodata = -9999.0;
    int    firstDataLine = 0;
    for(int i = 0; i < list.size() && i < 7; i++)
    {
        QStringList tempList = list[i].split(" ",QString::SkipEmptyParts);
        if(tempList.size() != 2)
            break;
        QString key = tempList[0].toLower();
        bool isNumber = false;
        double value = tempList[1].toDouble(&isNumber);
        if(isNumber == false)
            break;
        if(key == "ncols")          ncols = long(value);
        else if(key == "nrows")     nrows = long(value);
        else if(key == "xllcorner" || key == "xllcenter") mapH->xllcorner = value;
        else if(key == "yllcorner" || key == "yllcenter") mapH->yllcorner = value;
        else if(key == "cellsize")  mapH->cellsize = value;
        else if(key == "nodata_value") nodata = value;
        else break;
        firstDataLine = i + 1;
    }
    if(ncols < 2 || nrows < 2 || mapH->cellsize <= 0.0)
    {
        qWarning("Terrain file: wrong header");
        return;
    }

    for(long i=firstDataLine;i<list.size();i++)
    {
        QVector<double> vec;
        QStringList tempList=list[i].split(" ",QString::SkipEmptyParts);
        for(int j=0;j<tempList.size();j++)
        {
            double h = tempList[j].toDouble();
            // точки без данных считаем уровнем моря
            if(h == nodata)
                h = 0.0;
            vec.push_back(h);
        }
        if(vec.isEmpty() == false)
            mapH->data.push_back(vec);
    }
    // файл может быть короче, чем указано в заголовке
    if(mapH->data.size() < nrows)
        nrows = mapH->data.size();
    for(long i=0;i<nrows;i++)
    {
        if(mapH->data[i].size() < ncols)
            ncols = mapH->data[i].size();
    }
    if(ncols < 2 || nrows < 2)
        return;

    // перевод в декартовые координаты: касательная плоскость в центре карты,
    // x - на север, z - на восток (как у объектов сцены), 1 единица = 1 м
    const double R = 6371000.0;
    const double lat0 = mapH->yllcorner + (nrows - 1) * mapH->cellsize / 2.0;
    const double lon0 = mapH->xllcorner + (ncols - 1) * mapH->cellsize / 2.0;
    const double cosLat0 = cos(gradToRadian(lat0));
    mapH->minHeight = mapH->maxHeight = mapH->data[0][0];
    for(long i=0;i<nrows;i++)
    {
        QVector<QVector3D> vec;
        // строки файла идут с севера на юг
        const double lat = mapH->yllcorner + (nrows - 1 - i) * mapH->cellsize;
        const double x = gradToRadian(lat - lat0) * R;
        for(long j=0;j<ncols;j++)
        {
            const double lon = mapH->xllcorner + j * mapH->cellsize;
            const double z = gradToRadian(lon - lon0) * R * cosLat0;
            const double h = mapH->data[i][j];
            mapH->minHeight = qMin(mapH->minHeight, h);
            mapH->maxHeight = qMax(mapH->maxHeight, h);
            vec.push_back(QVector3D(x, h, z));
        }
        mapH->dataVectors.push_back(vec);
    }
    // нормали по центральным разностям
    for(long i=0;i<nrows;i++)
    {
        QVector<QVector3D> vec;
        for(long j=0;j<ncols;j++)
        {
            const QVector3D &n0 = mapH->dataVectors[qMax(i-1,0L)][j];
            const QVector3D &s0 = mapH->dataVectors[qMin(i+1,nrows-1)][j];
            const QVector3D &w0 = mapH->dataVectors[i][qMax(j-1,0L)];
            const QVector3D &e0 = mapH->dataVectors[i][qMin(j+1,ncols-1)];
            QVector3D north = n0 - s0;   // вдоль +x
            QVector3D east  = e0 - w0;   // вдоль +z
            QVector3D n = QVector3D::crossProduct(east, north).normalized();
            if(n.y() < 0.0f)
                n = -n;
            vec.push_back(n);
        }
        mapH->normals.push_back(vec);
    }
    mapH->ncols = ncols;
    mapH->nrows = nrows;
}
