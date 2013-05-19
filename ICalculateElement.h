#ifndef ICALCULATEELEMENT_H
#define ICALCULATEELEMENT_H
#include <list>
#include <vector>
#include <QVariant>
#include "gl_func.h"
#include "debug_report.h"
#include "IEvent.h"

class ICalculateElement;
class TEventModel
{
public:
    //! приоритет события
    enum Priority{HIGH,LOW,MEDIUM} ;
    //! тип запроса
    enum TypeRequest
    {
        NONE,
        CROSS,
        MSG,
        CONTROL_STICK,
        ADDR_STATEMENT
    };
    //! тип процесса
    enum TypeProcess
    {
        NEW,
        REPLACEMENT
    };

    TEventModel()
    {
        //prio=TEventNASP::LOW;
        prio=TEventModel::LOW;
        process=TEventModel::NEW;
        uuid=0;
        answerUuid=0;
        uuid_old=0;
    }
    virtual TypeRequest typeEvent(){return type;}
    //! Тип запроса
    TypeRequest type;
    //! корневой отправитель
    ICalculateElement *rootSender;
    //! ближайший элемент который ретранслировал событие
    ICalculateElement *nodeSender;
    //! идентификатор
    int uuid;
    //! код обработки данного запроса
    int uuid_old;
    //! идентификатор ответа
    int answerUuid;
    //! значение
    QVariant value;
    //! приоритет
    Priority prio;
    //! тип процесса передачи события
    TypeProcess process;
};
//! событие орагны управления
class TEventControlStick:public TEventModel
{
public:
    TEventControlStick()
    {
        type=CONTROL_STICK;
    }
    float x_rus;
    float y_rus;
    int idObj;
    virtual TypeRequest typeEvent(){return type;}
};
//! событие с данными результата пересечения
class TEventNASPCross:public TEventModel
{
public:
    TEventNASPCross():TEventModel()
    {

    }
    double fi;//рад
    double lam;//рад
    double h;//метры
};
//! событие получить адрес структуры
class TEventAddrStat:public TEventModel
{
public:
    TEventAddrStat(QString msg_,ICalculateElement *root):TEventModel()
    {
        rootSender=root;
        msg=msg_;
        type=TEventModel::ADDR_STATEMENT;
        uuid=qrand();
        process=TEventModel::REPLACEMENT;
        addr=0;
    }
    QString msg;
    unsigned addr;
};
//! интерфейсный класс расчетного элемента
typedef struct TTimerIC_
{
    TTimerIC_()
    {
        run=false;
        id=0;
        msec=0;
        timeCount=0;
        numMSEC=0;
        maxCount=0;
    }
    void start()
    {
        run=true;
    }
    void stop()
    {
        run=false;
    }
    void restart()
    {
        timeCount=0;
        msec=0;
    }
    bool isStart()
    {
        return run;
    }

    //! идентификатор таймера
    unsigned long id;
    //! текущее значение таймера пересчитанное в тики
    unsigned long timeCount;
    //! максимальное значение таймера  в млс
    unsigned long numMSEC;
    //! максимальное значение таймера пересчитанное в тики
    unsigned long maxCount;
    //! запущен/остановлен
    bool run;
    //! текущее значение таймера в млс
    unsigned int msec;
}TTimerIC;
//! основной расчетный элемент
class ICalculateElement
{
public:
    enum Frec{Hz1,
              Hz6_25,
              Hz12_5,
              Hz25,
              Hz50,
              Hz100};

    ICalculateElement();
    //! инициализация
    virtual void init();
    //! метод основного расчета
    virtual void calculate()=0;
    //! перезагрузка оружия для моделей
    virtual void reloadWeapon();
    //! финальные действия при уже вычисленных переменных
    virtual void finite()=0;
    //! обработка события
    virtual void eEvent(TEventModel *event);
    //! вызов события для потомков
    void eEventChild(TEventModel *event);
    //! добавить сообщение в очередь  
    void setCountFrecDelay(long value);
    void setFrec(Frec f);
    //! частота в enum
    Frec frequency;

    double timeStep();

    //! создать таймер
    void createTimer(TTimerIC *timer, unsigned int numMSEC_);
    //! удалить таймер
    void deleteTimer(TTimerIC *timer);
    //! обновить таймеры
    void updateTimers(void);
    void startTimer(unsigned long numMSEC_);
    void stopTimer();

    //! вернуть кол-во млсек
    unsigned long msec();

    void setStop();
    void setStart();
    bool stop();
    bool start();

    void run();

    static void setStep(double s)
    {
        step=s;
        ticsTime++;
        allTimeWork+=step;
    }
    //! шаг вызова всех модулей (сек)
    static double step;
    //! кол-во тиков за все время работы
    static unsigned ticsTime;
    //! общее время
    static double allTimeWork;
    //! добавить модуль, который будет синхронизирован с этим объектом
    void addSynchroTask(ICalculateElement* ele);
    //! добавить элемент
    void addElement(ICalculateElement*);
    QVector<ICalculateElement* >* returnElements()
    {
        return &elements;
    }

    //! записать сообщение
    static void msg(QString idName_,QString strMsg)
    {
        if(writeOk==true)
            ptrToDbg()->addMsg(allTimeWork,idName_,strMsg);
    }
    static void msgRow(QString idName,QString strMsg)
    {
        if(writeOk==true)
            ptrToDbg()->addMsgInOneRow(strMsg);
    }

    virtual ~ICalculateElement();

    //! указатель на предыдущий элемент
    ICalculateElement* prev;
    //! признак того, что элемент начальный
    bool isBegin;
    //! признак того, что элемент конечный
    bool isEnd;
    //! указатель на следующий элемент
    ICalculateElement* next;
    static DebugReport * ptrToDbg()
    {
        if(debugRep==0)
            debugRep=new DebugReport;

        return debugRep;
    }
    static void switchOnWriteToFile(bool on){writeOk=on;}
    ICalculateElement* parent(){return parent_;}
private:
    //! список таймеров
    QList<TTimerIC*> timers;
    //! список с синхронизированными объектами
    QList<ICalculateElement* > synchroList;
    //! список подчиненных моделей
    QVector<ICalculateElement* >    elements;
    //! указатель на родителя
    ICalculateElement *parent_;
    //! список полученных сообщений
    //QVector<TEventModel> msgs;
    //! признак, что таймер запущен
    bool isTimerStart;

    //! счтечик времени
    unsigned long timeCount;
    unsigned long numMSEC;
    unsigned long maxCount;

    //! делитель частоты
    unsigned long countFrec;
    unsigned long maxCountFrec;
    bool stop_;
    bool start_;

    static DebugReport *debugRep;
    static bool writeOk;
};

//! класс стратегии распределенной загрузки
class IStrategyDistribLoading
{
public:
    IStrategyDistribLoading()
    {
        flushCounter();
    }
    void flushCounter();
    //! формирование массива задержек
    void formDelay(ICalculateElement* list,int num,int all);
    //! инициализировать начальные моменты старта
    void initTimeRuning(QVector<ICalculateElement* >  *elements);
private:
    int array[16];

    QVector<ICalculateElement* > el50Hz;
    QVector<ICalculateElement* > el25Hz;
    QVector<ICalculateElement* > el12_5Hz;
    QVector<ICalculateElement* > el6_25Hz;
};

#endif // ICALCULATEELEMENT_H
