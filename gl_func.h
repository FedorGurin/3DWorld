#ifndef _GL_FUNC_H
#define _GL_FUNC_H
#include <QString>
#include "Matrix3D_D.h"
#include "Vector3D_D.h"

//! углы относительно строительной оси
class TAngle
{
public:
    TAngle(double tau_=0.0,double sigma_=0.0):tau(tau_),sigma(sigma_){}
    void set(double tau_,double sigma_){tau=tau_;sigma=sigma_;}

    //! смещение по вертикали (положительное направление - вверх)
    double tau;
    //! смещение по горизонтали (положительное направление - влево)
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

    //! проверить углы
    void checkAngle(double &gorPos,double &verPos,double &gorPosPred,double &verPosPred);

    //! углы передаваемые этой функции должны быть в градусах
    TAngle limitAngle(TAngle angle);
    TAngle limitAngle(TAngle angle,           /*точка которую нужно ограничить*/
                      TAngle angleCenter,     /*центральная точка от которой будут строятся ограничения*/
                      double radius_grad_max, /*минимальный радиус*/
                      double radius_grad_min); /*максимальный радиус*/
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
    //! сидим на ограничении
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
//! преобразование точки(в земной системе координат) в координаты КАИ(два угла относительно строительной оси)
void earthToSGF(const Vector3D_D &inCoord,const Vector3D_D &inAircraft, const Matrix3D_D &inMatrixAircraft,  TAngle *outAngleSGF);

//! преобразование двух углов(относительно СГФ) в координаты точки в земной системе координат и пересечение с высотой dH
void sgfToEarth(const TAngle &inAngleSGF,const Vector3D_D &inAircraft,const Matrix3D_D &inMatrixAircraft,const double &dH,Vector3D_D *outCoord);

typedef struct _Tcordd
{
        int grad;
        int min;
        double sec;
} Tcordd;

/*
Преобразование градусов/минут/секунд в дробное число.
Вызов: <Результат в double> = coord_to_float(<градусы>, <минуты>, <секунды>)
*/
double coord_to_float(int grad, int min, float sec);

/*
Преобразование дробного числа в градусы/минуты/секунды
Вызов: <Результат в int>[3] = coord_to_float(<Угол дробным числом>)
Результат является ссылкой на структуру из двух int и одного double (градусы, минуты, секунды)
*/
Tcordd float_to_coord(double flt);

//! Преобразование сферических в декартовы
/*void convertSphereToDekart(double lambda0,double fi0,
                           double lambda,double fi,
                           double &x,double &z);

//! Преобразование декартовых в сферические
void convertDekartToSphere(double lambda0,double fi0,
                           double x,double z,
                           double &lambda,double &fi);
*/
double GradToRadian(double x);
double RadianToGrad(double x);
//! Преобразование [180.0;-180] в [0;360.0]
double PsiToKurs(double);
//! Преобразование [0;360.0] в [180.0;-180]
double KursToPsi(double);
//! проверка выхода за [180;-180]
double checkPI(double);
double check180(double);
//! проверка для угла Сигма
void checkSigma(double &sigma,double D0x1, double D0z1);
//! Преобразование сферических в декартовы
void convertSphereToDekart(double lambda0,double fi0,
                           double lambda,double fi,
                           double &x,double &z);

//! Преобразование декартовых в сферические
void convertDekartToSphere(double lambda0,double fi0,
                           double x,double z,
                           double &lambda,double &fi);


typedef struct
{
    //! уникальный идентификатор объекта
    int id;

    //! код объекта
    int code;
    //! угол атаки
    float alfa;
    //! воздушная скорости
    float vc;

    //! углы ориентации вектора скорости в траекторной СК
    float fi;
    float unt;

    //! координаты носителя в географической СК
    double fi_geo;
    double lam_geo;

    //! координаты старта носителя в географической СК
    double fi0_geo;
    double lam0_geo;

    //! координаты в земной СК
    float xg;
    float yg;
    float zg;

    //! компоненты вектора скорости относительной земной СК
    float vxg;
    float vyg;
    float vzg;

    //! компоненты ускорений
    float axg;
    float ayg;
    float azg;

    //! угловые скорости вращения носителя
    float omega_x;
    float omega_y;
    float omega_z;

    //! ориентация носителя в связанной СК
    float psi;
    float gamma;
    float tan;
    //! число Маха
    float m;
    //!
    float nya;
    //!
    float ny;
    //!
    float nx;

    int idManevr;
}TVis;
//! данные по работе с высотой.
typedef struct _THeight_data
{
    double x;   // Координата точки в которой запрашивается высота рельефа.
    double z;   // Координата точки в которой запрашивается высота рельефа.
    double h;   // Высота точки.
}THeight_data;

double cutValues(double x);
template<typename T> T limitMinMax(T value,T min,T max)
{
    if(value<min) value=min;
    else if(value>max) value=max;

    return value;
}

//! ограничение сверху
double limitMax(double value,double max);
//! ограничение снизу
double limitMin(double value,double min);
class FilterAperiodic
{
public:
    FilterAperiodic(double valueStart_,double k_,double step_,double maxValue_, double minValue_);
    //! сброс фильтра
    void resetFilter(double valueStart_);
    //! установка текущего значения фильтра
    double filterValue(double valueCurr);
    //! обновление фильтра(с заданной частотой)
    void refresh();
    //! возврат текущего значения фильтра
    double setFilterValue(double currentValue);
    //! текущее значение
    double curValue(){return value;}
    //! разность между текущим и предыдущим значением
    double delta(){return delta_;}
    //! задать коэффициент при звене
    void setK(double k_);
private:
    double valueStart;
    double valueCurrent;//значение на входе в фильтр
    double maxValue;
    double minValue;

    double value;//текущее знчачение фильтра
    double step;
    double k;

    double delta_;
};



QString TranslateFromCP1251(const QByteArray& ArrayString);
QString TranslateFromKOI8(const QByteArray& ArrayString);

#endif
