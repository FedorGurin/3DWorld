#ifndef GLOBALDATA_H
#define GLOBALDATA_H

typedef struct
{
    double time;
    //! ���������� ������������� �������
    int id;

    //! ��� �������
    int code;
    //! ���� �����
    float alfa;
    //! ��������� ��������
    float vc;

    //! ���� ���������� ������� �������� � ����������� ��
    float fi;
    float unt;

    //! ���������� �������� � �������������� ��
    float fi_geo;
    float lam_geo;

    //! ���������� ������ �������� � �������������� ��
    float fi0_geo;
    float lam0_geo;

    //! ���������� � ������ ��
    float xg;
    float yg;
    float zg;

    //! ���������� ������� �������� ������������� ������ ��
    float vxg;
    float vyg;
    float vzg;

    //! ���������� ���������
    float axg;
    float ayg;
    float azg;

    //! ������� �������� �������� ��������
    float omega_x;
    float omega_y;
    float omega_z;

    //! ���������� �������� � ��������� ��
    float psi;
    float gamma;
    float tan;
    //! ����� ����
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
    //! ������������� �������
    int idObj;
    //! ��������� ������� ����������
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
