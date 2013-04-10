#include "formstatusexitprogram.h"
#include "ui_formstatusexitprogram.h"

FormStatusExitProgram::FormStatusExitProgram(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormStatusExitProgram)
{
    ui->setupUi(this);
}

FormStatusExitProgram::~FormStatusExitProgram()
{
    delete ui;
}

void FormStatusExitProgram::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
void FormStatusExitProgram::setMainMsg(const QString& name)
{

    ui->labelMainMsg->setText(name);
}

void FormStatusExitProgram::setSecondMsg(const QString& name)
{
    ui->labelSecondMsg->setText(name);
}
void FormStatusExitProgram::count_signal()
{
    count++;
    ui->progressBar->setValue((count/((float)max))*100);
}
