#include <QApplication>
#include "view3DArea.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    view3DArea *area3D = new view3DArea;
    area3D->show();
    // необязательный аргумент - текстовый файл с траекторией
    if(argc > 1)
        area3D->loadTXTFile(QString::fromLocal8Bit(argv[1]));

    return a.exec();
}
