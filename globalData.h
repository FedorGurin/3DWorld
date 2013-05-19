#ifndef GLOBALDATA_H
#define GLOBALDATA_H

typedef struct
{
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
//! данные по работе с высотой.
typedef struct _THeight_data
{
    double x;   // Координата точки в которой запрашивается высота рельефа.
    double z;   // Координата точки в которой запрашивается высота рельефа.
    double h;   // Высота точки.
}THeight_data;


#endif // GLOBALDATA_H
