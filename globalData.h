#ifndef GLOBALDATA_H
#define GLOBALDATA_H

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
typedef struct
{
    int id;
    int code;

    double time;
   // double time_prev;
    double x_g;
    double y_g;
    double z_g;

//    double x_g_prev;
//    double y_g_prev;
//    double z_g_prev;

    double psi;
    double gamma;
    double tan;

    double alfa;
    double beta;

    double nx;
    double ny;
    double nz;

    double vc;

    double fi;
    double unt;

    double vx_g;
    double vy_g;
    double vz_g;

    double vx_c;
    double vy_c;
    double vz_c;

    double wx;
    double wy;
    double wz;


//    double psi_prev;
//    double gamma_prev;
//    double tan_prev;
}TSolidObj;
typedef struct
{
    //! идентификатор объекта
    int idObj;
    //! положение органов управления
    float tan_rus;  // [-1.0;1.0]
    float gamma_rus;// [-1.0;1.0]
    float rud;      // [ 0.0;1.0]
}TControlStick;
typedef struct
{
    int sizeObj;
    TSolidObj obj[17];
}TTrans3DWorld;
#endif // GLOBALDATA_H
