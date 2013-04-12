#ifndef _GL_FUNC_H
#define _GL_FUNC_H
#include <QString>
#include "Matrix3D_D.h"
#include "Vector3D_D.h"

//! ���� ������������ ������������ ���
class TAngle
{
public:
    TAngle(double tau_=0.0,double sigma_=0.0):tau(tau_),sigma(sigma_){}
    void set(double tau_,double sigma_){tau=tau_;sigma=sigma_;}

    //! �������� �� ��������� (������������� ����������� - �����)
    double tau;
    //! �������� �� ����������� (������������� ����������� - �����)
    double sigma;

    const TAngle operator + (const TAngle &angle)const
    {
        TAngle a;
        a.sigma=this->sigma+angle.sigma;
        a.tau=this->tau+angle.tau;
        return a;
    }
    const TAngle operator - (const TAngle &angle)const
    {
        TAngle a;
        a.sigma=this->sigma-angle.sigma;
        a.tau=this->tau-angle.tau;
        return a;
    }

    void operator = (const TAngle& angle)
    {
        this->sigma=angle.sigma;
        this->tau=angle.tau;
    }

    void clear()
    {
        tau=0.0;
        sigma=0.0;
    }
};
class LimitPositionKAI
{
public:
    LimitPositionKAI(double radius_=12.0,
                     double maxVDown=-12.0,
                     double maxVUp=2.5,
                     double maxHRight=-12.0,
                     double maxHLeft=12.0);
    void init();

    //! ��������� ����
    void checkAngle(double &gorPos,double &verPos,double &gorPosPred,double &verPosPred);

    //! ���� ������������ ���� ������� ������ ���� � ��������
    TAngle limitAngle(TAngle angle);
    TAngle limitAngle(TAngle angle,           /*����� ������� ����� ����������*/
                      TAngle angleCenter,     /*����������� ����� �� ������� ����� �������� �����������*/
                      double radius_grad_max, /*����������� ������*/
                      double radius_grad_min); /*������������ ������*/
    TAngle limitAngle1(TAngle angleCenter, TAngle angle_, double R);

    TAngle limitAngle(TAngle angle, double rad);
    TAngle limitAngle10(TAngle angle, double rad);
    TAngle limitAngle(TAngle angle,TAngle angleCenter,double radius_grad=-1);

    double limitAngleGor(double currentAngleGor);
    double limitAngleVer(double currentAngleVer);

    double mappingAngleGorToGorPos(double aGor);
    double mappingAngleVerToVerPos(double aVer);

    double currentAngleGor(){return angleGor;}
    double currentAngleVer(){return angleVer;}

    double currentGorPos(){return gorPos_;}
    double currentVerPos(){return verPos_;}

    TAngle currentAngle(){return angleCurr;}
    TAngle currentAngleLimits(){return angle;}

    double curMaxVerDownDeg(){return maxVerDownAngle;}
    double curMaxHorLeftDeg(){return maxGorLeftAngle;}
    double curMaxHorRightDeg(){return maxGorRightAngle;}
    double curRadius(){return radius;}
    //! ����� �� �����������
    bool onLimits;
private:
    TAngle angle;
    TAngle angleCurr;
    double radius;

    double angleGor;
    double angleVer;

    double gorPos_;
    double verPos_;

    double maxVerUpAngle;
    double maxVerDownAngle;

    double maxGorLeftAngle;
    double maxGorRightAngle;
};
//! �������������� �����(� ������ ������� ���������) � ���������� ���(��� ���� ������������ ������������ ���)
void earthToSGF(const Vector3D_D &inCoord,const Vector3D_D &inAircraft, const Matrix3D_D &inMatrixAircraft,  TAngle *outAngleSGF);

//! �������������� ���� �����(������������ ���) � ���������� ����� � ������ ������� ��������� � ����������� � ������� dH
void sgfToEarth(const TAngle &inAngleSGF,const Vector3D_D &inAircraft,const Matrix3D_D &inMatrixAircraft,const double &dH,Vector3D_D *outCoord);

typedef struct _Tcordd
{
        int grad;
        int min;
        double sec;
} Tcordd;

/*
�������������� ��������/�����/������ � ������� �����.
�����: <��������� � double> = coord_to_float(<�������>, <������>, <�������>)
*/
double coord_to_float(int grad, int min, float sec);

/*
�������������� �������� ����� � �������/������/�������
�����: <��������� � int>[3] = coord_to_float(<���� ������� ������>)
��������� �������� ������� �� ��������� �� ���� int � ������ double (�������, ������, �������)
*/
Tcordd float_to_coord(double flt);

//! �������������� ����������� � ���������
/*void convertSphereToDekart(double lambda0,double fi0,
                           double lambda,double fi,
                           double &x,double &z);

//! �������������� ���������� � �����������
void convertDekartToSphere(double lambda0,double fi0,
                           double x,double z,
                           double &lambda,double &fi);
*/
double GradToRadian(double x);
double RadianToGrad(double x);
//! �������������� [180.0;-180] � [0;360.0]
double PsiToKurs(double);
//! �������������� [0;360.0] � [180.0;-180]
double KursToPsi(double);
//! �������� ������ �� [180;-180]
double checkPI(double);
double check180(double);
//! �������� ��� ���� �����
void checkSigma(double &sigma,double D0x1, double D0z1);
//! �������������� ����������� � ���������
void convertSphereToDekart(double lambda0,double fi0,
                           double lambda,double fi,
                           double &x,double &z);

//! �������������� ���������� � �����������
void convertDekartToSphere(double lambda0,double fi0,
                           double x,double z,
                           double &lambda,double &fi);


typedef struct
{
    //! ���������� ������������� �������
    int id;

    //! ��� �������
    int code;
    //! ���� �����
    float alfa;
    //! ��������� ��������
    float vc;

    //! ���� ���������� ������� �������� � ����������� ��
    float fi;
    float unt;

    //! ���������� �������� � �������������� ��
    double fi_geo;
    double lam_geo;

    //! ���������� ������ �������� � �������������� ��
    double fi0_geo;
    double lam0_geo;

    //! ���������� � ������ ��
    float xg;
    float yg;
    float zg;

    //! ���������� ������� �������� ������������� ������ ��
    float vxg;
    float vyg;
    float vzg;

    //! ���������� ���������
    float axg;
    float ayg;
    float azg;

    //! ������� �������� �������� ��������
    float omega_x;
    float omega_y;
    float omega_z;

    //! ���������� �������� � ��������� ��
    float psi;
    float gamma;
    float tan;
    //! ����� ����
    float m;
    //!
    float nya;
    //!
    float ny;
    //!
    float nx;

    int idManevr;
}TVis;
//! ������ �� ������ � �������.
typedef struct _THeight_data
{
    double x;   // ���������� ����� � ������� ������������� ������ �������.
    double z;   // ���������� ����� � ������� ������������� ������ �������.
    double h;   // ������ �����.
}THeight_data;

double cutValues(double x);
template<typename T> T limitMinMax(T value,T min,T max)
{
    if(value<min) value=min;
    else if(value>max) value=max;

    return value;
}

//! ����������� ������
double limitMax(double value,double max);
//! ����������� �����
double limitMin(double value,double min);
class FilterAperiodic
{
public:
    FilterAperiodic(double valueStart_,double k_,double step_,double maxValue_, double minValue_);
    //! ����� �������
    void resetFilter(double valueStart_);
    //! ��������� �������� �������� �������
    double filterValue(double valueCurr);
    //! ���������� �������(� �������� ��������)
    void refresh();
    //! ������� �������� �������� �������
    double setFilterValue(double currentValue);
    //! ������� ��������
    double curValue(){return value;}
    //! �������� ����� ������� � ���������� ���������
    double delta(){return delta_;}
    //! ������ ����������� ��� �����
    void setK(double k_);
private:
    double valueStart;
    double valueCurrent;//�������� �� ����� � ������
    double maxValue;
    double minValue;

    double value;//������� ��������� �������
    double step;
    double k;

    double delta_;
};



QString TranslateFromCP1251(const QByteArray& ArrayString);
QString TranslateFromKOI8(const QByteArray& ArrayString);

#endif
