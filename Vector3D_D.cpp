//
// Basic class for 3d vectors.
// Supports all basic vector operations
//
// Author: Alex V. Boreskoff
//

#include	<stdlib.h>
#include	"Vector3D_D.h"

Vector3D_D	Vector3D_D :: zero ( 0, 0, 0 );
Vector3D_D	Vector3D_D :: one  ( 1, 1, 1 );


static	inline	double	rnd ()		// return double random number in [-1,1]
{
    return 2.0f * double ( rand () ) / double ( RAND_MAX ) - 1.0f;
}

Vector3D_D	Vector3D_D :: getRandomVector ( double len )
{
	Vector3D_D	v;

	for ( ; ; )
	{
		v.x = rnd ();
		v.y = rnd ();
		v.z = rnd ();

		if ( v.lengthSq () < EPS )
			continue;

		v *= len / v.length ();

		return v;
	}
}

Vector3D_D&	Vector3D_D :: clamp ( double lower, double upper )
{
	if ( x < lower )
		x = lower;
	else
	if ( x > upper )
		x = upper;

	if ( y < lower )
		y = lower;
	else
	if ( y > upper )
		y = upper;
			
	if ( z < lower )
		z = lower;
	else
	if ( z > upper )
		z = upper;

	return *this;
}

