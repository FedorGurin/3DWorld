#include "gl_func.h"
#include <math.h>
#include <QTextCodec>
#include <QStringList>


void checkSigma(double &sigma,double D0x1, double D0z1)
{
    if(D0x1>0.0) sigma=-atan(D0z1/D0x1);

    if(fabs(D0x1)<EPS && D0z1>0.0) sigma=-M_PI/2.0;
    if(fabs(D0x1)<EPS && D0z1<0.0) sigma=M_PI/2.0;

    if(D0x1<0.0 && D0z1>0.0) sigma=-atan(D0z1/D0x1)-M_PI;
    if(D0x1<0.0 && D0z1<0.0) sigma=-atan(D0z1/D0x1)+M_PI;



    /*if(fabs(D0z1)<EPS && D0x1>0.0) sigma=0.0;
    if(D0z1==0.0 && D0x1<0.0) sigma=M_PI;//+-M_PI*/
}
//! Преобразование сферических в декартовы
void convertSphereToDekart(double lambda0,double fi0,
                           double lambda,double fi,
                           double &x,double &z)
{

    Matrix3D_D Alambda0;
    double cosLambda0=cos(lambda0);
    double sinLambda0=sin(lambda0);

    Alambda0.x[0][0]=1.0;
    Alambda0.x[0][1]=0.0;
    Alambda0.x[0][2]=0.0;

    Alambda0.x[1][0]=0.0;
    Alambda0.x[1][1]=cosLambda0;
    Alambda0.x[1][2]=sinLambda0;

    Alambda0.x[2][0]=0.0;
    Alambda0.x[2][1]=-sinLambda0;
    Alambda0.x[2][2]=cosLambda0;
    /////////////////////////////////////////////////////

    Matrix3D_D Afi0;
    double cosfi0=cos(fi0);
    double sinfi0=sin(fi0);

    Afi0.x[0][0]=cosfi0;
    Afi0.x[0][1]=-sinfi0;
    Afi0.x[0][2]=0.0;

    Afi0.x[1][0]=sinfi0;
    Afi0.x[1][1]=cosfi0;
    Afi0.x[1][2]=0.0;

    Afi0.x[2][0]=0.0;
    Afi0.x[2][1]=0.0;
    Afi0.x[2][2]=1.0;
    /////////////////////////////////////////////////////
    Matrix3D_D AsG=Afi0*Alambda0;
    ////////////////////////////////////////////////////

    Matrix3D_D AgG;

    double cosfi=cos(fi);
    double sinfi=sin(fi);
    double cosLambda=cos(lambda);
    double sinLambda=sin(lambda);

    AgG.x[0][0]=cosfi;
    AgG.x[0][1]=-sinfi*cosLambda;
    AgG.x[0][2]=-sinfi*sinLambda;

    AgG.x[1][0]=sinfi;
    AgG.x[1][1]=cosfi*cosLambda;
    AgG.x[1][2]=cosfi*sinLambda;

    AgG.x[2][0]=0.0;
    AgG.x[2][1]=-sinLambda;
    AgG.x[2][2]=cosLambda;

    Matrix3D_D tempAsG=AsG;
    Matrix3D_D Asg=AgG*tempAsG.transpose();

    double tetta=asin(Asg.x[1][0]);
    double psi=atan2(Asg.x[1][2],Asg.x[1][1]);

    x=6356767.0*tetta; z=6356767.0*psi;

}

//! Преобразование декартовых в сферические
//! положение стартовой СК относительно земной СК
//!                                             -lambda
//!                                             -fi

//! Декартовы координаты объекта относительно стартовой СК
//!                                             -x
//!                                             -z
void convertDekartToSphere(double lambda0,double fi0,
                           double x,double z,
                           double &lambda,double &fi)
{
    Matrix3D_D Alambda0;
    double cosLambda0=cos(lambda0);
    double sinLambda0=sin(lambda0);

    Alambda0.x[0][0]=1.0;
    Alambda0.x[0][1]=0.0;
    Alambda0.x[0][2]=0.0;

    Alambda0.x[1][0]=0.0;
    Alambda0.x[1][1]=cosLambda0;
    Alambda0.x[1][2]=sinLambda0;

    Alambda0.x[2][0]=0.0;
    Alambda0.x[2][1]=-sinLambda0;
    Alambda0.x[2][2]=cosLambda0;
    /////////////////////////////////////////////////////

    Matrix3D_D Afi0;
    double cosfi0=cos(fi0);
    double sinfi0=sin(fi0);

    Afi0.x[0][0]=cosfi0;
    Afi0.x[0][1]=-sinfi0;
    Afi0.x[0][2]=0.0;

    Afi0.x[1][0]=sinfi0;
    Afi0.x[1][1]=cosfi0;
    Afi0.x[1][2]=0.0;

    Afi0.x[2][0]=0.0;
    Afi0.x[2][1]=0.0;
    Afi0.x[2][2]=1.0;
    /////////////////////////////////////////////////////
    Matrix3D_D AsG=Afi0*Alambda0;
    ////////////////////////////////////////////////////
    double tetta=x/6356767.0;
    double psi=z/6356767.0;

    Matrix3D_D Atetta;
    double cosTetta=cos(tetta);
    double sinTetta=sin(tetta);

    Atetta.x[0][0]=cosTetta;
    Atetta.x[0][1]=-sinTetta;
    Atetta.x[0][2]=0.0;

    Atetta.x[1][0]=sinTetta;
    Atetta.x[1][1]=cosTetta;
    Atetta.x[1][2]=0.0;

    Atetta.x[2][0]=0.0;
    Atetta.x[2][1]=0.0;
    Atetta.x[2][2]=1.0;


    Matrix3D_D Apsi;

    double cosPsi=cos(psi);
    double sinPsi=sin(psi);

    Apsi.x[0][0]=1.0;
    Apsi.x[0][1]=0.0;
    Apsi.x[0][2]=0.0;

    Apsi.x[1][0]=0.0;
    Apsi.x[1][1]=cosPsi;
    Apsi.x[1][2]=sinPsi;

    Apsi.x[2][0]=0.0;
    Apsi.x[2][1]=-sinPsi;
    Apsi.x[2][2]=cosPsi;

    Matrix3D_D tempMatrix=Atetta*AsG;
    Matrix3D_D AgG=Apsi*tempMatrix;

    fi=asin(AgG.x[1][0]);
    lambda=atan2(AgG.x[1][2],AgG.x[1][1]);

}
LimitPositionKAI::LimitPositionKAI(double radius_,
                                   double maxVDown,
                                   double maxVUp,
                                   double maxHRight,
                                   double maxHLeft)
{
    radius=radius_;
    maxVerUpAngle=maxVUp;
    maxVerDownAngle=maxVDown;

    maxGorLeftAngle=maxHLeft;
    maxGorRightAngle=maxHRight;

    init();
}
void LimitPositionKAI::init()
{
    angle.sigma=0.0;
    angle.tau=0.0;

    angleCurr.sigma=0.0;
    angleCurr.tau=0.0;

    angleGor=0.0;
    angleVer=0.0;

    gorPos_=0.0;
    verPos_=0.0;
    onLimits=false;
}

//! проверить углы
void LimitPositionKAI::checkAngle(double &gorPos,double &verPos,double &gorPosPred,double &verPosPred)
{
    if(verPos<=0.0)
    {
        if((gorPos*gorPos+verPos*verPos)>1.0)
        {
            gorPos=gorPosPred;
            verPos=verPosPred;
        }
    }
    double scaleVer=fabs(maxVerUpAngle)/fabs(maxVerDownAngle);

    if(verPos>=scaleVer) verPos=scaleVer;

    if(gorPos>0.0) angleGor=gorPos*fabs(maxGorRightAngle);
    else angleGor=gorPos*fabs(maxGorLeftAngle);

    if(verPos>0.0) angleVer=verPos*fabs(maxVerDownAngle);
    else angleVer=verPos*fabs(maxVerDownAngle);

    gorPos_=gorPos;
    verPos_=verPos;

    if((angleVer*angleVer+angleGor*angleGor)>=((radius-0.1)*(radius-0.1)))
    {
        onLimits=true;
    }else onLimits=false;

    angleCurr.sigma=GradToRadian(angleGor);
    angleCurr.tau=GradToRadian(angleVer);

}

/* углы передаваемые этой функции должны быть в градусах*/
TAngle LimitPositionKAI::limitAngle(TAngle angle)
{
    TAngle an;
    if((angle.sigma*angle.sigma+angle.tau*angle.tau)>(GradToRadian(radius)*GradToRadian(radius)))
    {
        double dx0=cos(angle.tau)*cos(angle.sigma);
        double dy0=sin(angle.tau);
        double dz0=-cos(angle.tau)*sin(angle.sigma);

        double dyz=sqrt(dy0*dy0+dz0*dz0);
        double k=0;
        double alfa=radius;
        double salfa=sin(GradToRadian(alfa));
        if(dx0>=0.0)
        {
            if(dyz<salfa) k=alfa/salfa;
            else k=alfa/dyz;
        }else k=alfa/dyz;

        an.tau=GradToRadian(k*dy0);
        an.sigma=-GradToRadian(k*dz0);

        return an;
    }
    return angle;
}
TAngle LimitPositionKAI::limitAngle10(TAngle angle, double rad)
{
    TAngle an;

    double dx0=cos(angle.tau)*cos(angle.sigma);
    double dy0=sin(angle.tau);
    double dz0=-cos(angle.tau)*sin(angle.sigma);

    double dyz=sqrt(dy0*dy0+dz0*dz0);
    double k=0;
    double alfa=rad;
    double salfa=sin(GradToRadian(alfa));
    if(dx0>=0.0)
    {
        if(dyz<salfa) k=alfa/salfa;
        else k=alfa/dyz;
    }else k=alfa/dyz;

    an.tau=GradToRadian(k*dy0);
    an.sigma=-GradToRadian(k*dz0);

    return an;
}

TAngle LimitPositionKAI::limitAngle(TAngle angle, double rad)
{
    TAngle an;
    if((angle.sigma*angle.sigma+angle.tau*angle.tau)>(GradToRadian(rad)*GradToRadian(rad)))
    {
        double dx0=cos(angle.tau)*cos(angle.sigma);
        double dy0=sin(angle.tau);
        double dz0=-cos(angle.tau)*sin(angle.sigma);

        double dyz=sqrt(dy0*dy0+dz0*dz0);
        double k=0;
        double alfa=rad;
        double salfa=sin(GradToRadian(alfa));
        if(dx0>=0.0)
        {
            if(dyz<salfa) k=alfa/salfa;
            else k=alfa/dyz;
        }else k=alfa/dyz;

        an.tau=GradToRadian(k*dy0);
        an.sigma=-GradToRadian(k*dz0);

        return an;
    }
    return angle;
}

TAngle LimitPositionKAI::limitAngle1(TAngle angleCenter, TAngle angle_, double R)
{
    TAngle angle,an;

    R=radius-R;
    angle.sigma=angleCenter.sigma-angle_.sigma;
    angle.tau=angleCenter.tau-angle_.tau;

    double dx0=cos(angle.tau)*cos(angle.sigma);
    double dy0=sin(angle.tau);
    double dz0=-cos(angle.tau)*sin(angle.sigma);

    double dyz=sqrt(dy0*dy0+dz0*dz0);
    double k=0;

    double alfa=R;
    double salfa=sin(GradToRadian(alfa));
    if(dx0>=0.0)
    {
        if(dyz<salfa)
            k=alfa/salfa;
        else
            k=alfa/dyz;
    }else
        k=alfa/dyz;

    an.tau=GradToRadian(k*dy0);
    an.sigma=-GradToRadian(k*dz0);

    return an;
}

TAngle LimitPositionKAI::limitAngle(TAngle angle,           /*точка которую нужно ограничить*/
                                    TAngle angleCenter,     /*центральная точка от которой будут строятся ограничения*/
                                    double radius_grad_max, /*минимальный радиус*/
                                    double radius_grad_min) /*максимальный радиус*/
{
    TAngle an;
    double Rmax=GradToRadian(radius_grad_max);
    double Rmin=GradToRadian(radius_grad_min);
    if(Rmax<0.0)
        Rmax=GradToRadian(radius);

    if((angle.sigma*angle.sigma+angle.tau*angle.tau)>(Rmax*Rmax))
    {
        double x=0.0,y=0.0;

        if(fabs(angle.sigma-angleCenter.sigma)>1e-8)
        {
            double a=(angle.tau-angleCenter.tau)/(angle.sigma-angleCenter.sigma);
            double b=angleCenter.tau-a*angleCenter.sigma;

            double disc=sqrt(pow(2*a*b,2)-4*(a*a+1)*(b*b-Rmax*Rmax));

            double x1=(-2*a*b-disc)/(2*(a*a+1));
            double x2=(-2*a*b+disc)/(2*(a*a+1));

            double y1=a*x1+b;
            double y2=a*x2+b;

            double d2=sqrt(pow(y2-angle.tau,2)+pow(x2-angle.sigma,2));
            double d1=sqrt(pow(y1-angle.tau,2)+pow(x1-angle.sigma,2));

            if(d1>d2)
            {
                y=y2;x=x2;
            }
            else
            {
                y=y1;x=x1;
            }

        }else
        {
            x=angle.sigma;
            y=-sqrt(Rmax*Rmax-x*x);
            //y=sqrt(R*R-x*x);
        }
        an.sigma=x;
        an.tau=y;

        return an;
    }else if(((angle.sigma*angle.sigma+angle.tau*angle.tau)<(Rmin*Rmin)) && Rmin>0.0)
    {
 /*       double sigma=(angle.sigma-angleCenter.sigma);
        double tau=  (angle.tau  -angleCenter.tau);

        double t_2=(Rmin*Rmin)/(sigma*sigma)-tau*tau);

        an.sigma=angleCenter.sigma+sigma*sqrt(fabs(t_2));
        an.tau=angleCenter.tau+tau*sqrt(fabs(t_2));

        return an;*/
        double x=0.0,y=0.0;

        if(fabs(angle.sigma-angleCenter.sigma)>1e-8)
        {
            double a=(angle.tau-angleCenter.tau)/(angle.sigma-angleCenter.sigma);
            double b=angleCenter.tau-a*angleCenter.sigma;

            double disc=sqrt(pow(2*a*b,2)-4*(a*a+1)*(b*b-Rmin*Rmin));

            double x1=(-2*a*b-disc)/(2*(a*a+1));
            double x2=(-2*a*b+disc)/(2*(a*a+1));

            double y1=a*x1+b;
            double y2=a*x2+b;

            double d2=sqrt(pow(y2-angle.tau,2)+pow(x2-angle.sigma,2));
            double d1=sqrt(pow(y1-angle.tau,2)+pow(x1-angle.sigma,2));

            if(d1>d2)
            {
                y=y2;x=x2;
            }
            else
            {
                y=y1;x=x1;
            }

        }else
        {
            x=angle.sigma;
            y=-sqrt(Rmin*Rmin-x*x);
        }
        an.sigma=x;
        an.tau=y;

        return an;

    }
    return angle;
}
TAngle LimitPositionKAI::limitAngle(TAngle angle,TAngle angleCenter,double radius_grad)
{
    TAngle an;
    double R=GradToRadian(radius);
    if(radius_grad>0.0) R=GradToRadian(radius_grad);

    if((angle.sigma*angle.sigma+angle.tau*angle.tau)>(R*R))
    {
        double x=0.0,y=0.0;

        if(fabs(angle.sigma-angleCenter.sigma)>1e-8)
        {
            double a=(angle.tau-angleCenter.tau)/(angle.sigma-angleCenter.sigma);
            double b=angleCenter.tau-a*angleCenter.sigma;

            double disc=sqrt(pow(2*a*b,2)-4*(a*a+1)*(b*b-R*R));

            double x1=(-2*a*b-disc)/(2*(a*a+1));
            double x2=(-2*a*b+disc)/(2*(a*a+1));

            double y1=a*x1+b;
            double y2=a*x2+b;

            double d2=sqrt(pow(y2-angle.tau,2)+pow(x2-angle.sigma,2));
            double d1=sqrt(pow(y1-angle.tau,2)+pow(x1-angle.sigma,2));

            if(d1>d2)  {y=y2;x=x2;}
            else {y=y1;x=x1;}

        }else
        {
            x=angle.sigma;
            y=-sqrt(R*R-x*x);
            //y=sqrt(R*R-x*x);
        }
        an.sigma=x;
        an.tau=y;

        return an;
    }
    if(radius_grad>0.0) angle.tau*=-1.0;
    return angle;
}

double LimitPositionKAI::limitAngleGor(double currentAngleGor)
{
    angle.sigma=limitMinMax(currentAngleGor,maxGorRightAngle,maxGorLeftAngle);
    return angle.sigma;
}

double LimitPositionKAI::limitAngleVer(double currentAngleVer)
{
    angle.tau=limitMinMax(currentAngleVer,maxVerDownAngle,maxVerUpAngle);
    return angle.tau;
}

double LimitPositionKAI::mappingAngleGorToGorPos(double aGor)
{
    aGor=limitAngleGor(aGor);

    if(aGor<0.0) return aGor/fabs(maxGorLeftAngle);
    return aGor/fabs(maxGorRightAngle);
}

double LimitPositionKAI::mappingAngleVerToVerPos(double aVer)
{
    aVer=limitAngleVer(aVer);

    if(aVer<0.0) return aVer/fabs(maxVerDownAngle);
    return aVer/fabs(maxVerDownAngle);
}
//! преобразование точки(в земной системе кординат) в координаты КАИ(два угла относительно строительной оси)
void earthToSGF(const Vector3D_D &inCoord,const Vector3D_D &inAircraft, const Matrix3D_D &inMatrixAircraft,  TAngle *outAngleSGF)
{
    double Dxg,Dyg,Dzg;

    Dxg=(inCoord.x)-(inAircraft.x);
    Dyg=(inCoord.y)-(inAircraft.y);
    Dzg=(inCoord.z)-(inAircraft.z);

    //! Находим компоненты вектора D  относительно связанной системы координат
    double Dx1,Dy1,Dz1;
    Dx1=Dxg*inMatrixAircraft.x[0][0]+Dyg*inMatrixAircraft.x[0][1]+Dzg*inMatrixAircraft.x[0][2];
    Dy1=Dxg*inMatrixAircraft.x[1][0]+Dyg*inMatrixAircraft.x[1][1]+Dzg*inMatrixAircraft.x[1][2];
    Dz1=Dxg*inMatrixAircraft.x[2][0]+Dyg*inMatrixAircraft.x[2][1]+Dzg*inMatrixAircraft.x[2][2];

    /*Dx1=Dxg*inMatrixAircraft.x[0][0]+Dyg*inMatrixAircraft.x[1][0]+Dzg*inMatrixAircraft.x[2][0];
    Dy1=Dxg*inMatrixAircraft.x[0][1]+Dyg*inMatrixAircraft.x[1][1]+Dzg*inMatrixAircraft.x[2][1];
    Dz1=Dxg*inMatrixAircraft.x[0][2]+Dyg*inMatrixAircraft.x[1][2]+Dzg*inMatrixAircraft.x[2][2];*/

   // Vector3D_D vec=inMatrixAircraft*
   /* Dx1=Dxg*inMatrixAircraft.x[0][0]+Dyg*inMatrixAircraft.x[1][0]+Dzg*inMatrixAircraft.x[2][0];
    Dy1=Dxg*inMatrixAircraft.x[0][1]+Dyg*inMatrixAircraft.x[1][1]+Dzg*inMatrixAircraft.x[2][1];
    Dz1=Dxg*inMatrixAircraft.x[0][2]+Dyg*inMatrixAircraft.x[1][2]+Dzg*inMatrixAircraft.x[2][2];*/


    //! Нормируем вектор D, т.е. и находим единичный вектор D0;
    double length=sqrt(Dx1*Dx1+Dy1*Dy1+Dz1*Dz1);

    double D0x1,D0y1,D0z1;
    D0x1=Dx1/length;
    D0y1=Dy1/length;
    D0z1=Dz1/length;
    //double d=D0x1*D0x1+D0y1*D0y1+D0z1*D0z1;
    //Находим выражения компонент единичного вектора   через углы sigma, tau

    double sigma,tau;

    sigma=atan2(D0z1,D0x1);
    checkSigma(sigma,D0x1,D0z1);
    tau=asin(D0y1);

    outAngleSGF->sigma=sigma;
    outAngleSGF->tau=tau;
}

//! преобразование двух углов(относительно СГФ) в прямую заданную параметрическим способом
void sgfToEarth(const TAngle &inAngleSGF,const Vector3D_D &inAircraft,const Matrix3D_D &inMatrixAircraft,const double &dH,Vector3D_D *outCoord)
{
    //===================================================
    Matrix3D_D matrixSigma, matrixTau;

    matrixSigma.x[0][0]=cos(inAngleSGF.sigma);
    matrixSigma.x[0][1]=0.0;
    matrixSigma.x[0][2]=-sin(inAngleSGF.sigma);

    matrixSigma.x[1][0]=0.0;
    matrixSigma.x[1][1]=1.0;
    matrixSigma.x[1][2]=0.0;

    matrixSigma.x[2][0]=sin(inAngleSGF.sigma);
    matrixSigma.x[2][1]=0.0;
    matrixSigma.x[2][2]=cos(inAngleSGF.sigma);
    //===================================================

    matrixTau.x[0][0]=cos(inAngleSGF.tau);
    matrixTau.x[0][1]=sin(inAngleSGF.tau);
    matrixTau.x[0][2]=0.0;

    matrixTau.x[1][0]=-sin(inAngleSGF.tau);
    matrixTau.x[1][1]=cos(inAngleSGF.tau);
    matrixTau.x[1][2]=0.0;

    matrixTau.x[2][0]=0.0;
    matrixTau.x[2][1]=0.0;
    matrixTau.x[2][2]=1.0;

    //! результирующая матрица поворота
    Matrix3D_D matrixResultRotate;
    Matrix3D_D tempMatrix;

    tempMatrix=matrixSigma*inMatrixAircraft;
    matrixResultRotate=matrixTau*tempMatrix;

    double D0xg,D0yg,D0zg;

    D0xg=matrixResultRotate.x[0][0];
    D0yg=matrixResultRotate.x[0][1];
    D0zg=matrixResultRotate.x[0][2];

    //double d=D0xg*D0xg+D0yg*D0yg+D0zg*D0zg;
    //if(d>1.0) qDebug("Error in NASP code: function=sgfToEarth, vector!=1.0\n");

    //double u=-inAircraft.y/D0yg;
    double u=(dH-inAircraft.y)/D0yg;

    outCoord->x=inAircraft.x+u*D0xg;
    outCoord->y=inAircraft.y+u*D0yg;
    outCoord->z=inAircraft.z+u*D0zg;
}
QString TranslateFromCP1251(const QByteArray& ArrayString)
{
    QTextCodec *codec = QTextCodec::codecForName("CP1251");
    QString unicode=codec->toUnicode(ArrayString);
    return unicode;
}

QString TranslateFromKOI8(const QByteArray& ArrayString)
{
    QTextCodec *codec = QTextCodec::codecForName("KOI8-R");
    QString unicode=codec->toUnicode(ArrayString);
    return unicode;
}
/*
Преобразование градусов/минут/секунд в дробное число.
Вызов: <Результат в double> = coord_to_float(<градусы>, <минуты>, <секунды>)
*/
double coord_to_float(int grad, int min, float sec)
{
    double flt;
    if((min < 60)&&(sec < 60))
        flt = (1.0)*grad + min/60.0 + sec/3600.0;
    else
        flt = 0;
    return flt;
}
/*
Преобразование дробного числа в градусы/минуты/секунды
Вызов: <Результат в int>[3] = coord_to_float(<Угол дробным числом>)
Результат является ссылкой на массив из трех int (градусы, минуты, секунды)
*/
Tcordd float_to_coord(double flt)
{
    double dgrad, dmin, dsec;
    Tcordd cordd;

    flt = modf(flt, &dgrad);
    flt = flt * 60;
    flt = modf(flt, &dmin);
    dsec = flt * 60;

    cordd.grad = (int)dgrad;
    cordd.min = (int)dmin;
    cordd.sec = dsec;
    return cordd;
}
double GradToRadian(double x)
{
    return (x*M_PI)/180.0;
}
double RadianToGrad(double x)
{
     return (x*180)/M_PI;
}

double PsiToKurs(double psi_)
{
    psi_=-psi_;
    double kurs_=psi_;
    if(psi_<0.0) kurs_=2*M_PI+psi_;
    return kurs_;
}
//! проверка выхода за [180;-180]
double checkPI(double angle/*в рад*/)
{
    if(angle>M_PI) angle+=-2.0*M_PI;
    else if(angle<-M_PI)angle+=2.0*M_PI;
    return angle;
}
double check180(double angle/*в рад*/)
{
    if(angle>180) angle+=-2.0*180;
    else if(angle<-180)angle+=2.0*180;
    return angle;
}
double KursToPsi(double kurs)
{
    double psi=kurs;
    if(kurs>M_PI) psi=psi-2*M_PI;
    psi=-psi;

    return psi ;
}

/*
//! Преобразование сферических в декартовы
void convertSphereToDekart(double lambda0,double fi0,
                           double lambda,double fi,
                           double &x,double &z)
{

    Matrix3D_D Alambda0;
    double cosLambda0=cos(lambda0);
    double sinLambda0=sin(lambda0);

    Alambda0.x[0][0]=1.0;
    Alambda0.x[0][1]=0.0;
    Alambda0.x[0][2]=0.0;

    Alambda0.x[1][0]=0.0;
    Alambda0.x[1][1]=cosLambda0;
    Alambda0.x[1][2]=sinLambda0;

    Alambda0.x[2][0]=0.0;
    Alambda0.x[2][1]=-sinLambda0;
    Alambda0.x[2][2]=cosLambda0;
    /////////////////////////////////////////////////////

    Matrix3D_D Afi0;
    double cosfi0=cos(fi0);
    double sinfi0=sin(fi0);

    Afi0.x[0][0]=cosfi0;
    Afi0.x[0][1]=-sinfi0;
    Afi0.x[0][2]=0.0;

    Afi0.x[1][0]=sinfi0;
    Afi0.x[1][1]=cosfi0;
    Afi0.x[1][2]=0.0;

    Afi0.x[2][0]=0.0;
    Afi0.x[2][1]=0.0;
    Afi0.x[2][2]=1.0;
    /////////////////////////////////////////////////////
    Matrix3D_D AsG=Afi0*Alambda0;
    ////////////////////////////////////////////////////

    Matrix3D_D AgG;

    double cosfi=cos(fi);
    double sinfi=sin(fi);
    double cosLambda=cos(lambda);
    double sinLambda=sin(lambda);

    AgG.x[0][0]=cosfi;
    AgG.x[0][1]=-sinfi*cosLambda;
    AgG.x[0][2]=-sinfi*sinLambda;

    AgG.x[1][0]=sinfi;
    AgG.x[1][1]=cosfi*cosLambda;
    AgG.x[1][2]=cosfi*sinLambda;

    AgG.x[2][0]=0.0;
    AgG.x[2][1]=-sinLambda;
    AgG.x[2][2]=cosLambda;

    Matrix3D_D tempAsG=AsG;
    Matrix3D_D Asg=AgG*tempAsG.transpose();

    double tetta=asin(Asg.x[1][0]);
    double psi=atan2(Asg.x[1][2],Asg.x[1][1]);

    x=6356767.0*tetta; z=6356767.0*psi;

}

//! Преобразование декартовых в сферические
//! положение стартовой СК относительно земной СК
//!                                             -lambda
//!                                             -fi

//! Декартовы координаты объекта относительно стартовой СК
//!                                             -x
//!                                             -z
void convertDekartToSphere(double lambda0,double fi0,
                           double x,double z,
                           double &lambda,double &fi)
{
    Matrix3D_D Alambda0;
    double cosLambda0=cos(lambda0);
    double sinLambda0=sin(lambda0);

    Alambda0.x[0][0]=1.0;
    Alambda0.x[0][1]=0.0;
    Alambda0.x[0][2]=0.0;

    Alambda0.x[1][0]=0.0;
    Alambda0.x[1][1]=cosLambda0;
    Alambda0.x[1][2]=sinLambda0;

    Alambda0.x[2][0]=0.0;
    Alambda0.x[2][1]=-sinLambda0;
    Alambda0.x[2][2]=cosLambda0;
    /////////////////////////////////////////////////////

    Matrix3D_D Afi0;
    double cosfi0=cos(fi0);
    double sinfi0=sin(fi0);

    Afi0.x[0][0]=cosfi0;
    Afi0.x[0][1]=-sinfi0;
    Afi0.x[0][2]=0.0;

    Afi0.x[1][0]=sinfi0;
    Afi0.x[1][1]=cosfi0;
    Afi0.x[1][2]=0.0;

    Afi0.x[2][0]=0.0;
    Afi0.x[2][1]=0.0;
    Afi0.x[2][2]=1.0;
    /////////////////////////////////////////////////////
    Matrix3D_D AsG=Afi0*Alambda0;
    ////////////////////////////////////////////////////
    double tetta=x/6356767.0;
    double psi=z/6356767.0;

    Matrix3D_D Atetta;
    double cosTetta=cos(tetta);
    double sinTetta=sin(tetta);

    Atetta.x[0][0]=cosTetta;
    Atetta.x[0][1]=-sinTetta;
    Atetta.x[0][2]=0.0;

    Atetta.x[1][0]=sinTetta;
    Atetta.x[1][1]=cosTetta;
    Atetta.x[1][2]=0.0;

    Atetta.x[2][0]=0.0;
    Atetta.x[2][1]=0.0;
    Atetta.x[2][2]=1.0;


    Matrix3D_D Apsi;

    double cosPsi=cos(psi);
    double sinPsi=sin(psi);

    Apsi.x[0][0]=1.0;
    Apsi.x[0][1]=0.0;
    Apsi.x[0][2]=0.0;

    Apsi.x[1][0]=0.0;
    Apsi.x[1][1]=cosPsi;
    Apsi.x[1][2]=sinPsi;

    Apsi.x[2][0]=0.0;
    Apsi.x[2][1]=-sinPsi;
    Apsi.x[2][2]=cosPsi;

    Matrix3D_D tempMatrix=Atetta*AsG;
    Matrix3D_D AgG=Apsi*tempMatrix;

    fi=asin(AgG.x[1][0]);
    lambda=atan2(AgG.x[1][2],AgG.x[1][1]);

}*/
//! Апериодический фильтр
FilterAperiodic::FilterAperiodic(double valueStart_,double k_,double step_,double maxValue_, double minValue_)
{
    k=k_;
    step=step_;
    maxValue=maxValue_;
    minValue=minValue_;
    value=valueStart_;
    valueStart=valueStart_;
    valueCurrent=valueStart;
    delta_=fabs(valueCurrent-value);
}
//! сброс фильтра (задать текущее значение фильтра)
void FilterAperiodic::resetFilter(double valueStart_)
{
    value=valueStart_;
}
//! возврат текущего значения фильтра
double FilterAperiodic::filterValue(double valueCurr)
{
    value=value+(k*cutValues(valueCurr-value)*step);
    value = limitMinMax(value,minValue,maxValue);
    return value;
}
void FilterAperiodic::setK(double k_)
{
    k=k_;
}
//! обновление фильтра(с заданной частотой)
void FilterAperiodic::refresh()
{
    delta_=fabs(valueCurrent-value);
    if(delta_<10e-3)
        value=valueCurrent;
    else
    {
        value=value+(k*cutValues(valueCurrent-value)*step);
        value=limitMinMax(value,minValue,maxValue);
    }
}
//! установка текущего значения фильтра
double FilterAperiodic::setFilterValue(double v)
{
    valueCurrent=v;
    return value;
}
double cutValues(double x)//отрезать дробную чпсть
{
    if(fabs(x)<1e-30) return 0.0;
    return x;
}
//! ограничение сверху
double limitMax(double value,double max)
{
    if(value>max) value=max;
    return  value;
}
//! ограничение снизу
double limitMin(double value,double min)
{
    if(value>min) value=min;
    return  value;
}
