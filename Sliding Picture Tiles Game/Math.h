#pragma once

#include "Colors.h"		// Color
#include "Vector.h"		// Vector
#include "Matrix.h"		// Matrix
#include "Lines.h"		// LineSegment, (Todo: Ray, Line)
#include "Geometry.h"	// Rect, AABB, Circle, Triangle
#include "TextureVertex.h"

#include <cmath>		// sqrt
#include <algorithm>	// move, min, max
#include <random>		// random_device, mt19937, uniform_real_distribution, uniform_int_distribution 
#include <cassert>		// assert

namespace Math
{
	// Constants

	constexpr float PI = 3.14159265f;
	constexpr double PI_D = 3.1415926535897932;

	// Classes & Structs

	struct VectorTT
	{
		Vector top;
		Vector tail;
	};

	//---------------------------------------------

	// General Functions	
	template <typename T> static T		Absolute( const T& VALUE )
	{
		return (VALUE < (T)0) ? T( -VALUE ) : VALUE;
	}
	template <typename T> static T		Square( const T& VALUE )
	{
		return VALUE * VALUE;
	}
	template <typename T> static T		Normalise( const T& MIN, const T& MAX, const T& VALUE )
	{
		/*
		lambda version for inside functions:
		auto normalise = []( const auto& MIN, const auto& MAX, const auto& VALUE ) { return (VALUE - MIN) / (MAX - MIN); };
		*/

		assert( MIN <= MAX );
		return (VALUE - MIN) / (MAX - MIN);
	}
	template <typename T> static T		WrapAngle( const T& ANGLE )
	{
		const T modded = fmod( ANGLE, (T)2.0 * (T)PI_D );
		return (modded > (T)PI_D) ?
			(modded - (T)2.0 * (T)PI_D) :
			modded;
	}
	template <typename T> static T		DegreesToRadians( const T& DEGREES )
	{
		return DEGREES * PI / (T)180.0;
	}
	template <typename T> static T		RadiansToDegrees( const T& RADIANS )
	{
		return RADIANS * T( 180.0 / PI_D );
	}
	template <typename T> static void	Swap( T& a, T& b )
	{
		T temp = a;
		a = b;
		b = temp;
	}
	template <typename T> static void	Swap( T&& a, T&& b )
	{
		T temp( std::move( a ) );
		a = std::move( b );
		b = std::move( temp );
	}

	template <typename T0, typename T1>
	static auto Ternary( bool Expression1, T0 Expression2, T1 Expression3 )
	{
		return (Expression1) ? Expression2 : Expression3;
	}

	static	int		Modulo( const int& A, const int& B )
	{
		return A % B;
	}
	static	float	Random( const float& MIN, const float& MAX )
	{
		std::random_device rd;
		std::mt19937 engine( rd() );

		std::uniform_real_distribution<float> distribution( MIN, MAX );

		return distribution( engine );
	}
	static	int		Random( const int& MIN, const int& MAX )
	{
		assert( MIN <= MAX );

		std::random_device rd;
		std::mt19937 engine( rd() );

		std::uniform_int_distribution<int> distribution( MIN, MAX );

		return distribution( engine );
	}

	// Other Functions

	static	Vector	Reflect( LineSegment& LS, const Vector& V )
	{
		Vector lineNormal = Rotate90CCW( LS.GetNormal() );

		float length =
			DotProduct( V,
				(DotProduct( lineNormal, V ) > 0.0f)
				? lineNormal : Rotate180( lineNormal ) );

		return Add( V, Multiply( Multiply( lineNormal, length ), 2 ) );
	}

	static	float	DistanceBetween( const Vector& A, const Vector& B )
	{
		Vector result( A - B );
		return Length( result );
	}
	static	float	DistanceBetween( const Vector& A, const Circle& B )
	{
		return Length( A - B.GetPosition() ) - B.GetRadius();
	}
	static	float	DistanceBetween( const Circle& A, const Circle& B )
	{
		return Length( A.GetPosition() - B.GetPosition() )
			- (A.GetRadius() + B.GetRadius());
	}

	static	Vector	ClosestPoint( const class LineSegment& LS, const class Vector& V )
	{
		const Vector HYP = V - LS.a;
		const Vector ADJ = LS.b - LS.a;
		const Vector ADJ_NORMALIZED = Normalize( ADJ );

		const float	HYP_LENGTH = Length( HYP );
		const float ADJ_LENGTH = DotProduct( ADJ_NORMALIZED, HYP );
		//const float OPP_LENGTH = sqrt( Square( HYP_LENGTH ) - Square( ADJ_LENGTH ) );

		return LS.a + ADJ_NORMALIZED * ADJ_LENGTH;
	}

	//static	Vector	Intersection( const LineSegment& A, const LineSegment& B )
	//{
	//	/*
	//	based on math and code from the youtube channel 'Coding Maths':
	//	Episode 32: Line Intersections Part 1 (https://www.youtube.com/watch?v=4bIsntTiKfM&t=1s)
	//	Episode 33: Line Intersections Part 2 (https://www.youtube.com/watch?v=A86COO8KC58)
	//	*/
	//
	//	const float A1 = A.b.y - A.a.y;
	//	const float B1 = A.a.x - A.b.x;
	//	const float C1 = A1 * A.a.x + B1 * A.a.y;
	//
	//	const float A2 = B.b.y - B.a.y;
	//	const float B2 = B.a.x - B.b.x;
	//	const float C2 = A2 * B.a.x + B2 * B.a.y;
	//
	//	const float DENOMINATOR = A1 * B2 - A2 * B1;
	//
	//	if( DENOMINATOR == 0.0f )
	//	{
	//		return { 0.0f,0.0f };
	//	}
	//
	//	const float IX = (B2 * C1 - B1 * C2) / DENOMINATOR;
	//	const float IY = (A1 * C2 - A2 * C1) / DENOMINATOR;
	//
	//	/*const float IXN_0 = Normalise(A.a.x, A.b.x, IX);
	//	const float IYN_0 = Normalise(A.a.y, A.b.y, IY);
	//	const float IXN_1 = Normalise(B.a.x, B.b.x, IX);
	//	const float IYN_1 = Normalise(B.a.y, B.b.y, IY);
	//
	//	if ((IXN_0 >= 0.0f && IXN_0 <= 1.0f ||
	//		 IYN_0 >= 0.0f && IYN_0 <= 1.0f)
	//		 &&
	//		(IXN_1 >= 0.0f && IXN_1 <= 1.0f ||
	//		 IYN_1 >= 0.0f && IYN_1 <= 1.0f))
	//	{
	//		return { IX, IY };
	//	}
	//	else return { 0.0f, 0.0f };*/
	//
	//	return { IX,IY };
	//}

	static	bool	Overlap( const AABB& A, const AABB& B )
	{
		return false;
	}
	static	bool	Overlap( const RectI& A, const RectI& B )
	{
		if( A.left		< B.right &&
			A.right		> B.left &&
			A.top		< B.bottom &&
			A.bottom	> B.top )
		{
			return true;
		}
		else return false;
	}
	static	bool	Overlap( const RectF& A, const RectF& B )
	{
		if( A.left		< B.right &&
			A.right		> B.left &&
			A.top		< B.bottom &&
			A.bottom	> B.top)
		{
			return true;
		}
		else return false;
	}
	static	bool	Overlap( const Circle& A, const Circle& B )
	{
		return (DistanceBetween( A, B ) < 0.0f);
	}

	static	void	Constrain( Vector& v, const RectF& RECT )
	{
		if(v.x < RECT.left)   v.x = RECT.left;
		if(v.x > RECT.right)  v.x = RECT.right;
		if(v.y < RECT.top)    v.y = RECT.top;
		if(v.y > RECT.bottom) v.y = RECT.bottom;
	}
	static	void	Constrain( Circle& c, const RectF& RECT )
	{
		if(c.position.x - c.radius < RECT.left)
			c.position.x = RECT.left + c.radius;

		if(c.position.x + c.radius > RECT.right)
			c.position.x = RECT.right - c.radius;

		if(c.position.y - c.radius < RECT.top)
			c.position.y = RECT.top + c.radius;

		if(c.position.y + c.radius > RECT.bottom)
			c.position.y = RECT.bottom - c.radius;
	}
	static	void	Constrain( RectF& r, const RectF& RECT )
	{
		const float WIDTH = r.Width();
		if(r.left < RECT.left)
		{
			r.left = RECT.left;
			r.right = r.left + WIDTH - 1.0f;
		}
		else if(r.right > RECT.right)
		{
			r.right = RECT.right;
			r.left = r.right - WIDTH - 1.0f;
		}

		const float HEIGHT = r.Height();
		if(r.top < RECT.top)
		{
			r.top = RECT.top;
			r.bottom = r.top + HEIGHT - 1.0f;
		}
		else if(r.bottom > RECT.bottom)
		{
			r.bottom = RECT.bottom;
			r.top = r.bottom - HEIGHT - 1.0f;
		}
	}

	static	void	Wrap( float& f, const float MIN, const float MAX )
	{
		// CHECK IT WORKS

		if(f < MIN) f -= MAX - (MIN - f);
		else if(f > MAX) f += MIN + (f - MAX);
	}
	static	void	Wrap( Vector& v, const RectF& RECT )
	{
		// CHECK IT WORKS

		if(v.x < RECT.left)		v.x -= RECT.right - (RECT.left - v.x);
		else if(v.x > RECT.right)		v.x += RECT.left + (v.x - RECT.right);

		if(v.y < RECT.top)		v.y -= RECT.bottom - (RECT.top - v.y);
		else if(v.y > RECT.bottom)	v.y += RECT.top + (v.y - RECT.bottom);
	}

	static	bool	Contains( const Vector& P, const Circle& C )
	{
		return (Square( C.position.x - P.x ) + Square( C.position.y - P.y )) <= Square( C.radius );
	}
	static	bool	Contains( const Vector& P, const RectF& R )
	{
		return (P.x > R.left && P.x < R.right&& P.y > R.top && P.y < R.bottom);
	}

	static	RectF	MakeRect( const Circle& CIRCLE )
	{
		const float LEFT = CIRCLE.position.x - CIRCLE.radius;
		const float TOP = CIRCLE.position.y - CIRCLE.radius;
		const float RIGHT = CIRCLE.position.x + CIRCLE.radius;
		const float BOTTOM = CIRCLE.position.y + CIRCLE.radius;

		return {LEFT,TOP,RIGHT,BOTTOM};
	}
	static	RectF	MakeRect( const Math::LineSegment& LS )
	{
		const float LEFT = std::min( LS.a.x, LS.b.x );
		const float TOP = std::min( LS.a.y, LS.b.y );
		const float RIGHT = std::max( LS.a.x, LS.b.x );
		const float BOTTOM = std::max( LS.a.y, LS.b.y );

		return {LEFT,TOP,RIGHT,BOTTOM};
	}

	static	int		Offset( const int& LENGTH, const unsigned int& SIZE )
	{
		if(LENGTH < 0)
		{
			return SIZE - (Absolute( LENGTH ) % SIZE);
		}
		else return int( LENGTH % SIZE );
	}
		
	static	float			Interpolate( const float& SOURCE, const float& DESTINATION, const float& ALPHA )
	{
		assert( SOURCE >= 0.0f );
		assert( SOURCE <= 1.0f );

		assert( DESTINATION >= 0.0f );
		assert( DESTINATION <= 1.0f );

		assert( ALPHA >= 0.0f );
		assert( ALPHA <= 1.0f );

		return SOURCE + (DESTINATION - SOURCE) * ALPHA;
	}
	static	unsigned char	AlphaBlend( const unsigned char& A, const unsigned char& B, const unsigned char& ALPHA )
	{
		assert( A <= 255 );
		assert( B <= 255 );
		assert( ALPHA <= 255 );
		const unsigned char NUM = 255u;
		return (B * ALPHA + A * (NUM - ALPHA)) / NUM;
	}
	static	Color			AlphaBlend( const Color& A, const Color& B )
	{
		const unsigned char RED		= AlphaBlend( A.GetR(), B.GetR(), B.GetA() );
		const unsigned char GREEN	= AlphaBlend( A.GetG(), B.GetG(), B.GetA() );
		const unsigned char BLUE	= AlphaBlend( A.GetB(), B.GetB(), B.GetA() );

		return {RED,GREEN,BLUE};
	}

	static	std::pair<bool, Vector>	Intersection( const LineSegment& A, const LineSegment& B )
	{
		/*
		based on math and code from the youtube channel 'Coding Maths':
		Episode 32: Line Intersections Part 1 (https://www.youtube.com/watch?v=4bIsntTiKfM&t=1s)
		Episode 33: Line Intersections Part 2 (https://www.youtube.com/watch?v=A86COO8KC58)
		*/

		const float A1 = A.b.y - A.a.y;
		const float B1 = A.a.x - A.b.x;
		const float C1 = A1 * A.a.x + B1 * A.a.y;

		const float A2 = B.b.y - B.a.y;
		const float B2 = B.a.x - B.b.x;
		const float C2 = A2 * B.a.x + B2 * B.a.y;

		const float DENOMINATOR = A1 * B2 - A2 * B1;

		if(DENOMINATOR == 0.0f)
		{
			return {false, {0.0f,0.0f}};
		}

		const float IX = (B2 * C1 - B1 * C2) / DENOMINATOR;
		const float IY = (A1 * C2 - A2 * C1) / DENOMINATOR;

		auto normalise = []( const auto& MIN, const auto& MAX, const auto& VALUE ) { return (VALUE - MIN) / (MAX - MIN); };

		const float IXN_0( normalise( A.a.x, A.b.x, IX ) );
		const float IYN_0( normalise( A.a.y, A.b.y, IY ) );

		const float IXN_1( normalise( B.a.x, B.b.x, IX ) );
		const float IYN_1( normalise( B.a.y, B.b.y, IY ) );

		if((IXN_0 >= 0.0f && IXN_0 <= 1.0f ||
			IYN_0 >= 0.0f && IYN_0 <= 1.0f)
			&&
			(IXN_1 >= 0.0f && IXN_1 <= 1.0f ||
			IYN_1 >= 0.0f && IYN_1 <= 1.0f))
		{
			return {true, { IX, IY }};
		}
		else return {false, { IX, IY }};

		return {true, {IX, IY}};
	}
}
