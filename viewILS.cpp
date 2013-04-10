#include "viewILS.h"

ViewILS::ViewILS(VisUDP *v_)
{
    vis=v_;
    obj=0;
    init();
}
void ViewILS::init()
{

}

//! отрисовка ИЛС
void ViewILS::draw()
{
    if(obj==0)
        return;
    printf("ViewILS::draw()\n");
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
    qglColor(QColor(Qt::green));
    glLineWidth(5.0);
    glDisable(GL_BLEND);//Уберем прозрачность
    glTranslatef(width()/2.0,height()/2.0,0);
    glRotated(-obj->gamma,0.0,0.0,1.0);

    for(int i=0;i<10;i++)
    {
        if(i==0) glLineWidth(5.0);
        else glLineWidth(2.0);
        double hILS=(obj->tan+GradToRadian(10*i))*(height()/camera()->fieldOfView());

       // qDebug("HFov=%f\n",camera()->fieldOfView());
       // qDebug("Height=%d\n",height());

        if(i==0)
        {

            //drawText((width()/2.0)-(width()/3.0) - 20,hILS,QString::number(-i*10));
            //drawText((width()/2.0)+(width()/3.0) + 20,hILS,QString::number(-i*10));

            drawText(-(width()/3.0) - 20,height()/2.0+hILS,QString::number(-i*10));
            drawText((width()/3.0) + 20,height()/2.0+hILS,QString::number(-i*10));

            glBegin(GL_LINES);
            glVertex2i((width()/2.0)-width()/3.0,height()/2.0+hILS);
            glVertex2i((width()/2.0)+width()/3.0,height()/2.0+hILS);
            glVertex2i(-width()/3.0,height()/2.0+hILS);
            glVertex2i(width()/3.0,height()/2.0+hILS);
            glEnd();
        }else
        {
            drawText(-(width()/3.0) - 20,height()/2.0+hILS,QString::number(-i*10));
            drawText((width()/3.0) + 20,height()/2.0+hILS,QString::number(-i*10));

            glBegin(GL_LINES);
            glVertex2i((width()/2.0)-width()/4.0,height()/2.0+hILS);
            glVertex2i((width()/2.0)+width()/4.0,height()/2.0+hILS);
            glVertex2i(-width()/4.0,height()/2.0+hILS);
            glVertex2i(width()/4.0,height()/2.0+hILS);
            glEnd();
        }
    }
    for(int i=0;i<10;i++)
    {
        if(i==0) glLineWidth(5.0);
        else glLineWidth(2.0);

        double hILS=(obj->tan+GradToRadian(-10*i))*(height()/camera()->fieldOfView());

        if(i==0)
        {
            drawText(-(width()/3.0) - 20,height()/2.0+hILS,QString::number(i*10));
            drawText((width()/3.0) + 20,height()/2.0+hILS,QString::number(i*10));

            glBegin(GL_LINES);
            //glVertex2i((width()/2.0)-width()/3.0,hILS);
            //glVertex2i((width()/2.0)+width()/3.0,hILS);
            glVertex2i(-width()/3.0,height()/2.0+hILS);
            glVertex2i(width()/3.0,height()/2.0+hILS);

            glEnd();
        }else
        {
            drawText(-(width()/3.0) - 20,height()/2.0+hILS,QString::number(i*10));
            drawText((width()/3.0) + 20,height()/2.0+hILS,QString::number(i*10));

            glBegin(GL_LINES);
            //glVertex2i((width()/2.0)-width()/4.0,hILS);
            //glVertex2i((width()/2.0)+width()/4.0,hILS);
            glVertex2i(-width()/4.0,height()/2.0+hILS);
            glVertex2i(width()/4.0,height()/2.0+hILS);
            glEnd();
        }
    }

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
