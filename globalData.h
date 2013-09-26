#ifndef GLOBALDATA_H
#define GLOBALDATA_H
typedef struct
{
    int id;
    int code;
    double x_g;
    double y_g;
    double z_g;

    double psi;
    double gamma;
    double tan;
}TSolidObj;
typedef struct
{
    int sizeObj;
    TSolidObj obj[17];
}TTrans3DWorld;
#endif // GLOBALDATA_H
