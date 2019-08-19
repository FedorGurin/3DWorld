#ifndef FORMSETTINGS_H
#define FORMSETTINGS_H

#include <QWidget>

namespace Ui {
class FormSettings;
}
//форма с настройками отображения
class FormSettings : public QWidget
{
    Q_OBJECT

public:
    explicit FormSettings(QWidget *parent = 0);
    ~FormSettings();
signals:
    void fog(bool);

    void grid(bool);
    void vertical(bool);
    void ils(bool);
    void info(bool);
    void loadDataFlight();
    void loadLand();
    void terra(bool);
    void freq(int);
    void multTime(double);
    void densityFog(double);
    void sizeScene(double);
    void offsetMapX(double);
    void offsetMapZ(double);
    void rotateMap(double);
private:
    Ui::FormSettings *ui;
};

#endif // FORMSETTINGS_H
