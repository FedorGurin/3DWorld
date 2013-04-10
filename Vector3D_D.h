//
// Basic class for 3d vectors.
// Supports all basic vector operations
//
// Author: Alex V. Boreskoff
//

#ifndef	__Vector3D_D_D__
#define	__Vector3D_D_D__

#include	<math.h>

#ifndef	EPS
    #define	EPS	0.000000000000001f
#endif

#ifndef	M_PI								// since not all compilers define it
	#define M_PI        3.14159265358979323846f
#endif

class	Vector3D_D
{
public:
    double	x, y, z;

	Vector3D_D () {}
    Vector3D_D ( double v ) : x ( v ), y ( v ), z ( v ) {}
    Vector3D_D ( double px, double py, double pz ) : x ( px ), y ( py ), z ( pz ) {}
	Vector3D_D ( const Vector3D_D& v ) : x ( v.x ), y ( v.y ), z ( v.z ) {}

	Vector3D_D& operator = ( const Vector3D_D& v )
	{
		x = v.x;
		y = v.y;
		z = v.z;

		return *this;
	}

	Vector3D_D operator + () const
	{
		return *this;
	}

	Vector3D_D operator - () const
	{
		return Vector3D_D ( -x, -y, -z );
	}

	Vector3D_D& operator += ( const Vector3D_D& v )
	{
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}

	Vector3D_D& operator -= ( const Vector3D_D& v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

	Vector3D_D& operator *= ( const Vector3D_D& v )
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;

		return *this;
	}

    Vector3D_D& operator *= ( double f )
	{
		x *= f;
		y *= f;
		z *= f;

		return *this;
	}

	Vector3D_D& operator /= ( const Vector3D_D& v )
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;

		return *this;
	}

    Vector3D_D& operator /= ( double f )
	{
		x /= f;
		y /= f;
		z /= f;

		return *this;
	}

    double& operator [] ( int index )
	{
		return * ( index + &x );
	}

    double operator [] ( int index ) const
	{
		return * ( index + &x );
	}

	int	operator == ( const Vector3D_D& v ) const
	{
		return x == v.x && y == v.y && z == v.z;
	}

	int	operator != ( const Vector3D_D& v ) const
	{
		return x != v.x || y != v.y || z != v.z;
	}

    operator double * ()
	{
		return &x;
	}

    operator const double * () const
	{
		return &x;
	}

    double	length () const
	{
        return (double) sqrt ( x * x + y * y + z * z );
	}

    double   lengthSq () const
    {
        return x * x + y * y + z * z;
    }

	Vector3D_D&	normalize ()
	{
		return (*this) /= length ();
	}

    double   maxLength () const
    {
        return max3 ( (double)fabs (x), (double)fabs (y), (double)fabs (z) );
    }

    double   distanceToSq ( const Vector3D_D& p ) const
    {
        return sqr ( x - p.x ) + sqr ( y - p.y ) + sqr ( z - p.z );
    }

    double   distanceTo ( const Vector3D_D& p ) const
    {
        return (double)sqrt ( sqr ( x - p.x ) + sqr ( y - p.y ) + sqr ( z - p.z ) );
    }

    double	distanceToAlongAxis ( const Vector3D_D& p, int axis ) const
	{
        return (double)fabs ( operator [] ( axis ) - p [axis] );
	}

	int	getMainAxis () const
	{
		int		axis = 0;
        double	val  = (double) fabs ( x );

		for ( register int i = 1; i < 3; i++ )
		{
            double	vNew = (double) fabs ( operator [] ( i ) );

			if ( vNew > val )
			{
				val  = vNew;
				axis = i;
			}
		}

		return axis;
	}

    Vector3D_D&	clamp ( double lower, double upper );

    static	Vector3D_D	getRandomVector ( double len = 1 );

	static	inline	Vector3D_D vmin ( const Vector3D_D& v1, const Vector3D_D& v2 )
	{
		return Vector3D_D ( v1.x < v2.x ? v1.x : v2.x, v1.y < v2.y ? v1.y : v2.y, v1.z < v2.z ? v1.z : v2.z );
	}

	static	inline	Vector3D_D vmax ( const Vector3D_D& v1, const Vector3D_D& v2 )
	{
		return Vector3D_D ( v1.x > v2.x ? v1.x : v2.x, v1.y > v2.y ? v1.y : v2.y, v1.z > v2.z ? v1.z : v2.z );
	}

	friend Vector3D_D operator + ( const Vector3D_D&, const Vector3D_D& );
	friend Vector3D_D operator - ( const Vector3D_D&, const Vector3D_D& );
	friend Vector3D_D operator * ( const Vector3D_D&, const Vector3D_D& );
    friend Vector3D_D operator * ( double,           const Vector3D_D& );
    friend Vector3D_D operator * ( const Vector3D_D&, double );
    friend Vector3D_D operator / ( const Vector3D_D&, double );
	friend Vector3D_D operator / ( const Vector3D_D&, const Vector3D_D& );
    friend double    operator & ( const Vector3D_D&, const Vector3D_D& );
	friend Vector3D_D operator ^ ( const Vector3D_D&, const Vector3D_D& );

    static	Vector3D_D	zero;
    static	Vector3D_D	one;

private:
    double   max3 ( double a, double b, double c ) const
    {
        return a > b ? (a > c ? a : (b > c ? b : c)) :
                       (b > c ? b : (a > c ? a : c));
    }

    double   min3 ( double a, double b, double c ) const
    {
        return a < b ? (a < c ? a : (b < c ? b : c)) :
                       (b < c ? b : (a < c ? a : c));
    }

    double   sqr ( double x ) const
    {
        return x*x;
    }
};

inline Vector3D_D operator + ( const Vector3D_D& u, const Vector3D_D& v )
{
	return Vector3D_D ( u.x + v.x, u.y + v.y, u.z + v.z );
}

inline Vector3D_D operator - ( const Vector3D_D& u, const Vector3D_D& v )
{
	return Vector3D_D ( u.x - v.x, u.y - v.y, u.z - v.z );
}

inline Vector3D_D operator * ( const Vector3D_D& u, const Vector3D_D& v )
{
	return Vector3D_D ( u.x*v.x, u.y*v.y, u.z * v.z );
}

inline Vector3D_D operator * ( const Vector3D_D& v, double a )
{
	return Vector3D_D ( v.x*a, v.y*a, v.z*a );
}

inline Vector3D_D operator * ( double a, const Vector3D_D& v )
{
	return Vector3D_D ( v.x*a, v.y*a, v.z*a );
}

inline Vector3D_D operator / ( const Vector3D_D& u, const Vector3D_D& v )
{
	return Vector3D_D ( u.x/v.x, u.y/v.y, u.z/v.z );
}

inline Vector3D_D operator / ( const Vector3D_D& v, double a )
{
	return Vector3D_D ( v.x/a, v.y/a, v.z/a );
}

inline Vector3D_D operator / ( double a, const Vector3D_D& v )
{
	return Vector3D_D ( a / v.x, a / v.y, a / v.z );
}

inline double operator & ( const Vector3D_D& u, const Vector3D_D& v )
{
	return u.x*v.x + u.y*v.y + u.z*v.z;
}

inline Vector3D_D operator ^ ( const Vector3D_D& u, const Vector3D_D& v )
{
	return Vector3D_D (u.y*v.z-u.z*v.y, u.z*v.x-u.x*v.z, u.x*v.y-u.y*v.x);
}

inline	Vector3D_D lerp ( const Vector3D_D& a, const Vector3D_D& b, double t )
{
	return a + t * (b - a);
}

inline	double	mixedProduct ( const Vector3D_D& a, const Vector3D_D& b, const Vector3D_D& c )
{
	return ( a & ( b ^ c ) );
}

inline	bool	areCollinear ( const Vector3D_D& a, const Vector3D_D& b, const Vector3D_D& c )
{
	return ((b - a) ^ (c - a)).lengthSq () < EPS * EPS;
}

inline	bool	areComplanar ( const Vector3D_D& a, const Vector3D_D& b, const Vector3D_D& c, const Vector3D_D& d )
{
	return fabs ( mixedProduct ( b - a, c - a, d - a ) ) < EPS * EPS * EPS;
}

#endif
