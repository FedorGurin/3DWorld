#ifndef GLOBALDATA_H
#define GLOBALDATA_H
#include <glm/glm.hpp>
typedef struct
{
    double time;
    //! уникальный идентификатор объекта
    int id;

    //! код объекта
    int code;
    //! угол атаки
    float alfa;
    //! воздушная скорости
    float vc;

    //! углы ориентации вектора скорости в траекторной СК
    float fi;
    float unt;

    //! координаты носителя в географической СК
    float fi_geo;
    float lam_geo;

    //! координаты старта носителя в географической СК
    float fi0_geo;
    float lam0_geo;

    //! координаты в земной СК
    float xg;
    float yg;
    float zg;

    //! компоненты вектора скорости относительной земной СК
    float vxg;
    float vyg;
    float vzg;

    //! компоненты ускорений
    float axg;
    float ayg;
    float azg;

    //! угловые скорости вращения носителя
    float omega_x;
    float omega_y;
    float omega_z;

    //! ориентация носителя в связанной СК
    float psi;
    float gamma;
    float tan;
    //! число Маха
    float m;
    //!
    float nya;
    //!
    float ny;
    //!
    float nx;

    int idManevr;
}TVis;
typedef struct TVisSimple
{
    int id;
    int code;
    double time;
    glm::vec3 c_g;

    double psi;
    double gamma;
    double tan;

    double alfa;
    double beta;

    glm::vec3 n_c;
    double vc;

    double fi;
    double unt;

    glm::vec3 v_g;
    glm::vec3 v_c;
    glm::vec3 omega;
}TVisSimple;
typedef struct
{
    //! идентификатор объекта
    int idObj;
    //! положение органов управления
    float tan_rus;  // [-1.0;1.0]
    float gamma_rus;// [-1.0;1.0]
    float rud;      // [ 0.0;1.0]
}TControlStick;

#endif // GLOBALDATA_H
