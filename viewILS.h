#ifndef VIEWILS_H
#define VIEWILS_H

#include <QGLViewer/qglviewer.h>

#include "math_func.h"

#include "visudp.h"

//! класс для отображения ИЛС
class ViewILS:public QGLViewer
{
public:
    ViewILS(VisUDP *v_);
    void init();

    //! выбор объекта
    void setCameraToObject(TSendVehicleVisSimple *obj_)
    {
        obj = obj_;
    }
    void draw();
private:
    VisUDP *vis;
    TSendVehicleVisSimple *obj;
};

#endif // VIEWILS_H
