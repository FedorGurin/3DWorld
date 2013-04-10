/*
 * visual.h
 *
 *  Created on: 10.11.2008
 *      Author: user
 */

#ifndef VISUAL_H_
#define VISUAL_H_

#include <QWidget>


#include "view3DArea.h"

class cl_Visual3D:public QWidget
{

public:
    cl_Visual3D();
    void setGamma(float gamma){/*view3DModel->setGamma(gamma);*/}
    void setTeta(float teta){/*view3DModel->setTeta(teta);*/}
    void setPsi(float psi){/*view3DModel->setPsi(psi);*/}


//    void setAirCraftCoord(float x,float y,float z,float teta,float gama,float psi)
//    {
//        // view3DFlight->setAirCraft(x,y,z);
//        TSolidData data;
//        data.x=x;	    data.y=y;	    data.z=z;
//        data.teta=teta; data.gamma=gama;data.psi=psi;
//        area3D->setAirCraft(data);
//    }
//    void setTargetCoord(float x, float y,float z,float teta,float gama,float psi,int num)
//    {
//        //view3DFlight->setTarget(x,y,z,num);
//        TSolidData data;
//        data.x=x;	    data.y=y;	    data.z=z;
//		data.teta=teta;	    data.gamma=gama;data.psi=psi;

//		area3D->setTarget(data,num);
//	}
    //void setXTarget(float x,int numT){view3DFlight->setXTarget(x,num);}
    //void setYTarget(float y,int numT){view3DFlight->setYTarget(y,num);}
    //void setZTarget(float z,int numT){view3DFlight->setZTarget(z,num);}
    //void clearXYZ(){view3DFlight->clearMasXYZ();}
    void clearScene()
    {
        //view3DFlight->clearAllScene();
        //area3D->clearAllScene();
    }
public:

    view3DArea *area3D;
};

#endif /* VISUAL_H_ */
