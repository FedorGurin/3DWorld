
#include "ICalculateElement.h"

double ICalculateElement::step=0.01;
unsigned ICalculateElement::ticsTime=0;
double ICalculateElement::allTimeWork=0.0;
bool ICalculateElement::writeOk=false;
DebugReport* ICalculateElement::debugRep=0;
////////////////////////////////////////////////////////////////////////////////////
ICalculateElement::ICalculateElement()
{
    timers.clear();
    synchroList.clear();

    parent_=0;
    prev=0;
    next=0;

    isBegin=false;
    isEnd=false;

    countFrec=1;

    isTimerStart=false;
    stop_=false;
    start_=true;

    writeOk=false;

    setFrec(ICalculateElement::Hz100);
}
//! обработка события
void ICalculateElement::eEvent(TEventModel *event)
{
    ICalculateElement *oldSender;
    oldSender=event->nodeSender;
    event->nodeSender=this;
    if(event->rootSender==this)
    {
        if(prev!=0)
            prev->eEvent(event);
        event->nodeSender=this;
        if(next!=0)
            next->eEvent(event);

        if(next==0)
        {
            for(int i=0;i<elements.size();i++)
            {
                elements[i]->eEvent(event);
            }
        }
    }else
    {
        if(next!=0)
        {
            if(oldSender==prev)
            {
                next->eEvent(event);
            }
        }
        if(next==0)
        {
            for(int i=0;i<elements.size();i++)
            {
                elements[i]->eEvent(event);
            }
        }
        if(prev!=0)
        {
            if(oldSender==next)
            {
                prev->eEvent(event);
            }
        }
    }
}
//! отправка сообщения потомкам
void ICalculateElement::eEventChild(TEventModel *event)
{
    if(elements.isEmpty()==false)
    {
        elements.first()->eEvent(event);
    }
}
//! этап связывания моделей между собой
/*void ICalculateElement::bind()
{

}*/
void ICalculateElement::init()
{
    for(int i=0;i<elements.size();i++)
        elements[i]->init();    
}
//! перезагрузка оружия для моделей
void ICalculateElement::reloadWeapon()
{
    for(long i=0;i<elements.size();i++) elements[i]->reloadWeapon();
}
void ICalculateElement::addElement(ICalculateElement* element)
{
    if(elements.isEmpty()==false)
    {
        element->isEnd=true;
        element->prev=elements.back();
        //! за кольцовываем буфер
        //element->next=elements.first();
    }
    else
    {
        element->prev=0;
        isBegin=true;
        isEnd=true;
    }

    if(elements.isEmpty()==false)
    {
        (elements.back())->next=element;
        (elements.back())->isEnd=false;
    }

    elements.push_back(element);
    element->parent_=this;
}
void ICalculateElement::setCountFrecDelay(long value)
{
    countFrec=maxCountFrec-value;
}
/*void ICalculateElement::msg(QString idName_,QString strMsg)
{
    ICalculateElement::debugRep.addMsg(allTimeWork,idName_,strMsg);
}*/

void ICalculateElement::setFrec(Frec f)
{

    frequency=f;

    switch(frequency)
    {
    case ICalculateElement::Hz100:  {maxCountFrec=1;break;}
    case ICalculateElement::Hz50:   {maxCountFrec=2;break;}
    case ICalculateElement::Hz25:   {maxCountFrec=4;break;}
    case ICalculateElement::Hz12_5: {maxCountFrec=8;break;}
    case ICalculateElement::Hz6_25: {maxCountFrec=16;break;}
    case ICalculateElement::Hz1:    {maxCountFrec=100;break;}
    default:                        {maxCountFrec=1;}
    };
}
double ICalculateElement::timeStep()
{
   return step*maxCountFrec;
}

void ICalculateElement::deleteTimer(TTimerIC *timer)
{
    timers.removeAll(timer);
}

//! создать таймер
void ICalculateElement::createTimer(TTimerIC *timer, unsigned int numMSEC_)
{
    timers.removeAll(timer);
    timer->maxCount=(numMSEC_*0.001/(step));
    timer->numMSEC=numMSEC_;
    timer->timeCount=0;
    timer->run=false;
    timer->msec=timer->timeCount*step/0.001;

    timers.push_back(timer);

}
//! обновить таймеры
void ICalculateElement::updateTimers(void)
{
    for(int i=0;i<timers.size();i++)
    {
        TTimerIC *timer=timers[i];
        if(timer->timeCount<timer->maxCount && timer->run==true)
        {
            timer->timeCount++;
            timer->msec=timer->timeCount*step/0.001;
        }
    }
}

void ICalculateElement::startTimer(unsigned long numMSEC_)
{
    isTimerStart=true;

    numMSEC=numMSEC_;
    maxCount=(numMSEC/step)+1;
    timeCount=0;
}
void ICalculateElement::stopTimer()
{
    isTimerStart=false;
    numMSEC=0;
    timeCount=0;
}

//! вернуть кол-во млсек
unsigned long ICalculateElement::msec()
{
    return step*timeCount/0.001;
}

void ICalculateElement::setStop()
{
    stop_=true;
    start_=false;
}
void ICalculateElement::setStart()
{
    stop_=false;
    start_=true;
}
bool ICalculateElement::stop(){return stop_;}
bool ICalculateElement::start(){return start_;}

void ICalculateElement::run()
{
    for(int i=0;i<synchroList.size();i++)
    {
        synchroList[i]->run();
    }
    for(long i=0;i<elements.size();i++) elements[i]->run();

    if(start_==true)
    {
        updateTimers();

        if(isTimerStart==true)
        {
            timeCount++;

            if(timeCount>maxCount)
            {
                isTimerStart=false;
            }
        }
        if(countFrec==maxCountFrec)
        {
            calculate();
            finite();

            countFrec=0;

        }
        countFrec++;
    }else countFrec=0;
}


//! добавить модуль, который будет синхронизирован с этим объектом
void ICalculateElement::addSynchroTask(ICalculateElement* ele)
{
    synchroList.push_back(ele);
}
ICalculateElement::~ICalculateElement()
{
    qDebug("virtual destructor = class ICalculateElement\n");
    /*for(int i=0;i<synchroList.size();i++)  delete synchroList[i];
    synchroList.clear();*/
    //for(int i=0;i<elements.size();i++) delete elements[i];

    //for(int i=0;i<timers.size();i++)  delete timers[i];
    timers.clear();
}

void IStrategyDistribLoading::flushCounter()
{
    for(int i=0;i<16;i++) array[i]=0;
    el50Hz.clear();
    el25Hz.clear();
    el12_5Hz.clear();
    el6_25Hz.clear();
}

//! формирование массива
void IStrategyDistribLoading::formDelay(ICalculateElement* list,int num,int all)
{
    int min=array[0];
    int minIndex=0;
    for(int j=0;j<num;j++)
    {
        if(array[j]<min)
        {
            min=array[j];
            minIndex=j;
        }
    }
    for(int i=minIndex;i<all;i+=num)
    {
        array[i]++;
    }
    list->setCountFrecDelay(minIndex);
}

//! инициализировать начальные моменты старта
void IStrategyDistribLoading::initTimeRuning(QVector<ICalculateElement* >  *elements)
{
    flushCounter();
    for(int i=0;i<elements->size();i++)
    {
        if((*elements)[i]->frequency==ICalculateElement::Hz50)
        {
            el50Hz.push_back((*elements)[i]);
        }
        if((*elements)[i]->frequency==ICalculateElement::Hz25)
        {
            el25Hz.push_back((*elements)[i]);
        }
        if((*elements)[i]->frequency==ICalculateElement::Hz12_5)
        {
            el12_5Hz.push_back((*elements)[i]);
        }
        if((*elements)[i]->frequency==ICalculateElement::Hz6_25)
        {
            el6_25Hz.push_back((*elements)[i]);
        }
    }

    for(int i=0;i<el50Hz.size();i++)        formDelay(el50Hz[i],2,16);
    for(int i=0;i<el25Hz.size();i++)        formDelay(el25Hz[i],4,16);
    for(int i=0;i<el12_5Hz.size();i++)      formDelay(el12_5Hz[i],8,16);
    for(int i=0;i<el6_25Hz.size();i++)      formDelay(el6_25Hz[i],16,16);
}


/*TAngle LimitPositionKAI::limitAngle(TAngle angle,TAngle angleCenter)
{
    TAngle an;
    //if(angle.tau<0.0)
    //{
        if(sqrt(angle.sigma*angle.sigma+angle.tau*angle.tau)>GradToRadian(12))
        {
            double x=0.0,y=0.0;
            double R=GradToRadian(12.0);
            if((angle.sigma-angleCenter.sigma)>1)
            {
                 double a=(angle.tau-angleCenter.tau)/(angle.sigma-angleCenter.sigma);
            double b=angleCenter.tau-a*angleCenter.sigma;

            double x1=(-2*a*b+sqrt(2*a*b+4*(a*a+1)*(R*R-b*b)))/(2*(a*a+1));
            double x2=(-2*a*b-sqrt(2*a*b+4*(a*a+1)*(R*R-b*b)))/(2*(a*a+1));
            double y1=a*x1+b;
            double y2=a*x2+b;

            double d1=sqrt(angle.sigma*x1+angle.tau*y1);
            double d2=sqrt(angle.sigma*x2+angle.tau*y2);

            if(d1>d2) {y=y2;x=x2;}
            else {y=y1;x=x1;}

        }else
        {
            x=angle.sigma;
            y=-sqrt(R*R-x*x);
        }
            an.sigma=x;
            an.tau=y;

            return an;
        }

    return angle;
}*/

