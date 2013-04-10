#include "AligmentForm.h"
#include "ui_aligmentform.h"
#include <math.h>
AligmentForm::AligmentForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AligmentForm)
{
    ui->setupUi(this);
    x=0;y=0;z=0;
    connect(ui->dialGamma,SIGNAL(valueChanged(int)),this,SLOT(slotChangeGamma(int)));
    connect(ui->dialPsi,SIGNAL(valueChanged(int)),this,SLOT(slotChangePsi(int)));
    connect(ui->dialTeta,SIGNAL(valueChanged(int)),this,SLOT(slotChangeTeta(int)));
    connect(ui->dialFov,SIGNAL(valueChanged(int)),this,SLOT(slotChangeFov(int)));
    connect(ui->pushButtonCurRes,SIGNAL(clicked()),this,SIGNAL(readCurrentRes()));
    connect(ui->pushButtonSetRes,SIGNAL(clicked()),this,SLOT(slotChangeResolution()));
    connect(ui->pushButtonDOWN,SIGNAL(clicked()),this,SLOT(slotPushButtonDOWN()));
    connect(ui->pushButtonRIGHT,SIGNAL(clicked()),this,SLOT(slotPushButtonRIGHT()));
    connect(ui->pushButtonLEFT,SIGNAL(clicked()),this,SLOT(slotPushButtonLEFT()));
    connect(ui->pushButtonUP,SIGNAL(clicked()),this,SLOT(slotPushButtonUP()));

    connect(ui->doubleSpinBoxX,SIGNAL(valueChanged(double)),this,SLOT(slotChangeX(double)));
    connect(ui->doubleSpinBoxY,SIGNAL(valueChanged(double)),this,SLOT(slotChangeY(double)));
    connect(ui->doubleSpinBoxZ,SIGNAL(valueChanged(double)),this,SLOT(slotChangeZ(double)));

}
void AligmentForm::setCurrentResolution(int w,int h)
{
    ui->spinBoxH->setValue(h);
    ui->spinBoxW->setValue(w);
}
void AligmentForm::setBeginPos(QPoint p)
{
    ui->spinBoxX->setValue(p.x());
    ui->spinBoxY->setValue(p.y());
}
void AligmentForm::setBeginFov(double value)
{
    ui->doubleSpinBoxCenterFov->setValue(value*180.0/M_PI);
}
void AligmentForm::slotChangeTeta(int v)
{
    double value=v*ui->doubleSpinBoxStepTeta->value()+ui->doubleSpinBoxCenterTeta->value();
    ui->doubleSpinBoxTeta->setValue(value);
    emit changeTeta(value);
}
void AligmentForm::slotChangeResolution()
{
    emit changeResolution(ui->spinBoxW->value(),ui->spinBoxH->value());
}

void AligmentForm::slotChangePsi(int v)
{
    double value=v*ui->doubleSpinBoxStepPsi->value()+ui->doubleSpinBoxCenterPsi->value();
    ui->doubleSpinBoxPsi->setValue(value);
    emit changePsi(value);
}

void AligmentForm::slotChangeGamma(int v)
{
    double value=v*ui->doubleSpinBoxStepGamma->value()+ui->doubleSpinBoxCenterGamma->value();
    ui->doubleSpinBoxGamma->setValue(value);
    emit changeGamma(value);
}
void AligmentForm::slotChangeX(double x_)
{
    x=x_;
    emit changeCoord(x,y,z);
}

void AligmentForm::slotChangeY(double y_)
{
    y=y_;
    emit changeCoord(x,y,z);
}

void AligmentForm::slotChangeZ(double z_)
{
    z=z_;
    emit changeCoord(x,y,z);
}


void AligmentForm::slotPushButtonLEFT()
{
    ui->spinBoxX->setValue(ui->spinBoxX->value()-1);
    emit changePosWindow(ui->spinBoxX->value(),ui->spinBoxY->value());
}

void AligmentForm::slotPushButtonRIGHT()
{
    ui->spinBoxX->setValue(ui->spinBoxX->value()+1);
    emit changePosWindow(ui->spinBoxX->value(),ui->spinBoxY->value());
}

void AligmentForm::slotPushButtonDOWN()
{
    ui->spinBoxY->setValue(ui->spinBoxY->value()-1);
    emit changePosWindow(ui->spinBoxX->value(),ui->spinBoxY->value());
}

void AligmentForm::slotPushButtonUP()
{
    ui->spinBoxY->setValue(ui->spinBoxY->value()+1);
    emit changePosWindow(ui->spinBoxX->value(),ui->spinBoxY->value());
}

void AligmentForm::slotChangeFov(int v)
{
    double value=v*ui->doubleSpinBoxStepFov->value()+ui->doubleSpinBoxCenterFov->value();
    ui->doubleSpinBoxFov->setValue(value);
    emit changeFov(value);
}

AligmentForm::~AligmentForm()
{
    delete ui;
}
