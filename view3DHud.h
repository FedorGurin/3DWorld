#ifndef VIEW3DHUD_H
#define VIEW3DHUD_H

#include <QFont>
#include <QString>
#include <QVector>
#include <glm/glm.hpp>

namespace qglviewer {
class Camera;
}
class QGLViewer;

//! параметры объекта, необходимые для отрисовки HUD
typedef struct
{
    //! углы ориентации: курс, тангаж, крен, рад
    double psi;
    double tan;
    double gamma;
    //! углы атаки и скольжения, рад
    double alfa;
    double beta;
    //! скорость, м/с
    double v;
    //! компоненты скорости в связанной СК, м/с
    double vx;
    double vy;
    double vz;
    //! высота, м
    double altitude;
    //! нормальная перегрузка, ед
    double ny;
    //! положение объекта в земной СК, м
    glm::vec3 position;
}THudData;

//! цель для отображения на HUD
typedef struct
{
    int id;
    glm::vec3 position;
}THudTarget;

//! индикатор на лобовом стекле (HUD)
class View3DHud
{
public:
    View3DHud();
    //! отрисовка HUD поверх сцены
    void draw(QGLViewer *viewer,
              const THudData &data,
              const QVector<THudTarget> &targets);

    //! цвет символики
    float color[3];
private:
    //! шкала тангажа и линия горизонта
    void drawPitchLadder(QGLViewer *viewer, const THudData &data);
    //! символ продольной оси самолета ("W") и маркер вектора скорости
    void drawBoresight(const THudData &data);
    //! шкала курса сверху
    void drawHeadingTape(QGLViewer *viewer, const THudData &data);
    //! вертикальная шкала (скорость слева, высота справа)
    void drawVerticalTape(QGLViewer *viewer, double value, int x, bool left,
                          double step, int labelEvery, double pixelsPerUnit);
    //! шкала крена
    void drawBankScale(const THudData &data);
    //! цели
    void drawTargets(QGLViewer *viewer, const THudData &data,
                     const QVector<THudTarget> &targets);
    //! текст с выравниванием: align = -1 влево, 0 по центру, 1 вправо
    void text(QGLViewer *viewer, int x, int y, const QString &str, int align = -1);

    //! фокусное расстояние камеры в пикселях
    double focal;
    //! центр экрана
    double cx, cy;
    int width, height;
    QFont font;
    QFont fontSmall;
};

#endif // VIEW3DHUD_H
