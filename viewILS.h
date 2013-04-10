#ifndef VIEWILS_H
#define VIEWILS_H

#include <QGLViewer/qglviewer.h>

#include "Matrix3D_D.h"
#include "Vector3D_D.h"
#include "visudp.h"

//! класс для отображения ИЛС
class ViewILS:public QGLViewer
{
public:
    ViewILS(VisUDP *v_);
    void init();

    //! выбор объекта
    void setCameraToObject(TVis *obj_)
    {
        obj=obj_;
    }
    //void draw();
private:
    VisUDP *vis;
    TVis *obj;
};

#endif // VIEWILS_H
