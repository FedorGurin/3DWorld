//
// Basic class for 3d matrices
// Supports all basic matrix operations
//
// Author: Alex V. Boreskoff
//

#ifndef __Matrix3D_D__
#define __Matrix3D_D__

#include	"Vector3D_D.h"

class Matrix3D_D
{
public:
    double x [3][3];

	Matrix3D_D () {}
    Matrix3D_D ( double );
	Matrix3D_D ( const Matrix3D_D& );
    Matrix3D_D ( double d1, double d2, double d3 );
	Matrix3D_D ( const Vector3D_D& c1, const Vector3D_D& c2, const Vector3D_D& c3 );

	Matrix3D_D& operator =  ( const Matrix3D_D& );
    Matrix3D_D& operator =  ( double );
	Matrix3D_D& operator += ( const Matrix3D_D& );
	Matrix3D_D& operator -= ( const Matrix3D_D& );
	Matrix3D_D& operator *= ( const Matrix3D_D& );
    Matrix3D_D& operator *= ( double );
    Matrix3D_D& operator /= ( double );

    const double * operator [] ( int i ) const
	{
		return & x[i][0];
	}

    double * operator [] ( int i )
	{
		return & x[i][0];
	}

	Matrix3D_D&	invert    ();
	Matrix3D_D& 	transpose ();
    double		det       () const;

	Matrix3D_D	inverse () const
	{
		return Matrix3D_D ( *this ).invert ();
	}

    void getHomMatrix ( double * matrix ) const;
    void getHomMatrix ( double * matrix, const Vector3D_D& offs ) const;

	static	Matrix3D_D	identity ();
	static	Matrix3D_D	scale    ( const Vector3D_D& );
    static	Matrix3D_D	rotateX  ( double );
    static	Matrix3D_D	rotateY  ( double );
    static	Matrix3D_D	rotateZ  ( double );
    static	Matrix3D_D	rotate   ( const Vector3D_D&, double );
    static	Matrix3D_D	rotate   ( double yaw, double pitch, double roll );
	static	Matrix3D_D	mirrorX  ();
	static	Matrix3D_D	mirrorY  ();
	static	Matrix3D_D	mirrorZ  ();

	friend Matrix3D_D operator + ( const Matrix3D_D&, const Matrix3D_D& );
	friend Matrix3D_D operator - ( const Matrix3D_D&, const Matrix3D_D& );
	friend Matrix3D_D operator * ( const Matrix3D_D&, const Matrix3D_D& );
    friend Matrix3D_D operator * ( const Matrix3D_D&, double );
    friend Matrix3D_D operator * ( double, const Matrix3D_D& );
	friend Vector3D_D operator * ( const Matrix3D_D&, const Vector3D_D& );
};

inline Vector3D_D operator * ( const Matrix3D_D& a, const Vector3D_D& b )
{
	return Vector3D_D ( a.x [0][0]*b.x + a.x [0][1]*b.y + a.x [0][2]*b.z,
	                  a.x [1][0]*b.x + a.x [1][1]*b.y + a.x [1][2]*b.z,
	                  a.x [2][0]*b.x + a.x [2][1]*b.y + a.x [2][2]*b.z );
}

#endif
