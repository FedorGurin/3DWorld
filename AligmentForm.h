#ifndef ALIGMENTFORM_H
#define ALIGMENTFORM_H

#include <QWidget>

namespace Ui {
    class AligmentForm;
}

class AligmentForm : public QWidget
{
    Q_OBJECT


public:
    explicit AligmentForm(QWidget *parent = 0);
    //void setCurrentTeta(double);
    //void setCurrentPsi(double);
    //void setCurrentGamma(double);
    void setCurrentResolution(int w,int h);
    void setBeginFov(double);
    void setBeginPos(QPoint p);

    ~AligmentForm();
signals:
    void changeTeta(double);
    void changePsi(double);
    void changeGamma(double);
    void changeFov(double);
    void changeCoord(double,double,double);
    void changeResolution(int w,int h);
    void readCurrentRes();
    void changePosWindow(int x,int y);
private:
    Ui::AligmentForm *ui;
    double x;
    double y;
    double z;
public slots:
    void slotChangeTeta(int);
    void slotChangePsi(int);
    void slotChangeGamma(int);
    void slotChangeFov(int);//SpinBoxX
    void slotChangeX(double x);
    void slotChangeY(double y);
    void slotChangeZ(double z);
    void slotChangeResolution(void);
    void slotPushButtonLEFT();
    void slotPushButtonRIGHT();
    void slotPushButtonDOWN();
    void slotPushButtonUP();


};

#endif // ALIGMENTFORM_H
