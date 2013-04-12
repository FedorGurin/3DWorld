/********************************************************************************
** Form generated from reading UI file 'formstatusexitprogram.ui'
**
** Created: Thu 11. Apr 19:23:02 2013
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORMSTATUSEXITPROGRAM_H
#define UI_FORMSTATUSEXITPROGRAM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FormStatusExitProgram
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QLabel *labelMainMsg;
    QLabel *labelSecondMsg;
    QProgressBar *progressBar;

    void setupUi(QWidget *FormStatusExitProgram)
    {
        if (FormStatusExitProgram->objectName().isEmpty())
            FormStatusExitProgram->setObjectName(QString::fromUtf8("FormStatusExitProgram"));
        FormStatusExitProgram->resize(267, 79);
        gridLayout = new QGridLayout(FormStatusExitProgram);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        labelMainMsg = new QLabel(FormStatusExitProgram);
        labelMainMsg->setObjectName(QString::fromUtf8("labelMainMsg"));

        verticalLayout->addWidget(labelMainMsg);

        labelSecondMsg = new QLabel(FormStatusExitProgram);
        labelSecondMsg->setObjectName(QString::fromUtf8("labelSecondMsg"));

        verticalLayout->addWidget(labelSecondMsg);

        progressBar = new QProgressBar(FormStatusExitProgram);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(24);
        progressBar->setAlignment(Qt::AlignCenter);
        progressBar->setInvertedAppearance(false);
        progressBar->setTextDirection(QProgressBar::TopToBottom);

        verticalLayout->addWidget(progressBar);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(FormStatusExitProgram);

        QMetaObject::connectSlotsByName(FormStatusExitProgram);
    } // setupUi

    void retranslateUi(QWidget *FormStatusExitProgram)
    {
        FormStatusExitProgram->setWindowTitle(QApplication::translate("FormStatusExitProgram", "\320\241\320\276\321\205\321\200\320\260\320\275\320\265\320\275\320\270\320\265 \320\264\320\260\320\275\320\275\321\213\321\205..", 0, QApplication::UnicodeUTF8));
        labelMainMsg->setText(QApplication::translate("FormStatusExitProgram", "Field1", 0, QApplication::UnicodeUTF8));
        labelSecondMsg->setText(QApplication::translate("FormStatusExitProgram", "Field 2", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FormStatusExitProgram: public Ui_FormStatusExitProgram {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMSTATUSEXITPROGRAM_H
