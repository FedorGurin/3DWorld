#ifndef TEMPLATENASP_HPP
#define TEMPLATENASP_HPP

#include "gl_func.h"
#include <QQueue>

//! гистерезис
template <class T> class Hysteresis
{
public:
    Hysteresis(){inHys=false;}
    bool check(T value,T center, T d_onHys, T d_offHys)
    {
        if(value<(center+fabs(d_onHys)) && value>(center-fabs(d_onHys)))
        {
            inHys=true;
            return true;
        }else  if(value>(center+fabs(d_offHys)) || value<(center-fabs(d_offHys)))
        {
            inHys=false;
            return false;
        }else if(value<(center+fabs(d_offHys)) && value>(center-fabs(d_offHys)) && inHys==true)
        {
            return true;
        }
        return false;
    }

private:
    //! попали на гистерезис
    bool inHys;
};
//! Для типа T должны быть определены операции <>=
template <typename T>
class Trigger
{
public:
    Trigger(T value)
    {
        newState=value;
        oldState=value;

        highFront_=false;
        lowFront_=false;
    }

    //! задать начальное состояние
    void setStartState(T value)
    {
        newState=value;
        oldState=value;

        highFront_=false;
        lowFront_=false;
    }

    //! задать текущее состояние
    void setState(T value)
    {
        newState=value;
        if(newState>oldState)
        {
            highFront_=true;
            lowFront_=false;
        }
        else if(newState<oldState)
        {
            highFront_=false;
            lowFront_=true;
        }else
        {
            highFront_=false;
            lowFront_=false;
        }
        oldState=newState;
    }

    //! текущее состояние
    bool state()
    {
        if(newState!=0) return true;
        else return false;
    }
    //! факт перехода состояния на более высокое значение
    bool isHightFront(){return highFront_;}
    //! факт перехода состояния на более низкое значение
    bool isLowFront(){return lowFront_;}
    //! факт перехода состояния на другое значение(низкое или большое)
    bool isChanged()
    {
        if(highFront_==true || lowFront_==true) return true;
        else return false;
    }

    //! сброс внутр. состояния в заданное в value
    void flush(T value)
    {
        newState=value;
        oldState=value;

        highFront_=false;
        lowFront_=false;
    }

private:
    T newState;
    T oldState;

    bool highFront_;
    bool lowFront_;
};

template <class T> class Button
{
public:
    Button(const T* b)
    {
        trigger=new Trigger<T>(*b);

        countHightFront_=0;
        countLowFront_=0;

        released_=false;
        pressed_=false;

        button_=b;

        maxHightCount_=100;
        maxLowCount_=100;

        timeHightFront=0;
        timeLowFront=0;
    }
    void init()
    {
        countHightFront_=0;
        countLowFront_=0;
        flush();
        maxHightCount_=100;
        maxLowCount_=100;

        timeHightFront=0;
        timeLowFront=0;
    }
    //! нажата или нет кнопка
    void setMaxCounts(int count)
    {
        maxHightCount_=count;
        maxLowCount_=count;
    }
    //! обновить состояние кнопок
    void refresh()
    {

        trigger->setState(*button_);

        if(maxHightCount_<countHightFront_) countHightFront_=1;
        if(maxLowCount_<countLowFront_) countLowFront_=1;

        if(*button_==true) timeHightFront++;
        if(*button_==false) timeLowFront++;
        if(trigger->isHightFront()==true)
        {
            timeHightFront=0;
            timeLowFront=0;

            countHightFront_++;

            pressed_=true;
            released_=false;
        }else if(trigger->isLowFront()==true)
        {
            timeLowFront=0;
            timeHightFront=0;

            countLowFront_++;

            released_=true;
            pressed_=false;
        }
    }
    //! кол-во нажатий на кнопку
    int countPressedClick()
    {
        return countHightFront_;
    }
    //! кол-во отпусканий кнопки
    int countReleasedClick()
    {
        return countLowFront_;
    }
    //! проверка нажата ли была кнопка
    bool pressed(){return pressed_;}
    //! текущее состояние кнопки
    bool currentState() {return *button_;}

    //! сброс состояния
    void flush()
    {
        released_=false;
        pressed_=false;

        countHightFront_=0;
        countLowFront_=0;

        trigger->flush(false);
    }

    void setPtrButton(const T* b)
    {
        button_=b;
    }

    //! проверка того, что кнопка отпущена
    bool released(){return released_;}
    Trigger<bool> *trigger;
private:
    //! было нажатие на кнопку
    bool released_;
    bool  pressed_;

    const T *button_;

    int countHightFront_;
    int countLowFront_;

    int maxLowCount_;
    int maxHightCount_;

    unsigned long timeHightFront;
    unsigned long timeLowFront;
};

template <class T> class Button3Positional
{
public:
    Button3Positional(const T *up,const T *neutral, const T *down)
    {
        up_=new Button<T>(up);
        neutral_=new Button<T>(neutral);
        down_=new Button<T>(down);
    }
    void init()
    {
        up_->init();
        neutral_->init();
        down_->init();
    }

    Button<T> *up(){return up_;}
    Button<T> *neutral(){return neutral_;}
    Button<T> *down(){return down_;}

    //! обновить состояние кнопок(эта функция должна периодически вызываться)
    //! причем частота вызовов должна быть менее или равна частоте вызова участка
    //! кода который проверяет состояние этой кнопки
    void refresh()
    {
        up_->refresh();
        neutral_->refresh();
        down_->refresh();
    }
private:
    Button<T> *up_;
    Button<T> *neutral_;
    Button<T> *down_;
};
class ILawOfSpeed
{
public:
    virtual double speedVer()=0;
    virtual double speedHor()=0;
    virtual void setDivideSpeedHor(double)=0;
    virtual void setDivideSpeedVer(double)=0;
    virtual void refresh()=0;

    virtual void stop()=0;
};
class DefaultSpeed:public ILawOfSpeed
{
public:
    DefaultSpeed(const float *LR,
                 const float *UD,
                 double step_=0.01);


    virtual double speedVer();
    virtual double speedHor();

    virtual void setDivideSpeedHor(double d);
    virtual void setDivideSpeedVer(double d);

    virtual void refresh();
    virtual void stop()
    {

    }
    double currentSpeedHor;
    double currentSpeedVer;

    FilterAperiodic *horFilter;
    FilterAperiodic *verFilter;

    //! скорость по горизонтали
    const float *speedHor_;
    //! скорость по вертикали
    const float *speedVer_;

    double limitHorPos;
    double limitVerPos;

    double divideHor;
    double divideVer;

    double step;
};

template <class T> class Knupel
{
public:
    Knupel(const float *LR,const float *UD,float step_=0.01)
    {

        speedHor=LR;
        speedVer=UD;

        step=step_;

        limits=new T;

        triggerOnLimits=new Trigger<bool> (limits->onLimits);
        //! первоначальная позиция
        currentPosHor=0.0;
        currentPosVer=-0.5;

        predPosHor=currentPosHor;
        predPosVer=currentPosVer;

        //! максимальная зона перемещения(зона симметричная)
        maxHorPos=1.0;
        maxVerPos=1.0;

        defaultSpeed=new DefaultSpeed(LR,UD,step);
        currentLawSpeed=defaultSpeed;

        isStop=false;
    }
    void setLawSpeed(ILawOfSpeed *law)
    {
        currentLawSpeed=law;
    }

    void init()
    {
        //! первоначальная позиция
        currentPosHor=0.0;
        currentPosVer=-0.5;

        predPosHor=currentPosHor;
        predPosVer=currentPosVer;

        maxHorPos=1.0;
        maxVerPos=1.0;

        //! уствновка множителя скорости(если это нужно)
        currentLawSpeed->setDivideSpeedHor(1.0);
        currentLawSpeed->setDivideSpeedVer(1.0);

        limits->init();

        triggerOnLimits=new Trigger<bool> (limits->onLimits);

        isStop=false;
    }
    //! необходим периодический вызов
    void refresh()
    {
        if(currentLawSpeed==0) currentLawSpeed=defaultSpeed;

        triggerOnLimits->setState(limits->onLimits);
        if(isStop==false)
        {
            currentLawSpeed->refresh();

            //! фильтр обработки по горизонтали
            filtrHorizontal();

            //! фильтр обработки по вертикали
            filtrVertical();

            //! ограничение кнюппеля
            limits->checkAngle(currentPosHor,currentPosVer,predPosHor,predPosVer);
        }else  currentLawSpeed->stop();
    }
    void filtrHorizontal()
    {
        predPosHor=currentPosHor;
        //qDebug("Knupel Speed=%f\n",currentLawSpeed->speedHor()*step);
        currentPosHor=predPosHor+currentLawSpeed->speedHor()*step;
        currentPosHor=limitMinMax(currentPosHor,-maxHorPos,maxHorPos);

        //return currentPosHor;
    }
    void filtrVertical()
    {
        predPosVer=currentPosVer;

        currentPosVer=predPosVer+currentLawSpeed->speedVer()*step;
        currentPosVer=limitMinMax(currentPosVer,-maxVerPos,maxVerPos);

        //return currentPosVer;
    }
    void setAnglePos(TAngle angle)
    {
        currentPosHor=limits->mappingAngleGorToGorPos(RadianToGrad(angle.sigma));
        currentPosVer=limits->mappingAngleVerToVerPos(RadianToGrad(angle.tau));

        predPosHor=currentPosHor;
        predPosVer=currentPosVer;
        //! ограничение кнюппеля
        limits->checkAngle(currentPosHor,currentPosVer,predPosHor,predPosVer);
    }

    //! сбросить
    void resetPosition()
    {
        currentPosVer=0.0;
        currentPosHor=0.0;
    }
    //! остановка расчетов фильтров
    void stop(void)
    {
        isStop=true;
    }
    //! старт
    void start(void)
    {
        isStop=false;
    }


    //! переход на ограничение
    Trigger<bool> *triggerOnLimits;

    //! Текущая позиция по горизонтали(1(влево);-1(вправо))
    double currentPosHor;
    //! Текущая позиция по вертикали
    double currentPosVer;

    //! Текущая позиция по горизонтали
    double predPosHor;
    //! Текущая позиция по вертикали
    double predPosVer;

    //! зона перемещения кнюппеля(прямоугольная)
    double maxHorPos;
    double maxVerPos;

    //! указателт на текущее значение положения кнюппеля
    //! скорость по горизонтали
    const float *speedHor;
    //! скорость по вертикали
    const float *speedVer;
    //! шаг расчета
    float step;
    //! признак останова
    bool isStop;

    //! закон изменения скорости
    ILawOfSpeed* currentLawSpeed;
    //!
    ILawOfSpeed* defaultSpeed;
    //! зона ограничений перемещения
    T* limits;
};

//! Четырех позиционная кнопка
template <class T> class Button4Positional
{
public:
    Button4Positional(const bool *left,const bool *right, const bool *up,const bool *down)
    {
        left_=left;
        right_=right;
        up_=up;
        down_=down;

        limits=new T;


        positionGorizontal_=0.0;
        positionVertical_=0.0;

        maxPositionLeft_=-1.0;
        maxPositionRight_=1.0;
        maxPositionUp_=-1.0;
        maxPositionDown_=1.0;

        maxPositionHor_=1.0;
        maxPositionVer_=1.0;

        maxSpeedLeft_=-1.0;
        maxSpeedRight_=1.0;

        maxSpeedDown_=1;
        maxSpeedUp_=-1;

    }
    //! необходим периодический вызов
    void refresh()
    {
        //! фильтр обработки по горизонтали
        filtrHorizontal();

        //! фильтр обработки по вертикали
        filtrVertical();

        limits->checkAngle(positionGorizontal_,positionVertical_,positionGorizontalPred_,positionVerticalPred_);
        /*if(positionVertical_<=0.0)
        {
            if((positionGorizontal_*positionGorizontal_+positionVertical_*positionVertical_)>1)
            {
                positionGorizontal_=positionGorizontalPred_;
                positionVertical_=positionVerticalPred_;
            }
        }
        if(positionVertical_>=0.2) positionVertical_=0.2;*/
    }
    double filtrHorizontal()
    {
        static double currentSpeed=0.0;
        double speedButton=0.0;

        positionGorizontalPred_=positionGorizontal_;

        if((*left_)==true) {speedButton=maxSpeedLeft_;maxPositionHor_=maxPositionLeft_;}
        else if((*right_)==true){ speedButton=maxSpeedRight_;maxPositionHor_=maxPositionRight_;}
        else {speedButton=0.0;currentSpeed=0.0;}

        currentSpeed=currentSpeed+cutValues(speedButton-currentSpeed)*0.01;

        if(positionGorizontal_>=0.0)
        {
            if(fabs(positionGorizontal_)>=fabs(maxPositionRight_)) positionGorizontal_=maxPositionRight_;
        }else if(positionGorizontal_<=0.0)
        {
            if(fabs(positionGorizontal_)>=fabs(maxPositionLeft_)) positionGorizontal_=maxPositionLeft_;
        }
        positionGorizontal_=positionGorizontal_+currentSpeed*0.01;
        return positionGorizontal_;
    }
    double filtrVertical()
    {
        static double currentSpeed=0.0;
        double speedButton=0.0;

        positionVerticalPred_=positionVertical_;

        if((*down_)==true) {speedButton=maxSpeedDown_;maxPositionVer_=maxPositionDown_;}
        else if((*up_)==true) {speedButton=maxSpeedUp_;maxPositionVer_=maxPositionUp_;}
        else {currentSpeed=0.0;speedButton=0.0;}

        currentSpeed=currentSpeed+cutValues(speedButton-currentSpeed)*0.01;

        if(positionVertical_>=0.0)
        {
            if(fabs(positionVertical_)>=fabs(maxPositionDown_)) positionVertical_=maxPositionDown_;
        }else if(positionVertical_<=0.0)
        {
            if(fabs(positionVertical_)>=fabs(maxPositionUp_)) positionVertical_=maxPositionUp_;
        }
        positionVertical_=positionVertical_+currentSpeed*0.01;

        return positionVertical_;
    }
    void resetPosition()
    {
        positionGorizontal_=0.0;
        positionVertical_=0.0;
    }
    //! Текущая позиция по горизонтали
    double positionGorizontal_;
    //! Текущая позиция по вертикали
    double positionVertical_;

    //! Текущая позиция по горизонтали
    double positionGorizontalPred_;
    //! Текущая позиция по вертикали
    double positionVerticalPred_;

    void currentGorPos()
    {
        return positionGorizontal_;
    }

    void currentVerPos()
    {
        return positionVertical_;
    }

    double maxPositionLeft_;
    double maxPositionRight_;
    double maxPositionUp_;
    double maxPositionDown_;

    double maxPositionHor_;
    double maxPositionVer_;

    double maxSpeedLeft_;
    double maxSpeedRight_;

    double maxSpeedDown_;
    double maxSpeedUp_;


    const bool *left_;
    const bool *right_;
    const bool *up_;
    const bool *down_;

    T* limits;
    //ILimitPosition *pos;
    //! not use
    double dt;

};


//! класс ограничения перемещения кнюппеля
//                                                    вверх-вниз    влево-вправо
//! ограничение должно быть нормированно также относит. [-1;1]    [-1;1]
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



#endif // TEMPLATENASP_HPP
