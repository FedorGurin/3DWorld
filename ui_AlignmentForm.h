/********************************************************************************
** Form generated from reading UI file 'AlignmentForm.ui'
**
** Created: Tue 11. Jan 12:44:31 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALIGNMENTFORM_H
#define UI_ALIGNMENTFORM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDial>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_2;
    QDial *dial;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBox_4;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBox;
    QVBoxLayout *verticalLayout_3;
    QDial *dial_2;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBox_5;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QDoubleSpinBox *doubleSpinBox_2;
    QVBoxLayout *verticalLayout_5;
    QDial *dial_3;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QDoubleSpinBox *doubleSpinBox_6;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_6;
    QDoubleSpinBox *doubleSpinBox_3;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QString::fromUtf8("Form"));
        Form->resize(501, 246);
        gridLayout = new QGridLayout(Form);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        dial = new QDial(Form);
        dial->setObjectName(QString::fromUtf8("dial"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(dial->sizePolicy().hasHeightForWidth());
        dial->setSizePolicy(sizePolicy);
        dial->setMinimum(-100);
        dial->setMaximum(100);
        dial->setWrapping(false);
        dial->setNotchesVisible(true);

        verticalLayout_2->addWidget(dial);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_2 = new QLabel(Form);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(label_2);

        doubleSpinBox_4 = new QDoubleSpinBox(Form);
        doubleSpinBox_4->setObjectName(QString::fromUtf8("doubleSpinBox_4"));
        doubleSpinBox_4->setDecimals(8);
        doubleSpinBox_4->setMinimum(-1e+09);
        doubleSpinBox_4->setMaximum(1e+09);
        doubleSpinBox_4->setSingleStep(1e-06);
        doubleSpinBox_4->setValue(0);

        horizontalLayout->addWidget(doubleSpinBox_4);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(Form);
        label->setObjectName(QString::fromUtf8("label"));
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);

        horizontalLayout_2->addWidget(label);

        doubleSpinBox = new QDoubleSpinBox(Form);
        doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));
        doubleSpinBox->setDecimals(8);

        horizontalLayout_2->addWidget(doubleSpinBox);


        verticalLayout->addLayout(horizontalLayout_2);


        verticalLayout_2->addLayout(verticalLayout);


        gridLayout->addLayout(verticalLayout_2, 0, 0, 1, 1);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        dial_2 = new QDial(Form);
        dial_2->setObjectName(QString::fromUtf8("dial_2"));
        sizePolicy.setHeightForWidth(dial_2->sizePolicy().hasHeightForWidth());
        dial_2->setSizePolicy(sizePolicy);
        dial_2->setMinimum(-100);
        dial_2->setMaximum(100);
        dial_2->setWrapping(false);
        dial_2->setNotchesVisible(true);

        verticalLayout_3->addWidget(dial_2);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(Form);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(label_3);

        doubleSpinBox_5 = new QDoubleSpinBox(Form);
        doubleSpinBox_5->setObjectName(QString::fromUtf8("doubleSpinBox_5"));
        doubleSpinBox_5->setDecimals(8);
        doubleSpinBox_5->setMinimum(-1e+09);
        doubleSpinBox_5->setMaximum(1e+09);
        doubleSpinBox_5->setSingleStep(1e-06);
        doubleSpinBox_5->setValue(0);

        horizontalLayout_3->addWidget(doubleSpinBox_5);


        verticalLayout_4->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_4 = new QLabel(Form);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        sizePolicy1.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy1);

        horizontalLayout_4->addWidget(label_4);

        doubleSpinBox_2 = new QDoubleSpinBox(Form);
        doubleSpinBox_2->setObjectName(QString::fromUtf8("doubleSpinBox_2"));
        doubleSpinBox_2->setDecimals(8);

        horizontalLayout_4->addWidget(doubleSpinBox_2);


        verticalLayout_4->addLayout(horizontalLayout_4);


        verticalLayout_3->addLayout(verticalLayout_4);


        gridLayout->addLayout(verticalLayout_3, 0, 1, 1, 1);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        dial_3 = new QDial(Form);
        dial_3->setObjectName(QString::fromUtf8("dial_3"));
        sizePolicy.setHeightForWidth(dial_3->sizePolicy().hasHeightForWidth());
        dial_3->setSizePolicy(sizePolicy);
        dial_3->setMinimum(-100);
        dial_3->setMaximum(100);
        dial_3->setWrapping(false);
        dial_3->setNotchesVisible(true);

        verticalLayout_5->addWidget(dial_3);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_5 = new QLabel(Form);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        sizePolicy1.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy1);

        horizontalLayout_5->addWidget(label_5);

        doubleSpinBox_6 = new QDoubleSpinBox(Form);
        doubleSpinBox_6->setObjectName(QString::fromUtf8("doubleSpinBox_6"));
        doubleSpinBox_6->setDecimals(8);
        doubleSpinBox_6->setMinimum(-1e+09);
        doubleSpinBox_6->setMaximum(1e+09);
        doubleSpinBox_6->setSingleStep(1e-06);
        doubleSpinBox_6->setValue(0);

        horizontalLayout_5->addWidget(doubleSpinBox_6);


        verticalLayout_6->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_6 = new QLabel(Form);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        sizePolicy1.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy1);

        horizontalLayout_6->addWidget(label_6);

        doubleSpinBox_3 = new QDoubleSpinBox(Form);
        doubleSpinBox_3->setObjectName(QString::fromUtf8("doubleSpinBox_3"));
        doubleSpinBox_3->setDecimals(8);

        horizontalLayout_6->addWidget(doubleSpinBox_3);


        verticalLayout_6->addLayout(horizontalLayout_6);


        verticalLayout_5->addLayout(verticalLayout_6);


        gridLayout->addLayout(verticalLayout_5, 0, 2, 1, 1);


        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Form", "\320\250\320\260\320\263", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Form", "\320\246\320\265\320\275\321\202\321\200", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Form", "\320\250\320\260\320\263", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Form", "\320\246\320\265\320\275\321\202\321\200", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Form", "\320\250\320\260\320\263", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Form", "\320\246\320\265\320\275\321\202\321\200", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALIGNMENTFORM_H
