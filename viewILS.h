#ifndef VIEWILS_H
#define VIEWILS_H

#include <QGLViewer/qglviewer.h>

#include "math_func.h"
#include "Vector3D_D.h"
#include "visudp.h"

//! ����� ��� ����������� ���
class ViewILS:public QGLViewer
{
public:
    ViewILS(VisUDP *v_);
    void init();

    //! ����� �������
    void setCameraToObject(TSolidObj *obj_)
    {
        obj=obj_;
    }
    //void draw();
private:
    VisUDP *vis;
    TSolidObj *obj;
};

#endif // VIEWILS_H
