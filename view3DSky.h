#ifndef VIEW3DSKY_H
#define VIEW3DSKY_H

#include <QtGui/qopengl.h>
#include <glm/glm.hpp>

//! отрисовка неба: градиентный купол, солнце и слой облаков
class View3DSky
{
public:
    View3DSky();
    //! создание текстуры облаков (вызывается при наличии контекста OpenGL)
    void init();
    //! отрисовка купола неба и солнца вокруг камеры (до остальной сцены)
    void drawDome(const glm::vec3 &cameraPos, float radius);
    //! отрисовка слоя облаков (после непрозрачных объектов сцены)
    void drawClouds(const glm::vec3 &cameraPos, float radius, double time);
    //! цвет неба в направлении с заданным синусом угла места
    glm::vec3 skyColor(float sinElevation, float altitude) const;
    //! направление на солнце в земной СК (единичный вектор)
    glm::vec3 sunDirection() const {return sunDir;}

    //! высота нижней границы облаков, м
    float cloudHeight;
    //! признак отрисовки облаков
    bool clouds;
private:
    glm::vec3 sunDir;
    //! текстура облаков
    GLuint cloudTexture;
};

#endif // VIEW3DSKY_H
