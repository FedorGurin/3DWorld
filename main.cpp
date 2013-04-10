#include <QtGui/QApplication>
#include "view3DArea.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*cl_Visual3D* w=new cl_Visual3D;
    w->show();*/

    view3DArea *area3D=new view3DArea;
    area3D->show();

    return a.exec();
}
