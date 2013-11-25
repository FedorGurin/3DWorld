       #ifndef GL_FUNC_H
#define GL_FUNC_H

#include "Matrix3D_D.h"
//#include "ibuffer.h"
#include <limits.h>
#include <QDomElement>

//#define M_PI 3.14159265358979323846f


//! компиляция для тренажера
//#define TRENAG_COMPILE

class TTable2D
{
public:
    double *m;
    unsigned sizeCol;
    unsigned sizeRow;
};
class TTable1D
{
public:
    double *m;
    unsigned sizeCol;
    unsigned sizeRow;
};
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
//! определение двух углов направления вектора заданного в земной горизонтированной системе координат
void earthTo2Angles(Vector3D_D inVec,TAngle *anglesVec);

//! расчет общей матрицы поворта на два угла
Matrix3D_D calcTAngleMatrix(TAngle);

//! преобразование точки(в земной системе координат) в координаты КАИ(два угла относительно строительной оси)
void earthToSGF(const Vector3D_D &inCoord,const Vector3D_D &inAircraft, const Matrix3D_D &inMatrixAircraft,  TAngle *outAngleSGF);

//void NaspCoordToEarthCoord();
//! преобразование двух углов(относительно СГФ) в координаты точки в земной системе координат и пересечение с высотой dH
void sgfToEarth(const TAngle &inAngleSGF,const Vector3D_D &inAircraft,const Matrix3D_D &inMatrixAircraft,const double &dH,Vector3D_D *outCoord);

//! определение направляющего вектора()
void sgfToEarthDirectionVec(TAngle inAngleSGF,Vector3D_D inAircraft,Matrix3D_D &inMatrixAircraft,Vector3D_D *outDirectionVec);

//! преобразование из земной СК в горизонтированную самолетную СК
void convertEarthToGorizontAircraft(Vector3D_D inAircraft,Matrix3D_D matrixPsi,Vector3D_D inPoint,Vector3D_D &outPointGor);

//! преобразование из горизонтированной самолетной СК в земную СК
void convertGorizontAircraftToEarth(Vector3D_D inPointGor, Vector3D_D inAircraft, Matrix3D_D matrixPsi,Vector3D_D &outPointEarth);

void convertGorizontAircraftToEarth(Vector3D_D inPointGor,Matrix3D_D matrixPsi,Vector3D_D &outPointEarth);

//! проверка для угла Сигма
void checkSigma(double &sigma,double D0x1, double D0z1);

//! углы поворота головы на цель
void calcAngleRotateHeadEarth(Vector3D_D &vecAircraft,   /*вектор на центр самолета относительно земли*/
                              Vector3D_D &vecDevice,     /*вектор на центр устройства(контейнера) относит. самолета*/
                              Vector3D_D &vecOnHead,     /*вектор на головку относительно центра контейнера*/
                              Vector3D_D &vecTarget,     /*вектор на цель(точка привязки) относительно земли*/
                              Matrix3D_D &matrixAircraft,/*матрица поворота самолета относительно земли*/
                              TAngle &angle              /*углы поворота головы контейнера на цель*/
                              );

void calcAgnleRotateHead(Vector3D_D &vecAircraft,   /*вектор на центр самолета относительно земли*/
                         Vector3D_D &vecDevice,     /*вектор на центр устройства(контейнера) относит. самолета*/
                         Vector3D_D &vecOnHead,     /*вектор на головку относительно центра контейнера*/
                         Vector3D_D &vecTarget,     /*вектор на цель(точка привязки) относительно земли*/
                         Matrix3D_D &matrixAircraft,/*матрица поворота самолета относительно земли*/
                         Matrix3D_D &matrixDevice,  /*матрица поворота устройства относ. самолета*/
                         TAngle &angle,               /* углы поворота головы контейнера на цель tau=(-90;90) sigma=(-180;180)*/
                         TAngle &angleNeg             /* углы поворота головы контейнера на цель tau=(-180;180) sigma=(-90;90)*/
                         );
//! линейная интерполяция
double linearInterpolation(double X,double Y2,double Y1,double X2,double X1);

//! Преобразование сферических в декартовы
void convertSphereToDekart(double lambda0,double fi0,
                           double lambda,double fi,
                           double &x,double &z);

//! Преобразование декартовых в сферические
void convertDekartToSphere(double lambda0,double fi0,
                           double x,double z,
                           double &lambda,double &fi);
///////////////////////////////////////////////////////
//! обертка преобразование географических в декартовые
void convertGeoToDekart(double lambda0,double fi0,
                        double lambda,double fi,
                        double &x,double &z);

//! обертка преобразование географических в декартовые
void convertDekartToGeo(double lambda0,double fi0,
                        double x,double z,
                        double &lambda,double &fi);
////////////////////////////////////////////////////////

double dTo(Vector3D_D &vecBegin,Vector3D_D &vecEnd);


//! преобразование координат точки в одной системе координат в другую
//TPoint convertToStartAircraft(TPoint rVectorOut, TPoint rVectorTo);
int searchColIndexInRow(double X,
                        double *mas,
                        int sizeCol);
int searchRowIndexIn2DTable(double Y,           /*тек. значение в столбце 0*/
                            double *mas,        /*указатель на массив*/
                            unsigned sizeCol,   /*кол-во столбцов*/
                            unsigned sizeRow);   /*кол-во строк*/
int searchColIndexIn2DTable(double X,           /*тек. значение в строке 0*/
                            double *mas,        /*указатель на массив*/
                            unsigned sizeCol);   /*кол-во столбцов*/
//! итерполяция для одной строки
double rowInterpolation(double Y,double *masY, int sizeCol,int indexCol=-1, int indexRow=1);
double rowInterpolation(double Y,const TTable1D &table,int indexCol, int indexRow);


//! линейная интерполяция двухмерной таблицы
double linearInter2DTable(double X,double Y,double *masXY,unsigned sizeRow,unsigned sizeCol,int indexRow=-1,int indexCol=-1);
double linearInter2DTable(double X,double Y,const TTable2D &table,int indexRow,int indexCol);



//! симуляция движения АСП попадание в любую движущуюся цель
// координаты и скорости заданы относительно земли
/* Рекоминдации при выборе среднего значения для функции сопротивления E
– если задать слишком малое значение E , то сопротивление воздуха мало,
 АСП будет двигаться как в безвоздушном пространстве, траектория близка к параболической;

– если задать слишком большое значение E , то сопротивление воздуха велико,
 траектория АСП быстро выпрямляется и АСП движется по прямой линии.

 для хорошо обтекаемых гладких АБ можно взять E=0.02
*/
void simulateTrajectoryASP(Vector3D_D inStartASP,       /*координаты АСП в начальный момент времени (м)*/
                           Vector3D_D inSpeedASP,       /*компоненты вектора скорости в начальный момент (м/с)*/
                           Vector3D_D inEndASP,         /*координаты цели(конечная точка падения АСП) (м)*/
                           double inTimeMoveASP,        /*время движения АСП(сек.)*/
                           double inE,                  /*среднее значение функции сопротивления АСП (б/р)*/
                           double inCurrentT,           /*текущее значение времени(сек.)*/
                           Vector3D_D &curentSpeedASP,  /*Текущие компоненты скорости АСП-выходная переменная (м/с)*/
                           Vector3D_D &curentCoordASP   /*Текущие координаты АСП-выходная переменная (м)*/
                           );


//! преобразование градусов в радианы
double GradToRadian(double x);
double RadianToGrad(double x);
TAngle RadianToGrad(TAngle);
TAngle GradToRadian(TAngle);
double KmToMeter(double x);
double MeterToKm(double x);
double cutValues(double x);
double signF(double num,double s);
double signF(double);
double sigmoid(double value);
double LimitArc(double x);
//! Преобразование [180.0;-180] в [0;360.0]
double PsiToKurs(double);
//! проверка выхода за [180;-180]
double checkPI(double);
double check180(double);
double ugol_ogrMPI_toMPI(double value);
//! Преобразование [0;360.0] в [180.0;-180]
double KursToPsi(double);
//! проверка выхода за [0;-360]
double check2PI(double);
double check360(double);
//! огрничение сверху и снизу
//double limitMinMax(double value,double min,double max);
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
//! чтение данных в 2D таблицу
void readDataToMatrix(const QDomElement &root, const QString &nameMatrix, TTable2D &pM);
//! чтение данных в 1D таблицу
void readDataToVector(const QDomElement& root, const QString& nameVector,TTable1D &pM);
//! апериодический фильтр
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

class FiltersAperiodicAngle
{
public:
    FiltersAperiodicAngle(TAngle valueStart_,
                          double k_,
                          double step_,
                          double maxValue_,
                          double minValue_);
    //! сброс фильтра
    void resetFilter(TAngle valueStart_);
    //! установка текущего значения фильтра
    TAngle filterValue(TAngle valueCurr);
    //! обновление фильтра(с заданной частотой)
    void refresh();
    //! возврат текущего значения фильтра
    TAngle setFilterValue(TAngle currentValue);
    //! задать коэффициент при звене
    void setK(double k_);

private:

    FilterAperiodic *filterSigma;
    FilterAperiodic *filterTau;
};

//! метод Бутчера 6 порядка
/*
Butcher I.C. On Runge-Ketta processes of high order,
Y.Austral.Math.Soc., Vol.4, Part 2, p.179-194. - 1964
*/
template<typename T,int nParam>
void odd_Butcher_6order(T* const obj,
                    int nSDU,
                    double *inValue,
                    double *outValue,
                    double step)
{
    double tempSDU[nParam];
    double c2=1.0/2.0,      c3=2.0/3.0,     c4=1.0/3.0,     c5=5.0/6.0,     c6=1.0/6.0,     c7=1.0;

    double a21=1.0/2.0;
    double a31=2.0/9.0,     a32=4.0/9.0;
    double a41=7.0/36.0,    a42=2.0/9.0,    a43=-1.0/12.0;
    double a51=-35.0/144.0, a52=-55.0/36.0, a53=35.0/48.0,  a54=15.0/8.0;
    double a61=-1.0/360.0,  a62=-11.0/36.0, a63=-1.0/8.0,   a64=1.0/2.0,    a65=1.0/10.0;
    double a71=-41.0/260.0, a72=22.0/13.0,  a73=43.0/156.0, a74=-118.0/39.0,a75=32.0/195.0,a76=80.0/39.0;

    double b1=13.0/200.0,   b2=0.0,       b3=11.0/40.0,   b4=11.0/40.0,   b5=4.0/25.0,   b6=4.0/25.0,    b7=13.0/200.0;
    //////////////////////////////////////////////////////////////////////////

    double k1[nParam],k2[nParam],k3[nParam],k4[nParam],k5[nParam],k6[nParam],k7[nParam];

    for(int i=nSDU;i<nParam;i++) tempSDU[i]=inValue[i];

    obj->SDU(inValue,k1);

    for(int i=1;i<nSDU;i++) tempSDU[i]=inValue[i]+step*a21*k1[i];
    tempSDU[0]=inValue[0]+c2*step;
    obj->SDU(tempSDU,k2);

    for(int i=1;i<nSDU;i++) tempSDU[i]=inValue[i]+step*(a31*k1[i]+a32*k2[i]);
    tempSDU[0]=inValue[0]+c3*step;
    obj->SDU(tempSDU,k3);

    for(int i=1;i<nSDU;i++) tempSDU[i]=inValue[i]+step*(a41*k1[i]+a42*k2[i]+a43*k3[i]);
    tempSDU[0]=inValue[0]+c4*step;
    obj->SDU(tempSDU,k4);

    for(int i=1;i<nSDU;i++) tempSDU[i]=inValue[i]+step*(a51*k1[i]+a52*k2[i]+a53*k3[i]+a54*k4[i]);
    tempSDU[0]=inValue[0]+c5*step;
    obj->SDU(tempSDU,k5);

    for(int i=1;i<nSDU;i++) tempSDU[i]=inValue[i]+step*(a61*k1[i]+a62*k2[i]+a63*k3[i]+a64*k4[i]+a65*k5[i]);
    tempSDU[0]=inValue[0]+c6*step;
    obj->SDU(tempSDU,k6);

    for(int i=1;i<nSDU;i++) tempSDU[i]=inValue[i]+step*(a71*k1[i]+a72*k2[i]+a73*k3[i]+a74*k4[i]+a75*k5[i]+a76*k6[i]);
    tempSDU[0]=inValue[0]+c7*step;
    obj->SDU(tempSDU,k7);

    outValue[0]=inValue[0]+step;
    for(int i=1;i<nSDU;i++) outValue[i]=inValue[i]+step*(b1*k1[i]+b2*k2[i]+b3*k3[i]+b4*k4[i]+b5*k5[i]+b6*k6[i]+b7*k7[i]);

}


//! метод Рунге-Кутты 4 порядка (шаблонная)
template<typename T,int nParam /*общее кол-во параметров*/>
void rk4(T* const obj,      /*объект класса должен содержать функцию SDU*/
         double *inValue,   /*начальные значения ОДУ, inValue!=0*/
         double *outValue   /*вычисленные значения ОДУ, outValue!=0*/,
         int nSDU           /*кол-во дифф. уравнений*/,
         double step        /*шаг интегрирования*/
         )
{
    double df[nParam][4];
    double fout[nParam];

    for(int j = 0; j < 4; j++)
    {
        if(j==0)
            obj->SDU(inValue,fout);
        else
            obj->SDU(outValue,fout);

        for(int i = 0; i < nSDU; i++)
        {
            df[i][j] = fout[i] *step;
            switch(j)
            {
            case 0:{outValue[i] = inValue[i] + 0.5 * df[i][j];break;}
            case 1:{outValue[i] = inValue[i] + 0.5 * df[i][j];break;}
            case 2:{outValue[i] = inValue[i] + df[i][j];break;}
            case 3:{outValue[i] = inValue[i] + (df[i][0]+2.0*(df[i][1]+df[i][2])+df[i][3])/6.0;}
            }
        }
        for(int k=nSDU;k<nParam;k++)
        {
            outValue[k]=inValue[k];
        }
    }
}
//! метод Адамса 4-порядка
template<typename T, typename X>
void adams4(T* const obj,
            int nSDU,
            double step)
{
    double fout[nSDU];
    double outValue[nSDU];

    //! расчитаем коэфф. для шагов
    double k_0=55.0/24.0;
    double k_1=59.0/24.0;
    double k_2=37.0/24.0;
    double k_3=9.0/24.0;

    obj->SDU(obj->buf[0]->current()->masDouble,fout);

    for(int k=0;k<nSDU;k++)
    {
        double fi=obj->buf_p[0]->current()->masDouble[k];
        double fi_1=obj->buf_p[1]->current()->masDouble[k];
        double fi_2=obj->buf_p[2]->current()->masDouble[k];
        double fi_3=obj->buf_p[3]->current()->masDouble[k];

        double y=obj->buf[0]->current()->masDouble[k];

        double y1=y+step*(k_0*fi-k_1*fi_1+k_2*fi_2-k_3*fi_3);
        outValue[k]=y1;
    }
    BufferDATA<X> bufferdata;
    for(int k=0;k<nSDU;k++)
    {
        bufferdata.masDouble[k]=outValue[k];
    }

    obj->buf.push(bufferdata.value);
    for(int k=0;k<nSDU;k++)
    {
        bufferdata.masDouble[k]=fout[k];
    }
    obj->buf_p.push(bufferdata.value);

}
//! метод Адамса 4-порядка (предикатор-корректор)
/*template<typename T,typename J,int nParam>
void adams4_pred_correct(T* const obj,
            int nSDU,
            IBuffers<J> *buffers,
            double step)
{

}
//! метод Эйлера
template<typename T,int nParam>
void euler(T* const obj,
           int nSDU,
           double *inValue,
           double *outValue,
           double step)
{

}*/
#endif // GL_FUNC_H
