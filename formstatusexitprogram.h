#ifndef FORMSTATUSEXITPROGRAM_H
#define FORMSTATUSEXITPROGRAM_H

#include <QWidget>

namespace Ui {
    class FormStatusExitProgram;
}

class FormStatusExitProgram : public QWidget {
    Q_OBJECT
public:
    FormStatusExitProgram(QWidget *parent = 0);
    ~FormStatusExitProgram();
    //! установка сообщения
    void setMainMsg(const QString& name);


    void setSecondMsg(const QString& name);


    //! общее кол-во вызовов
    void setCountMax(unsigned int max_)
    {
	count=0;
	max=max_;
    }
    void count_signal();

protected:
    void changeEvent(QEvent *e);

private:
    unsigned int max;
    unsigned count;
    Ui::FormStatusExitProgram *ui;
};

#endif // FORMSTATUSEXITPROGRAM_H
