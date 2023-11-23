#ifndef GLOBALDATA_H
#define GLOBALDATA_H
#include <glm/glm.hpp>
#include <stdint.h>

//typedef struct
//{
//    double time;
//    //! уникальный идентификатор объекта
//    int id;

//    //! код объекта
//    int code;
//    //! угол атаки
//    float alfa;
//    //! воздушная скорости
//    float vc;

//    //! углы ориентации вектора скорости в траекторной СК
//    float fi;
//    float unt;

//    //! координаты носителя в географической СК
//    float fi_geo;
//    float lam_geo;

//    //! координаты старта носителя в географической СК
//    float fi0_geo;
//    float lam0_geo;

//    //! координаты в земной СК
//    float xg;
//    float yg;
//    float zg;

//    //! компоненты вектора скорости относительной земной СК
//    float vxg;
//    float vyg;
//    float vzg;

//    //! компоненты ускорений
//    float axg;
//    float ayg;
//    float azg;

//    //! угловые скорости вращения носителя
//    float omega_x;
//    float omega_y;
//    float omega_z;

//    //! ориентация носителя в связанной СК
//    float psi;
//    float gamma;
//    float tan;
//    //! число Маха
//    float m;
//    //!
//    float nya;
//    //!
//    float ny;
//    //!
//    float nx;

//    int idManevr;
//}TVis;

typedef struct TSendVehicleVisSimple_
{
    int id;
    int code;
    double time;
    double cg_x;
    double cg_y;
    double cg_z;


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

    double fi0_geo;
    double lam0_geo;

    double fi_geo;
    double lam_geo;

    double vg_x;
    double vg_y;
    double vg_z;

    double vc_x;
    double vc_y;
    double vc_z;

    double omega_x;
    double omega_y;
    double omega_z;
}TSendVehicleVisSimple;
typedef struct TSendArrayVisSimple_
{
    int id;
    int code;
    double time;

    //! координаты опорной точки
    double fi0_geo;
    double lam0_geo;

    //! кол-во элементов
    uint8_t num;

    typedef struct TGeoCoord_
    {
        double fi_geo;
        double lam_geo;
    }TGeoCoord;
    //! список объектов элемента
    TGeoCoord coord[256];
}TSendArrayVisSimple;


typedef struct TSendListVectorsVisSimple_
{
    int id;
    int code;
    double time;

    //! кол-во элементов
    uint8_t num;

    glm::vec3 points[256];
    //! список объектов элемента

}TSendListVectorsVisSimple;
//typedef struct TSendVehicleVisSimple
//{
//    int id;
//    int code;
//    double time;
//    double cg_x;
//    double cg_y;
//    double cg_z;

//    double psi;
//    double gamma;
//    double tan;

//    double alfa;
//    double beta;

//    double nx;
//    double ny;
//    double nz;

//    double vc;

//    double fi;
//    double unt;

//    double fi0_geo;
//    double lam0_geo;

//    double fi_geo;
//    double lam_geo;

//    double vg_x;
//    double vg_y;
//    double vg_z;

//    double vc_x;
//    double vc_y;
//    double vc_z;

//    double omega_x;
//    double omega_y;
//    double omega_z;
//}TSendVehicleVisSimple;
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
