/********************************************************************************
** Form generated from reading UI file 'formsettings.ui'
**
** Created: Mon 20. May 21:20:48 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORMSETTINGS_H
#define UI_FORMSETTINGS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FormSettings
{
public:
    QGridLayout *gridLayout_4;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QCheckBox *checkBoxVertLine;
    QCheckBox *checkBox;
    QCheckBox *checkBoxILS;
    QCheckBox *checkBoxLand;
    QCheckBox *checkBoxInfo;
    QCheckBox *checkBoxFog;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QDoubleSpinBox *doubleSpinBoxFog;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QDoubleSpinBox *doubleSpinBoxDeepFog;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QSpinBox *spinBoxFreq;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBoxMulTime;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_2;
    QRadioButton *radioButtonLinear;
    QRadioButton *radioButtonSpline;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QPushButton *pushButtonLoadData;
    QPushButton *pushButtonLoadLand;

    void setupUi(QWidget *FormSettings)
    {
        if (FormSettings->objectName().isEmpty())
            FormSettings->setObjectName(QString::fromUtf8("FormSettings"));
        FormSettings->resize(233, 495);
        gridLayout_4 = new QGridLayout(FormSettings);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox = new QGroupBox(FormSettings);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        checkBoxVertLine = new QCheckBox(groupBox);
        checkBoxVertLine->setObjectName(QString::fromUtf8("checkBoxVertLine"));

        gridLayout_3->addWidget(checkBoxVertLine, 0, 0, 1, 1);

        checkBox = new QCheckBox(groupBox);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        gridLayout_3->addWidget(checkBox, 1, 0, 1, 1);

        checkBoxILS = new QCheckBox(groupBox);
        checkBoxILS->setObjectName(QString::fromUtf8("checkBoxILS"));
        checkBoxILS->setChecked(true);

        gridLayout_3->addWidget(checkBoxILS, 2, 0, 1, 1);

        checkBoxLand = new QCheckBox(groupBox);
        checkBoxLand->setObjectName(QString::fromUtf8("checkBoxLand"));
        checkBoxLand->setChecked(true);

        gridLayout_3->addWidget(checkBoxLand, 3, 0, 1, 1);

        checkBoxInfo = new QCheckBox(groupBox);
        checkBoxInfo->setObjectName(QString::fromUtf8("checkBoxInfo"));

        gridLayout_3->addWidget(checkBoxInfo, 4, 0, 1, 1);

        checkBoxFog = new QCheckBox(groupBox);
        checkBoxFog->setObjectName(QString::fromUtf8("checkBoxFog"));

        gridLayout_3->addWidget(checkBoxFog, 5, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_4->addWidget(label_4);

        doubleSpinBoxFog = new QDoubleSpinBox(groupBox);
        doubleSpinBoxFog->setObjectName(QString::fromUtf8("doubleSpinBoxFog"));
        doubleSpinBoxFog->setDecimals(3);
        doubleSpinBoxFog->setMaximum(1);
        doubleSpinBoxFog->setSingleStep(0.01);
        doubleSpinBoxFog->setValue(0.5);

        horizontalLayout_4->addWidget(doubleSpinBoxFog);


        gridLayout_3->addLayout(horizontalLayout_4, 6, 0, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_5->addWidget(label_5);

        doubleSpinBoxDeepFog = new QDoubleSpinBox(groupBox);
        doubleSpinBoxDeepFog->setObjectName(QString::fromUtf8("doubleSpinBoxDeepFog"));

        horizontalLayout_5->addWidget(doubleSpinBoxDeepFog);


        gridLayout_3->addLayout(horizontalLayout_5, 7, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        doubleSpinBox = new QDoubleSpinBox(groupBox);
        doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));
        doubleSpinBox->setMinimum(-1e+09);
        doubleSpinBox->setMaximum(1e+09);
        doubleSpinBox->setSingleStep(0);
        doubleSpinBox->setValue(1);

        horizontalLayout->addWidget(doubleSpinBox);


        gridLayout_3->addLayout(horizontalLayout, 8, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        spinBoxFreq = new QSpinBox(groupBox);
        spinBoxFreq->setObjectName(QString::fromUtf8("spinBoxFreq"));
        spinBoxFreq->setMaximum(100);
        spinBoxFreq->setValue(20);

        horizontalLayout_2->addWidget(spinBoxFreq);


        gridLayout_3->addLayout(horizontalLayout_2, 9, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        doubleSpinBoxMulTime = new QDoubleSpinBox(groupBox);
        doubleSpinBoxMulTime->setObjectName(QString::fromUtf8("doubleSpinBoxMulTime"));
        doubleSpinBoxMulTime->setMinimum(-100);
        doubleSpinBoxMulTime->setMaximum(100);
        doubleSpinBoxMulTime->setValue(1);

        horizontalLayout_3->addWidget(doubleSpinBoxMulTime);


        gridLayout_3->addLayout(horizontalLayout_3, 10, 0, 1, 1);

        groupBox_3 = new QGroupBox(groupBox);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        gridLayout_2 = new QGridLayout(groupBox_3);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        radioButtonLinear = new QRadioButton(groupBox_3);
        radioButtonLinear->setObjectName(QString::fromUtf8("radioButtonLinear"));
        radioButtonLinear->setChecked(true);

        gridLayout_2->addWidget(radioButtonLinear, 0, 0, 1, 1);

        radioButtonSpline = new QRadioButton(groupBox_3);
        radioButtonSpline->setObjectName(QString::fromUtf8("radioButtonSpline"));

        gridLayout_2->addWidget(radioButtonSpline, 1, 0, 1, 1);


        gridLayout_3->addWidget(groupBox_3, 11, 0, 1, 1);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(FormSettings);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pushButtonLoadData = new QPushButton(groupBox_2);
        pushButtonLoadData->setObjectName(QString::fromUtf8("pushButtonLoadData"));

        gridLayout->addWidget(pushButtonLoadData, 0, 0, 1, 1);

        pushButtonLoadLand = new QPushButton(groupBox_2);
        pushButtonLoadLand->setObjectName(QString::fromUtf8("pushButtonLoadLand"));

        gridLayout->addWidget(pushButtonLoadLand, 1, 0, 1, 1);


        verticalLayout->addWidget(groupBox_2);


        gridLayout_4->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(FormSettings);

        QMetaObject::connectSlotsByName(FormSettings);
    } // setupUi

    void retranslateUi(QWidget *FormSettings)
    {
        FormSettings->setWindowTitle(QString());
        groupBox->setTitle(QApplication::translate("FormSettings", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\270", 0, QApplication::UnicodeUTF8));
        checkBoxVertLine->setText(QApplication::translate("FormSettings", "\320\262\320\265\321\200\321\202\320\270\320\272\320\260\320\273\321\214\320\275\321\213\320\265 \320\273\320\270\320\275\320\270\320\270", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("FormSettings", "\321\201\320\265\321\202\320\265\320\262\320\276\320\271 \320\276\320\261\320\274\320\265\320\275", 0, QApplication::UnicodeUTF8));
        checkBoxILS->setText(QApplication::translate("FormSettings", "\320\276\321\202\321\200\320\270\321\201\320\276\320\262\320\272\320\260 \320\230\320\233\320\241", 0, QApplication::UnicodeUTF8));
        checkBoxLand->setText(QApplication::translate("FormSettings", "\320\276\321\202\321\200\320\270\321\201\320\276\320\262\320\272\320\260 \320\273\320\260\320\275\320\264\321\210\320\260\321\204\321\202\320\260", 0, QApplication::UnicodeUTF8));
        checkBoxInfo->setText(QApplication::translate("FormSettings", "\320\270\320\275\321\204\320\276\321\200\320\274\320\260\321\206\320\270\321\217", 0, QApplication::UnicodeUTF8));
        checkBoxFog->setText(QApplication::translate("FormSettings", "\321\202\321\203\320\274\320\260\320\275", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("FormSettings", "\320\237\321\200\320\276\320\267\321\200\320\260\321\207\320\275\320\276\321\201\321\202\321\214 \321\202\321\203\320\274\320\260\320\275\320\260", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("FormSettings", "\320\223\320\273\321\203\320\261\320\270\320\275\320\260 \321\202\321\203\320\274\320\260\320\275\320\260", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("FormSettings", "\320\234\320\260\321\201\321\210\321\202\320\260\320\261", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("FormSettings", "\320\247\320\260\321\201\321\202\320\276\321\202\320\260 \320\272\320\260\320\264\321\200\320\276\320\262(\320\276\321\202\321\200\320\270\321\201\320\276\320\262\320\272\320\260)", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("FormSettings", "\320\234\320\275\320\276\320\266\320\270\321\202\320\265\320\273\321\214 \320\262\321\200\320\265\320\274\320\265\320\275\320\270", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("FormSettings", "\320\230\320\275\321\202\320\265\321\200\320\277\320\276\320\273\321\217\321\206\320\270\321\217", 0, QApplication::UnicodeUTF8));
        radioButtonLinear->setText(QApplication::translate("FormSettings", "\320\273\320\270\320\275\320\265\320\271\320\275\320\260\321\217", 0, QApplication::UnicodeUTF8));
        radioButtonSpline->setText(QApplication::translate("FormSettings", "\321\201\320\277\320\273\320\260\320\271\320\275", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("FormSettings", "\320\227\320\260\320\263\321\200\321\203\320\267\320\272\320\260", 0, QApplication::UnicodeUTF8));
        pushButtonLoadData->setText(QApplication::translate("FormSettings", "\320\264\320\260\320\275\320\275\321\213\320\265 \320\277\320\276\320\273\320\265\321\202\320\260", 0, QApplication::UnicodeUTF8));
        pushButtonLoadLand->setText(QApplication::translate("FormSettings", "\320\273\320\260\320\275\320\264\321\210\320\260\321\204\321\202", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FormSettings: public Ui_FormSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMSETTINGS_H
