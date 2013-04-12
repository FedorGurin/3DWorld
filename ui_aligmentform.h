/********************************************************************************
** Form generated from reading UI file 'aligmentform.ui'
**
** Created: Thu 11. Apr 19:23:02 2013
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALIGMENTFORM_H
#define UI_ALIGMENTFORM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDial>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AligmentForm
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_7;
    QLabel *label_13;
    QDial *dialFov;
    QVBoxLayout *verticalLayout_8;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_14;
    QDoubleSpinBox *doubleSpinBoxFov;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_15;
    QDoubleSpinBox *doubleSpinBoxStepFov;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_16;
    QDoubleSpinBox *doubleSpinBoxCenterFov;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_11;
    QDial *dialTeta;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_12;
    QDoubleSpinBox *doubleSpinBoxTeta;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBoxStepTeta;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBoxCenterTeta;
    QWidget *layoutWidget2;
    QVBoxLayout *verticalLayout_6;
    QLabel *label_9;
    QDial *dialGamma;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_10;
    QDoubleSpinBox *doubleSpinBoxGamma;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QDoubleSpinBox *doubleSpinBoxStepGamma;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QDoubleSpinBox *doubleSpinBoxCenterGamma;
    QWidget *layoutWidget3;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_8;
    QDial *dialPsi;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_7;
    QDoubleSpinBox *doubleSpinBoxPsi;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QDoubleSpinBox *doubleSpinBoxStepPsi;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_6;
    QDoubleSpinBox *doubleSpinBoxCenterPsi;
    QGroupBox *groupBox;
    QWidget *layoutWidget4;
    QHBoxLayout *horizontalLayout_13;
    QVBoxLayout *verticalLayout_9;
    QLabel *label_17;
    QDoubleSpinBox *doubleSpinBoxX;
    QVBoxLayout *verticalLayout_10;
    QLabel *label_18;
    QDoubleSpinBox *doubleSpinBoxY;
    QVBoxLayout *verticalLayout_11;
    QLabel *label_19;
    QDoubleSpinBox *doubleSpinBoxZ;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_20;
    QSpinBox *spinBoxW;
    QHBoxLayout *horizontalLayout_15;
    QLabel *label_21;
    QSpinBox *spinBoxH;
    QHBoxLayout *horizontalLayout_16;
    QPushButton *pushButtonSetRes;
    QPushButton *pushButtonCurRes;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout_13;
    QVBoxLayout *verticalLayout_12;
    QHBoxLayout *horizontalLayout_20;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButtonUP;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_19;
    QPushButton *pushButtonLEFT;
    QPushButton *pushButtonRIGHT;
    QHBoxLayout *horizontalLayout_21;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *pushButtonDOWN;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_22;
    QHBoxLayout *horizontalLayout_17;
    QLabel *label_22;
    QSpinBox *spinBoxX;
    QHBoxLayout *horizontalLayout_18;
    QLabel *label_23;
    QSpinBox *spinBoxY;

    void setupUi(QWidget *AligmentForm)
    {
        if (AligmentForm->objectName().isEmpty())
            AligmentForm->setObjectName(QString::fromUtf8("AligmentForm"));
        AligmentForm->resize(774, 486);
        layoutWidget = new QWidget(AligmentForm);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(490, 10, 157, 207));
        verticalLayout_7 = new QVBoxLayout(layoutWidget);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        label_13 = new QLabel(layoutWidget);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_13->sizePolicy().hasHeightForWidth());
        label_13->setSizePolicy(sizePolicy);
        label_13->setAlignment(Qt::AlignCenter);

        verticalLayout_7->addWidget(label_13);

        dialFov = new QDial(layoutWidget);
        dialFov->setObjectName(QString::fromUtf8("dialFov"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(dialFov->sizePolicy().hasHeightForWidth());
        dialFov->setSizePolicy(sizePolicy1);
        dialFov->setMinimum(-100);
        dialFov->setMaximum(100);
        dialFov->setWrapping(false);
        dialFov->setNotchesVisible(true);

        verticalLayout_7->addWidget(dialFov);

        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        label_14 = new QLabel(layoutWidget);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        horizontalLayout_10->addWidget(label_14);

        doubleSpinBoxFov = new QDoubleSpinBox(layoutWidget);
        doubleSpinBoxFov->setObjectName(QString::fromUtf8("doubleSpinBoxFov"));
        doubleSpinBoxFov->setDecimals(6);
        doubleSpinBoxFov->setMinimum(-1e+09);
        doubleSpinBoxFov->setMaximum(1e+09);

        horizontalLayout_10->addWidget(doubleSpinBoxFov);


        verticalLayout_8->addLayout(horizontalLayout_10);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        label_15 = new QLabel(layoutWidget);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        QSizePolicy sizePolicy2(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_15->sizePolicy().hasHeightForWidth());
        label_15->setSizePolicy(sizePolicy2);

        horizontalLayout_11->addWidget(label_15);

        doubleSpinBoxStepFov = new QDoubleSpinBox(layoutWidget);
        doubleSpinBoxStepFov->setObjectName(QString::fromUtf8("doubleSpinBoxStepFov"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Maximum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(doubleSpinBoxStepFov->sizePolicy().hasHeightForWidth());
        doubleSpinBoxStepFov->setSizePolicy(sizePolicy3);
        doubleSpinBoxStepFov->setDecimals(8);
        doubleSpinBoxStepFov->setMinimum(-1e+09);
        doubleSpinBoxStepFov->setMaximum(1e+09);
        doubleSpinBoxStepFov->setSingleStep(1e-06);
        doubleSpinBoxStepFov->setValue(0);

        horizontalLayout_11->addWidget(doubleSpinBoxStepFov);


        verticalLayout_8->addLayout(horizontalLayout_11);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        label_16 = new QLabel(layoutWidget);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        sizePolicy2.setHeightForWidth(label_16->sizePolicy().hasHeightForWidth());
        label_16->setSizePolicy(sizePolicy2);

        horizontalLayout_12->addWidget(label_16);

        doubleSpinBoxCenterFov = new QDoubleSpinBox(layoutWidget);
        doubleSpinBoxCenterFov->setObjectName(QString::fromUtf8("doubleSpinBoxCenterFov"));
        sizePolicy3.setHeightForWidth(doubleSpinBoxCenterFov->sizePolicy().hasHeightForWidth());
        doubleSpinBoxCenterFov->setSizePolicy(sizePolicy3);
        doubleSpinBoxCenterFov->setDecimals(8);

        horizontalLayout_12->addWidget(doubleSpinBoxCenterFov);


        verticalLayout_8->addLayout(horizontalLayout_12);


        verticalLayout_7->addLayout(verticalLayout_8);

        layoutWidget1 = new QWidget(AligmentForm);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(9, 9, 157, 207));
        verticalLayout_3 = new QVBoxLayout(layoutWidget1);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_11 = new QLabel(layoutWidget1);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        sizePolicy.setHeightForWidth(label_11->sizePolicy().hasHeightForWidth());
        label_11->setSizePolicy(sizePolicy);
        label_11->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_11);

        dialTeta = new QDial(layoutWidget1);
        dialTeta->setObjectName(QString::fromUtf8("dialTeta"));
        sizePolicy1.setHeightForWidth(dialTeta->sizePolicy().hasHeightForWidth());
        dialTeta->setSizePolicy(sizePolicy1);
        dialTeta->setMinimum(-100);
        dialTeta->setMaximum(100);
        dialTeta->setWrapping(false);
        dialTeta->setNotchesVisible(true);

        verticalLayout_3->addWidget(dialTeta);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        label_12 = new QLabel(layoutWidget1);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        horizontalLayout_9->addWidget(label_12);

        doubleSpinBoxTeta = new QDoubleSpinBox(layoutWidget1);
        doubleSpinBoxTeta->setObjectName(QString::fromUtf8("doubleSpinBoxTeta"));
        doubleSpinBoxTeta->setDecimals(6);
        doubleSpinBoxTeta->setMinimum(-1e+09);
        doubleSpinBoxTeta->setMaximum(1e+09);
        doubleSpinBoxTeta->setValue(0);

        horizontalLayout_9->addWidget(doubleSpinBoxTeta);


        verticalLayout->addLayout(horizontalLayout_9);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_2 = new QLabel(layoutWidget1);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        sizePolicy2.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy2);

        horizontalLayout->addWidget(label_2);

        doubleSpinBoxStepTeta = new QDoubleSpinBox(layoutWidget1);
        doubleSpinBoxStepTeta->setObjectName(QString::fromUtf8("doubleSpinBoxStepTeta"));
        doubleSpinBoxStepTeta->setDecimals(8);
        doubleSpinBoxStepTeta->setMinimum(-1e+09);
        doubleSpinBoxStepTeta->setMaximum(1e+09);
        doubleSpinBoxStepTeta->setSingleStep(1e-06);
        doubleSpinBoxStepTeta->setValue(0);

        horizontalLayout->addWidget(doubleSpinBoxStepTeta);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(layoutWidget1);
        label->setObjectName(QString::fromUtf8("label"));
        sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy2);

        horizontalLayout_2->addWidget(label);

        doubleSpinBoxCenterTeta = new QDoubleSpinBox(layoutWidget1);
        doubleSpinBoxCenterTeta->setObjectName(QString::fromUtf8("doubleSpinBoxCenterTeta"));
        doubleSpinBoxCenterTeta->setDecimals(8);
        doubleSpinBoxCenterTeta->setMinimum(-180);
        doubleSpinBoxCenterTeta->setMaximum(180);

        horizontalLayout_2->addWidget(doubleSpinBoxCenterTeta);


        verticalLayout->addLayout(horizontalLayout_2);


        verticalLayout_3->addLayout(verticalLayout);

        layoutWidget2 = new QWidget(AligmentForm);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(170, 10, 157, 207));
        verticalLayout_6 = new QVBoxLayout(layoutWidget2);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        label_9 = new QLabel(layoutWidget2);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        sizePolicy.setHeightForWidth(label_9->sizePolicy().hasHeightForWidth());
        label_9->setSizePolicy(sizePolicy);
        label_9->setAlignment(Qt::AlignCenter);

        verticalLayout_6->addWidget(label_9);

        dialGamma = new QDial(layoutWidget2);
        dialGamma->setObjectName(QString::fromUtf8("dialGamma"));
        sizePolicy1.setHeightForWidth(dialGamma->sizePolicy().hasHeightForWidth());
        dialGamma->setSizePolicy(sizePolicy1);
        dialGamma->setMinimum(-100);
        dialGamma->setMaximum(100);
        dialGamma->setWrapping(false);
        dialGamma->setNotchesVisible(true);

        verticalLayout_6->addWidget(dialGamma);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_10 = new QLabel(layoutWidget2);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        horizontalLayout_8->addWidget(label_10);

        doubleSpinBoxGamma = new QDoubleSpinBox(layoutWidget2);
        doubleSpinBoxGamma->setObjectName(QString::fromUtf8("doubleSpinBoxGamma"));
        doubleSpinBoxGamma->setDecimals(6);
        doubleSpinBoxGamma->setMinimum(-1e+09);
        doubleSpinBoxGamma->setMaximum(1e+09);

        horizontalLayout_8->addWidget(doubleSpinBoxGamma);


        verticalLayout_4->addLayout(horizontalLayout_8);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(layoutWidget2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        sizePolicy2.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy2);

        horizontalLayout_3->addWidget(label_3);

        doubleSpinBoxStepGamma = new QDoubleSpinBox(layoutWidget2);
        doubleSpinBoxStepGamma->setObjectName(QString::fromUtf8("doubleSpinBoxStepGamma"));
        doubleSpinBoxStepGamma->setDecimals(8);
        doubleSpinBoxStepGamma->setMinimum(-1e+09);
        doubleSpinBoxStepGamma->setMaximum(1e+09);
        doubleSpinBoxStepGamma->setSingleStep(1e-06);
        doubleSpinBoxStepGamma->setValue(0);

        horizontalLayout_3->addWidget(doubleSpinBoxStepGamma);


        verticalLayout_4->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_4 = new QLabel(layoutWidget2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        sizePolicy2.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy2);

        horizontalLayout_4->addWidget(label_4);

        doubleSpinBoxCenterGamma = new QDoubleSpinBox(layoutWidget2);
        doubleSpinBoxCenterGamma->setObjectName(QString::fromUtf8("doubleSpinBoxCenterGamma"));
        doubleSpinBoxCenterGamma->setDecimals(8);
        doubleSpinBoxCenterGamma->setMinimum(-180);
        doubleSpinBoxCenterGamma->setMaximum(180);

        horizontalLayout_4->addWidget(doubleSpinBoxCenterGamma);


        verticalLayout_4->addLayout(horizontalLayout_4);


        verticalLayout_6->addLayout(verticalLayout_4);

        layoutWidget3 = new QWidget(AligmentForm);
        layoutWidget3->setObjectName(QString::fromUtf8("layoutWidget3"));
        layoutWidget3->setGeometry(QRect(330, 10, 157, 207));
        verticalLayout_2 = new QVBoxLayout(layoutWidget3);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_8 = new QLabel(layoutWidget3);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        sizePolicy.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy);
        label_8->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label_8);

        dialPsi = new QDial(layoutWidget3);
        dialPsi->setObjectName(QString::fromUtf8("dialPsi"));
        sizePolicy1.setHeightForWidth(dialPsi->sizePolicy().hasHeightForWidth());
        dialPsi->setSizePolicy(sizePolicy1);
        dialPsi->setMinimum(-100);
        dialPsi->setMaximum(100);
        dialPsi->setWrapping(false);
        dialPsi->setNotchesVisible(true);

        verticalLayout_2->addWidget(dialPsi);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        label_7 = new QLabel(layoutWidget3);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_7->addWidget(label_7);

        doubleSpinBoxPsi = new QDoubleSpinBox(layoutWidget3);
        doubleSpinBoxPsi->setObjectName(QString::fromUtf8("doubleSpinBoxPsi"));
        doubleSpinBoxPsi->setDecimals(6);
        doubleSpinBoxPsi->setMinimum(-1e+09);
        doubleSpinBoxPsi->setMaximum(1e+09);

        horizontalLayout_7->addWidget(doubleSpinBoxPsi);


        verticalLayout_5->addLayout(horizontalLayout_7);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_5 = new QLabel(layoutWidget3);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        sizePolicy2.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy2);

        horizontalLayout_5->addWidget(label_5);

        doubleSpinBoxStepPsi = new QDoubleSpinBox(layoutWidget3);
        doubleSpinBoxStepPsi->setObjectName(QString::fromUtf8("doubleSpinBoxStepPsi"));
        sizePolicy3.setHeightForWidth(doubleSpinBoxStepPsi->sizePolicy().hasHeightForWidth());
        doubleSpinBoxStepPsi->setSizePolicy(sizePolicy3);
        doubleSpinBoxStepPsi->setDecimals(8);
        doubleSpinBoxStepPsi->setMinimum(-1e+09);
        doubleSpinBoxStepPsi->setMaximum(1e+09);
        doubleSpinBoxStepPsi->setSingleStep(1e-06);
        doubleSpinBoxStepPsi->setValue(0);

        horizontalLayout_5->addWidget(doubleSpinBoxStepPsi);


        verticalLayout_5->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_6 = new QLabel(layoutWidget3);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        sizePolicy2.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy2);

        horizontalLayout_6->addWidget(label_6);

        doubleSpinBoxCenterPsi = new QDoubleSpinBox(layoutWidget3);
        doubleSpinBoxCenterPsi->setObjectName(QString::fromUtf8("doubleSpinBoxCenterPsi"));
        sizePolicy3.setHeightForWidth(doubleSpinBoxCenterPsi->sizePolicy().hasHeightForWidth());
        doubleSpinBoxCenterPsi->setSizePolicy(sizePolicy3);
        doubleSpinBoxCenterPsi->setDecimals(8);
        doubleSpinBoxCenterPsi->setMinimum(-180);
        doubleSpinBoxCenterPsi->setMaximum(180);

        horizontalLayout_6->addWidget(doubleSpinBoxCenterPsi);


        verticalLayout_5->addLayout(horizontalLayout_6);


        verticalLayout_2->addLayout(verticalLayout_5);

        groupBox = new QGroupBox(AligmentForm);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 250, 401, 81));
        layoutWidget4 = new QWidget(groupBox);
        layoutWidget4->setObjectName(QString::fromUtf8("layoutWidget4"));
        layoutWidget4->setGeometry(QRect(10, 20, 359, 43));
        horizontalLayout_13 = new QHBoxLayout(layoutWidget4);
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        horizontalLayout_13->setContentsMargins(0, 0, 0, 0);
        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        label_17 = new QLabel(layoutWidget4);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        verticalLayout_9->addWidget(label_17);

        doubleSpinBoxX = new QDoubleSpinBox(layoutWidget4);
        doubleSpinBoxX->setObjectName(QString::fromUtf8("doubleSpinBoxX"));
        doubleSpinBoxX->setDecimals(4);
        doubleSpinBoxX->setMinimum(-1e+09);
        doubleSpinBoxX->setMaximum(1e+09);

        verticalLayout_9->addWidget(doubleSpinBoxX);


        horizontalLayout_13->addLayout(verticalLayout_9);

        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        label_18 = new QLabel(layoutWidget4);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        verticalLayout_10->addWidget(label_18);

        doubleSpinBoxY = new QDoubleSpinBox(layoutWidget4);
        doubleSpinBoxY->setObjectName(QString::fromUtf8("doubleSpinBoxY"));
        doubleSpinBoxY->setDecimals(4);
        doubleSpinBoxY->setMinimum(-1e+09);
        doubleSpinBoxY->setMaximum(1e+09);

        verticalLayout_10->addWidget(doubleSpinBoxY);


        horizontalLayout_13->addLayout(verticalLayout_10);

        verticalLayout_11 = new QVBoxLayout();
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        label_19 = new QLabel(layoutWidget4);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        verticalLayout_11->addWidget(label_19);

        doubleSpinBoxZ = new QDoubleSpinBox(layoutWidget4);
        doubleSpinBoxZ->setObjectName(QString::fromUtf8("doubleSpinBoxZ"));
        doubleSpinBoxZ->setDecimals(4);
        doubleSpinBoxZ->setMinimum(-1e+09);
        doubleSpinBoxZ->setMaximum(1e+09);

        verticalLayout_11->addWidget(doubleSpinBoxZ);


        horizontalLayout_13->addLayout(verticalLayout_11);

        groupBox_2 = new QGroupBox(AligmentForm);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(20, 340, 220, 114));
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        label_20 = new QLabel(groupBox_2);
        label_20->setObjectName(QString::fromUtf8("label_20"));

        horizontalLayout_14->addWidget(label_20);

        spinBoxW = new QSpinBox(groupBox_2);
        spinBoxW->setObjectName(QString::fromUtf8("spinBoxW"));
        spinBoxW->setMaximum(999999999);
        spinBoxW->setValue(800);

        horizontalLayout_14->addWidget(spinBoxW);


        gridLayout->addLayout(horizontalLayout_14, 0, 0, 1, 1);

        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        label_21 = new QLabel(groupBox_2);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        horizontalLayout_15->addWidget(label_21);

        spinBoxH = new QSpinBox(groupBox_2);
        spinBoxH->setObjectName(QString::fromUtf8("spinBoxH"));
        spinBoxH->setMaximum(999999999);
        spinBoxH->setValue(600);

        horizontalLayout_15->addWidget(spinBoxH);


        gridLayout->addLayout(horizontalLayout_15, 1, 0, 1, 1);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setObjectName(QString::fromUtf8("horizontalLayout_16"));
        pushButtonSetRes = new QPushButton(groupBox_2);
        pushButtonSetRes->setObjectName(QString::fromUtf8("pushButtonSetRes"));

        horizontalLayout_16->addWidget(pushButtonSetRes);

        pushButtonCurRes = new QPushButton(groupBox_2);
        pushButtonCurRes->setObjectName(QString::fromUtf8("pushButtonCurRes"));

        horizontalLayout_16->addWidget(pushButtonCurRes);


        gridLayout->addLayout(horizontalLayout_16, 2, 0, 1, 1);

        groupBox_3 = new QGroupBox(AligmentForm);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(420, 290, 193, 154));
        gridLayout_2 = new QGridLayout(groupBox_3);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        verticalLayout_13 = new QVBoxLayout();
        verticalLayout_13->setObjectName(QString::fromUtf8("verticalLayout_13"));
        verticalLayout_12 = new QVBoxLayout();
        verticalLayout_12->setObjectName(QString::fromUtf8("verticalLayout_12"));
        horizontalLayout_20 = new QHBoxLayout();
        horizontalLayout_20->setObjectName(QString::fromUtf8("horizontalLayout_20"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_20->addItem(horizontalSpacer);

        pushButtonUP = new QPushButton(groupBox_3);
        pushButtonUP->setObjectName(QString::fromUtf8("pushButtonUP"));

        horizontalLayout_20->addWidget(pushButtonUP);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_20->addItem(horizontalSpacer_2);


        verticalLayout_12->addLayout(horizontalLayout_20);

        horizontalLayout_19 = new QHBoxLayout();
        horizontalLayout_19->setObjectName(QString::fromUtf8("horizontalLayout_19"));
        pushButtonLEFT = new QPushButton(groupBox_3);
        pushButtonLEFT->setObjectName(QString::fromUtf8("pushButtonLEFT"));

        horizontalLayout_19->addWidget(pushButtonLEFT);

        pushButtonRIGHT = new QPushButton(groupBox_3);
        pushButtonRIGHT->setObjectName(QString::fromUtf8("pushButtonRIGHT"));

        horizontalLayout_19->addWidget(pushButtonRIGHT);


        verticalLayout_12->addLayout(horizontalLayout_19);

        horizontalLayout_21 = new QHBoxLayout();
        horizontalLayout_21->setObjectName(QString::fromUtf8("horizontalLayout_21"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_21->addItem(horizontalSpacer_3);

        pushButtonDOWN = new QPushButton(groupBox_3);
        pushButtonDOWN->setObjectName(QString::fromUtf8("pushButtonDOWN"));

        horizontalLayout_21->addWidget(pushButtonDOWN);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_21->addItem(horizontalSpacer_4);


        verticalLayout_12->addLayout(horizontalLayout_21);


        verticalLayout_13->addLayout(verticalLayout_12);

        horizontalLayout_22 = new QHBoxLayout();
        horizontalLayout_22->setObjectName(QString::fromUtf8("horizontalLayout_22"));
        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setObjectName(QString::fromUtf8("horizontalLayout_17"));
        label_22 = new QLabel(groupBox_3);
        label_22->setObjectName(QString::fromUtf8("label_22"));

        horizontalLayout_17->addWidget(label_22);

        spinBoxX = new QSpinBox(groupBox_3);
        spinBoxX->setObjectName(QString::fromUtf8("spinBoxX"));
        spinBoxX->setMaximum(999999);

        horizontalLayout_17->addWidget(spinBoxX);


        horizontalLayout_22->addLayout(horizontalLayout_17);

        horizontalLayout_18 = new QHBoxLayout();
        horizontalLayout_18->setObjectName(QString::fromUtf8("horizontalLayout_18"));
        label_23 = new QLabel(groupBox_3);
        label_23->setObjectName(QString::fromUtf8("label_23"));

        horizontalLayout_18->addWidget(label_23);

        spinBoxY = new QSpinBox(groupBox_3);
        spinBoxY->setObjectName(QString::fromUtf8("spinBoxY"));
        spinBoxY->setMaximum(999999);

        horizontalLayout_18->addWidget(spinBoxY);


        horizontalLayout_22->addLayout(horizontalLayout_18);


        verticalLayout_13->addLayout(horizontalLayout_22);


        gridLayout_2->addLayout(verticalLayout_13, 0, 0, 1, 1);


        retranslateUi(AligmentForm);

        QMetaObject::connectSlotsByName(AligmentForm);
    } // setupUi

    void retranslateUi(QWidget *AligmentForm)
    {
        AligmentForm->setWindowTitle(QApplication::translate("AligmentForm", "\320\256\321\201\321\202\320\270\321\200\320\276\320\262\320\272\320\260", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("AligmentForm", "\320\243\320\263\320\276\320\273 \320\276\320\261\320\267\320\276\321\200\320\260", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("AligmentForm", "\320\243\320\263\320\276\320\273", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("AligmentForm", "\320\250\320\260\320\263", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("AligmentForm", "\320\246\320\265\320\275\321\202\321\200", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("AligmentForm", "\320\243\320\263\320\276\320\273 \321\202\320\260\320\275\320\263\320\260\320\266\320\260 \320\272\320\260\320\274\320\265\321\200\321\213", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("AligmentForm", "\320\243\320\263\320\276\320\273", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("AligmentForm", "\320\250\320\260\320\263", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("AligmentForm", "\320\246\320\265\320\275\321\202\321\200", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("AligmentForm", "\320\243\320\263\320\276\320\273 \320\272\321\200\320\265\320\275\320\260 \320\272\320\260\320\274\320\265\321\200\321\213", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("AligmentForm", "\320\243\320\263\320\276\320\273", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("AligmentForm", "\320\250\320\260\320\263", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("AligmentForm", "\320\246\320\265\320\275\321\202\321\200", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("AligmentForm", "\320\243\320\263\320\276\320\273 \320\272\321\203\321\200\321\201\320\260 \320\272\320\260\320\274\320\265\321\200\321\213", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("AligmentForm", "\320\243\320\263\320\276\320\273", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("AligmentForm", "\320\250\320\260\320\263", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("AligmentForm", "\320\246\320\265\320\275\321\202\321\200", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("AligmentForm", "\320\246\320\265\320\273\321\214", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("AligmentForm", "X", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("AligmentForm", "Y", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("AligmentForm", "Z", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("AligmentForm", "\320\240\320\260\320\267\321\200\320\265\321\210\320\265\320\275\320\270\320\265 \320\276\320\272\320\275\320\260", 0, QApplication::UnicodeUTF8));
        label_20->setText(QApplication::translate("AligmentForm", "\320\250\320\270\321\200\320\270\320\275\320\260", 0, QApplication::UnicodeUTF8));
        label_21->setText(QApplication::translate("AligmentForm", "\320\222\321\213\321\201\320\276\321\202\320\260", 0, QApplication::UnicodeUTF8));
        pushButtonSetRes->setText(QApplication::translate("AligmentForm", "\320\227\320\260\320\264\320\260\321\202\321\214", 0, QApplication::UnicodeUTF8));
        pushButtonCurRes->setText(QApplication::translate("AligmentForm", "\320\242\320\265\320\272\321\203\321\211\320\265\320\265 \321\200\320\260\320\267\321\200\320\265\321\210\320\265\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("AligmentForm", "\320\240\320\260\321\201\320\277\320\276\320\273\320\276\320\266\320\265\320\275\320\270\320\265 \320\276\320\272\320\275\320\260", 0, QApplication::UnicodeUTF8));
        pushButtonUP->setText(QApplication::translate("AligmentForm", "UP", 0, QApplication::UnicodeUTF8));
        pushButtonLEFT->setText(QApplication::translate("AligmentForm", "LEFT", 0, QApplication::UnicodeUTF8));
        pushButtonRIGHT->setText(QApplication::translate("AligmentForm", "RIGHT", 0, QApplication::UnicodeUTF8));
        pushButtonDOWN->setText(QApplication::translate("AligmentForm", "DOWN", 0, QApplication::UnicodeUTF8));
        label_22->setText(QApplication::translate("AligmentForm", "x=", 0, QApplication::UnicodeUTF8));
        label_23->setText(QApplication::translate("AligmentForm", "y=", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class AligmentForm: public Ui_AligmentForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALIGMENTFORM_H
