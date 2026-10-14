#include "view3DHud.h"

#include <QGLViewer/qglviewer.h>
#include <QFontMetrics>
#include <math.h>

using namespace qglviewer;

static const double RAD2DEG = 180.0 / M_PI;
static const double DEG2RAD = M_PI / 180.0;

View3DHud::View3DHud()
{
    color[0] = 0.25f;
    color[1] = 1.0f;
    color[2] = 0.35f;
    focal  = 1.0;
    cx = cy = 0.0;
    width = height = 0;
    font = QFont("Monospace", 10, QFont::Bold);
    font.setStyleHint(QFont::TypeWriter);
    fontSmall = QFont("Monospace", 8, QFont::Bold);
    fontSmall.setStyleHint(QFont::TypeWriter);
}

void View3DHud::text(QGLViewer *viewer, int x, int y, const QString &str, int align)
{
    QFontMetrics fm(font);
    int w = fm.horizontalAdvance(str);
    if(align == 0)
        x -= w / 2;
    else if(align > 0)
        x -= w;
    // y - центр строки
    viewer->drawText(x, y + fm.ascent() / 2 - 1, str, font);
}

void View3DHud::draw(QGLViewer *viewer,
                     const THudData &data,
                     const QVector<THudTarget> &targets)
{
    width  = viewer->width();
    height = viewer->height();
    cx = width  / 2.0;
    cy = height / 2.0;
    // вертикальный угол поля зрения камеры
    focal = (height / 2.0) / tan(viewer->camera()->fieldOfView() / 2.0);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    // экранные координаты в пикселях, начало - левый верхний угол
    glOrtho(0, width, height, 0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glDisable(GL_FOG);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glColor3fv(color);

    drawPitchLadder(viewer, data);
    drawBoresight(data);
    drawHeadingTape(viewer, data);
    // скорость, км/ч: риска через 10, подпись через 50
    drawVerticalTape(viewer, data.v * 3.6, int(cx - width * 0.30), true, 10.0, 5, 3.0);
    // высота, м: риска через 50, подпись через 250
    drawVerticalTape(viewer, data.altitude, int(cx + width * 0.30), false, 50.0, 5, 0.6);
    drawBankScale(data);
    drawTargets(viewer, data, targets);

    // параметры полета в левом нижнем углу HUD
    glColor3fv(color);
    const int xInfo = int(cx - width * 0.30) - 40;
    const int yInfo = int(cy + height * 0.25) + 24;
    text(viewer, xInfo, yInfo,      QString("M %1").arg(data.v / 340.0, 0, 'f', 2));
    text(viewer, xInfo, yInfo + 18, QString("G %1").arg(data.ny, 0, 'f', 1));
    text(viewer, xInfo, yInfo + 36, QString::fromUtf8("α %1").arg(data.alfa * RAD2DEG, 0, 'f', 1));

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
}

void View3DHud::drawPitchLadder(QGLViewer *viewer, const THudData &data)
{
    const double pitchDeg = data.tan * RAD2DEG;
    const double roll = data.gamma;
    const double cr = cos(roll), sr = sin(roll);
    // область отображения шкалы (не заходит на ленты скорости и высоты)
    const double halfWidth = width * 0.12;
    const double gap       = width * 0.035;
    const double maxR      = height * 0.38;

    // поворот точки шкалы (u - вдоль линии, v - вниз) на угол крена вокруг центра
    auto rot = [&](double u, double v, double &x, double &y)
    {
        x = cx + u * cr + v * sr;
        y = cy - u * sr + v * cr;
    };

    // ближайшие к текущему тангажу линии через 5 град
    int first = int(floor((pitchDeg - 40.0) / 5.0)) * 5;
    for(int a = first; a <= pitchDeg + 40.0; a += 5)
    {
        if(a < -90 || a > 90)
            continue;
        double dv = tan((pitchDeg - a) * DEG2RAD) * focal;
        if(fabs(dv) > maxR)
            continue;

        double x0, y0, x1, y1;
        if(a == 0)
        {
            // линия горизонта - длинная, с разрывом в центре
            const double hw = width * 0.30;
            glLineWidth(2.0f);
            glBegin(GL_LINES);
            rot(-hw, dv, x0, y0); rot(-gap, dv, x1, y1);
            glVertex2d(x0, y0); glVertex2d(x1, y1);
            rot(gap, dv, x0, y0); rot(hw, dv, x1, y1);
            glVertex2d(x0, y0); glVertex2d(x1, y1);
            glEnd();
            continue;
        }

        const bool major = (a % 10) == 0;
        const double hw = major ? halfWidth : halfWidth * 0.6;
        // риски на концах направлены к горизонту
        const double tick = (a > 0 ? 8.0 : -8.0);
        glLineWidth(1.5f);
        if(a < 0)
        {
            // ниже горизонта - пунктир
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(1, 0x0F0F);
        }
        glBegin(GL_LINES);
        rot(-hw, dv, x0, y0); rot(-gap, dv, x1, y1);
        glVertex2d(x0, y0); glVertex2d(x1, y1);
        rot(gap, dv, x0, y0); rot(hw, dv, x1, y1);
        glVertex2d(x0, y0); glVertex2d(x1, y1);
        glEnd();
        glDisable(GL_LINE_STIPPLE);
        glBegin(GL_LINES);
        rot(-hw, dv, x0, y0); rot(-hw, dv + tick, x1, y1);
        glVertex2d(x0, y0); glVertex2d(x1, y1);
        rot(hw, dv, x0, y0); rot(hw, dv + tick, x1, y1);
        glVertex2d(x0, y0); glVertex2d(x1, y1);
        glEnd();

        if(major)
        {
            QString label = QString::number(abs(a));
            double xl, yl;
            rot(-hw - 22, dv, xl, yl);
            text(viewer, int(xl), int(yl), label, 0);
            rot(hw + 22, dv, xl, yl);
            text(viewer, int(xl), int(yl), label, 0);
        }
    }
}

void View3DHud::drawBoresight(const THudData &data)
{
    // символ продольной оси ("W") - неподвижен относительно самолета
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2d(cx - 24, cy);
    glVertex2d(cx - 12, cy);
    glVertex2d(cx - 6,  cy + 8);
    glVertex2d(cx,      cy);
    glVertex2d(cx + 6,  cy + 8);
    glVertex2d(cx + 12, cy);
    glVertex2d(cx + 24, cy);
    glEnd();

    // маркер вектора скорости: направление скорости в связанной СК
    double up = 0.0, right = 0.0;
    if(data.vx > 1.0)
    {
        up    = atan2(data.vy, data.vx);
        right = atan2(data.vz, data.vx);
    }else
    {
        up    = -data.alfa;
        right = data.beta;
    }
    double x = cx + tan(right) * focal;
    double y = cy - tan(up) * focal;
    // маркер не выходит за пределы центральной области
    const double lim = height * 0.35;
    x = qBound(cx - lim, x, cx + lim);
    y = qBound(cy - lim, y, cy + lim);

    const double r = 7.0;
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < 20; i++)
    {
        double a = 2.0 * M_PI * i / 20;
        glVertex2d(x + r * cos(a), y + r * sin(a));
    }
    glEnd();
    glBegin(GL_LINES);
    glVertex2d(x - r, y);      glVertex2d(x - r - 12, y);
    glVertex2d(x + r, y);      glVertex2d(x + r + 12, y);
    glVertex2d(x, y - r);      glVertex2d(x, y - r - 8);
    glEnd();
}

void View3DHud::drawHeadingTape(QGLViewer *viewer, const THudData &data)
{
    // курс: поворот psi против часовой стрелки вокруг оси Y,
    // поэтому курс (по часовой стрелке) равен -psi
    double heading = fmod(-data.psi * RAD2DEG, 360.0);
    if(heading < 0.0)
        heading += 360.0;

    const double y = height * 0.08;
    const double halfW = width * 0.22;
    const double ppd = 6.0; // пикселей на градус

    glLineWidth(1.5f);
    glBegin(GL_LINES);
    int first = int(floor((heading - halfW / ppd) / 5.0)) * 5;
    for(int h = first; h <= heading + halfW / ppd; h += 5)
    {
        double x = cx + (h - heading) * ppd;
        if(fabs(x - cx) > halfW)
            continue;
        double len = (h % 10 == 0) ? 10.0 : 5.0;
        glVertex2d(x, y);
        glVertex2d(x, y + len);
    }
    // указатель текущего курса
    glVertex2d(cx, y + 14); glVertex2d(cx - 6, y + 24);
    glVertex2d(cx, y + 14); glVertex2d(cx + 6, y + 24);
    glEnd();

    for(int h = first; h <= heading + halfW / ppd; h += 5)
    {
        double x = cx + (h - heading) * ppd;
        if(fabs(x - cx) > halfW - 10 || h % 10 != 0)
            continue;
        int hn = ((h % 360) + 360) % 360;
        // подписи в десятках градусов: 09 = 090
        if(fabs(x - cx) > 36)
            text(viewer, int(x), int(y - 12), QString("%1").arg(hn / 10, 2, 10, QChar('0')), 0);
    }
    // текущее значение курса в рамке
    glBegin(GL_LINE_LOOP);
    glVertex2d(cx - 22, y - 22); glVertex2d(cx + 22, y - 22);
    glVertex2d(cx + 22, y - 3);  glVertex2d(cx - 22, y - 3);
    glEnd();
    text(viewer, int(cx), int(y - 12), QString("%1").arg(int(heading + 0.5) % 360, 3, 10, QChar('0')), 0);
}

void View3DHud::drawVerticalTape(QGLViewer *viewer, double value, int x, bool left,
                                 double step, int labelEvery, double pixelsPerUnit)
{
    const double halfH = height * 0.25;
    const double dir = left ? -1.0 : 1.0;   // риски наружу от центра
    const double range = halfH / pixelsPerUnit;

    glLineWidth(1.5f);
    glBegin(GL_LINES);
    // основная линия шкалы
    glVertex2d(x, cy - halfH);
    glVertex2d(x, cy + halfH);
    long first = long(floor((value - range) / step));
    long last  = long(ceil((value + range) / step));
    for(long i = first; i <= last; i++)
    {
        double v = i * step;
        double y = cy - (v - value) * pixelsPerUnit;
        if(fabs(y - cy) > halfH)
            continue;
        double len = (i % labelEvery == 0) ? 12.0 : 6.0;
        glVertex2d(x, y);
        glVertex2d(x + dir * len, y);
    }
    glEnd();

    for(long i = first; i <= last; i++)
    {
        if(i % labelEvery != 0)
            continue;
        double v = i * step;
        double y = cy - (v - value) * pixelsPerUnit;
        // не подписываем риски рядом с рамкой текущего значения
        if(fabs(y - cy) > halfH - 6 || fabs(y - cy) < 16)
            continue;
        text(viewer, int(x + dir * 16), int(y), QString::number(long(v)), left ? 1 : -1);
    }

    // рамка с текущим значением
    const double bw = 62.0, bh = 11.0;
    const double bx = left ? x - 4 - bw : x + 4;
    glBegin(GL_LINE_LOOP);
    glVertex2d(bx,      cy - bh);
    glVertex2d(bx + bw, cy - bh);
    glVertex2d(bx + bw, cy + bh);
    glVertex2d(bx,      cy + bh);
    glEnd();
    // стрелка к шкале
    glBegin(GL_LINES);
    glVertex2d(left ? bx + bw : bx, cy);
    glVertex2d(x, cy);
    glEnd();
    text(viewer, int(bx + bw / 2), int(cy), QString::number(long(floor(value + 0.5))), 0);
}

void View3DHud::drawBankScale(const THudData &data)
{
    // дуга снизу: риски 0, 10, 20, 30, 45, 60 град
    const double r = height * 0.30;
    const double ccy = cy;
    const int ticks[] = {-60, -45, -30, -20, -10, 0, 10, 20, 30, 45, 60};
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    for(int t : ticks)
    {
        double a = t * DEG2RAD;
        double len = (t % 30 == 0) ? 12.0 : 7.0;
        double sx = sin(a), cyy = cos(a);
        glVertex2d(cx + r * sx,         ccy + r * cyy);
        glVertex2d(cx + (r + len) * sx, ccy + (r + len) * cyy);
    }
    glEnd();
    // указатель крена (треугольник) направлен "на землю" и поворачивается
    // вместе со шкалой тангажа
    double a = data.gamma;
    a = qBound(-70.0 * DEG2RAD, a, 70.0 * DEG2RAD);
    double sx = sin(a), cyy = cos(a);
    double px = cx + (r - 2) * sx, py = ccy + (r - 2) * cyy;
    // направление по касательной
    double tx = cyy, ty = -sx;
    glBegin(GL_LINE_LOOP);
    glVertex2d(px, py);
    glVertex2d(px - 10 * sx + 6 * tx, py - 10 * cyy + 6 * ty);
    glVertex2d(px - 10 * sx - 6 * tx, py - 10 * cyy - 6 * ty);
    glEnd();
}

void View3DHud::drawTargets(QGLViewer *viewer, const THudData &data,
                            const QVector<THudTarget> &targets)
{
    Camera *cam = viewer->camera();
    const double margin = 30.0;
    glLineWidth(1.5f);
    for(const THudTarget &t : targets)
    {
        Vec p(t.position.x, t.position.y, t.position.z);
        Vec inCam = cam->cameraCoordinatesOf(p);
        Vec scr   = cam->projectedCoordinatesOf(p);
        double x = scr.x, y = scr.y;
        bool behind = inCam.z > 0.0;
        if(behind)
        {
            // цель сзади - показываем у края экрана в ее стороне
            x = cx - (x - cx) * 1000.0;
            y = cy - (y - cy) * 1000.0;
        }
        bool clamped = false;
        if(behind || x < margin || x > width - margin || y < margin || y > height - margin)
        {
            // приводим к краю экрана по направлению на цель
            double dx = x - cx, dy = y - cy;
            double k = 1.0;
            if(fabs(dx) > 1e-6) k = qMin(k, (cx - margin) / fabs(dx));
            if(fabs(dy) > 1e-6) k = qMin(k, (cy - margin) / fabs(dy));
            x = cx + dx * k;
            y = cy + dy * k;
            clamped = true;
        }
        // ромб вокруг цели
        const double s = clamped ? 6.0 : 10.0;
        glBegin(GL_LINE_LOOP);
        glVertex2d(x,     y - s);
        glVertex2d(x + s, y);
        glVertex2d(x,     y + s);
        glVertex2d(x - s, y);
        glEnd();
        if(clamped == false)
        {
            double dist = glm::length(t.position - data.position);
            text(viewer, int(x + s + 4), int(y), QString::number(dist / 1000.0, 'f', 1));
        }
    }
}
