#include <QApplication>
#include "view3DArea.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    view3DArea *area3D = new view3DArea;
    area3D->show();

    return a.exec();
}
