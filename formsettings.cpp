#include "formsettings.h"
#include "ui_formsettings.h"

FormSettings::FormSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSettings)
{
    ui->setupUi(this);
    ///
    connect(ui->checkBoxFog,SIGNAL(clicked(bool)),this,SIGNAL(fog(bool)));
    connect(ui->checkBoxGrid,SIGNAL(clicked(bool)),this,SIGNAL(grid(bool)));
    connect(ui->checkBoxVertLine,SIGNAL(clicked(bool)),this,SIGNAL(vertical(bool)));
    ///
    connect(ui->pushButtonLoadData,SIGNAL(clicked(bool)),this,SIGNAL(loadDataFlight()));
    connect(ui->pushButtonLoadLand,SIGNAL(clicked(bool)),this,SIGNAL(loadLand()));
    ///
    connect(ui->checkBoxILS,SIGNAL(clicked(bool)),this,SIGNAL(ils(bool)));
    connect(ui->checkBoxInfo,SIGNAL(clicked(bool)),this,SIGNAL(info(bool)));
    ///
    connect(ui->checkBoxLand,SIGNAL(clicked(bool)),this,SIGNAL(terra(bool)));

    connect(ui->spinBoxFreq,SIGNAL(valueChanged(int)),this,SIGNAL(freq(int)));
    connect(ui->doubleSpinBoxMulTime,SIGNAL(valueChanged(double)),this,SIGNAL(multTime(double)));
    connect(ui->doubleSpinBoxFog,SIGNAL(valueChanged(double)),this,SIGNAL(densityFog(double)));
    connect(ui->doubleSpinBoxSizeScene,SIGNAL(valueChanged(double)),this,SIGNAL(sizeScene(double)));
    connect(ui->doubleSpinBoxOffsetX,SIGNAL(valueChanged(double)),this,SIGNAL(offsetMapX(double)));
    connect(ui->doubleSpinBoxOffsetZ,SIGNAL(valueChanged(double)),this,SIGNAL(offsetMapZ(double)));
    connect(ui->doubleSpinBoxRot,SIGNAL(valueChanged(double)),this,SIGNAL(rotateMap(double)));
}

FormSettings::~FormSettings()
{
    delete ui;
}

