/*
 * visual3D.cpp
 *
 *  Created on: 10.11.2008
 *      Author: user
 */
#include "visual3D.h"
#include <QVBoxLayout>

cl_Visual3D::cl_Visual3D()
{

    //view3DModel=new View3DModel;
    //view3DFlight=new View3DFlight;
    area3D=new view3DArea;
    area3D->show();
}
