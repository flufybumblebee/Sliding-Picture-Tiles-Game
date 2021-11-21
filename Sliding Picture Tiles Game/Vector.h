#pragma once

#include <cassert>	// assert
#include <cmath>	// sqrt
#include <utility>	// pair

namespace Math
{
	class Matrix;

	class Vector
	{
	public:
		Vector() = default;
		Vector( const Vector& V );
		Vector( const std::pair<int, int>& PAIR );
		Vector( const float& X, const float& Y );
		Vector( const float& X, const float& Y, const float& Z );
		Vector( const float& X, const float& Y, const float& Z, const float& W );
		~Vector() = default;

	public:
		Vector& operator =  ( const Vector& RHS );
		Vector	operator +  ( const Vector& RHS ) const;
		Vector	operator -  ( const Vector& RHS ) const;
		Vector& operator += ( const Vector& RHS );
		Vector& operator -= ( const Vector& RHS );
		Vector& operator *= ( const float& RHS );
		Vector	operator *  ( const float& RHS ) const;
		Vector& operator /= ( const float& RHS );
		bool 	operator == ( const Vector& RHS ) const;
		bool	operator != ( const Vector& RHS ) const;

		float	DotProduct( const Vector& RHS ) const;
		Vector	CrossProduct( const Vector& RHS ) const;
		float	Length() const;
		Vector& Normalise();

	public:
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float w = 1.0f;
	};

	// MATH FUNCTIONS
	static	Vector	Add( const Vector& A, const Vector& B )
	{
		return { A.x + B.x, A.y + B.y, A.z + B.z };
	}
	static	Vector	Subtract( const Vector& A, const Vector& B )
	{
		return { A.x - B.x, A.y - B.y };
	}

	template <typename T>
	static	Vector	Multiply( const Vector& V, const T& MUL )
	{
		return { V.x * MUL, V.y * MUL, V.z * MUL };
	}
	template <typename T>
	static	Vector	Divide( const Vector& V, const T& DIV )
	{
		return { V.x / DIV, V.y / DIV, V.z / DIV };
	}

	static	float	LengthSquared( const Vector& V )
	{
		return ( V.x * V.x ) + ( V.y * V.y ) + ( V.z * V.z );
	}
	static	float	Length( const Vector& V )
	{
		return std::sqrt( LengthSquared( V ) );
	}
	static	float	DotProduct( const float& AX, const float& AY, const float& BX, const float& BY )
	{
		return ( AX * BX ) + ( AY * BY );
	}
	static	float	DotProduct( const Vector& A, const Vector& B )
	{
		return ( A.x * B.x ) + ( A.y * B.y ) + ( A.z * B.z );
	}
	static	Vector	CrossProduct( const Vector& A, const Vector& B )
	{
		Vector result;

		return result;
	}
	static	Vector	Normalise( const Vector& V )
	{
		Vector result;
		float length = Length( V );

		//assert( length != 0.0f );
		if( length == 0.0f )
		{
			result.x = 0.0f;
			result.y = 0.0f;
		}
		else if( length < 1.0f )
		{
			result.x = V.x + V.x * ( 1.0f - length );
			result.y = V.y + V.y * ( 1.0f - length );
			result.z = V.z + V.z * ( 1.0f - length );

			assert( Length( result ) < 1.000001f );
			assert( Length( result ) > 0.999999f );
		}
		else
		{
			result.x = V.x / length;
			result.y = V.y / length;
			result.z = V.z / length;

			assert( Length( result ) < 1.000001f );
			assert( Length( result ) > 0.999999f );
		}

		return result;
	}
	static	Vector	Interpolate( const Vector& SOURCE, const Vector& DESTINATION, const float& ALPHA )
	{
		return SOURCE + Multiply( ( DESTINATION - SOURCE ), ALPHA );
	}
	static	Vector	Rotate90CCW( const Vector& V )
	{
		float temp = V.y;
		float y = -V.x;
		float x = temp;
		return { x, y, V.z, V.w };
	}
	static	Vector	Rotate90CW( const Vector& V )
	{
		float temp = V.y;
		float y = V.x;
		float x = -temp;
		return { x, y, V.z, V.w };
	}
	static	Vector	Rotate180( const Vector& V )
	{
		return { -V.x, -V.y, V.z, V.w };
	}
	static	Vector	Rotate( const Vector& V, const float& ANGLE )
	{
		return {
			V.x * cos( ANGLE ) - V.y * sin( ANGLE ),
			V.y * cos( ANGLE ) + V.x * sin( ANGLE ) };
	}
	/*static	Vector	Translate( const Vector& A, const Vector& B )
	{
		// make sure to rotate and scaling before translating
		return Add( A, B ); // not sure this is correct;
	}*/
	static	Vector	Invert( const Vector& V )
	{
		return { V.x * -1.0f, V.y * -1.0f, V.z * -1.0f };
	}

	// WINDOW TRANSFORM	
	static Vector	WindowTransform( const Vector& V, const float& WINDOW_WIDTH, const float& WINDOW_HEIGHT )
	{
		Vector result;

		result.x = V.x + WINDOW_WIDTH / 2.0f;
		result.y = -V.y + WINDOW_HEIGHT / 2.0f;

		return result;
	}
	static Vector	UnWindowTransform( const Vector& V, const float& WINDOW_WIDTH, const float& WINDOW_HEIGHT )
	{
		Vector result;

		result.x = V.x - WINDOW_WIDTH / 2.0f;
		result.y = -V.y - WINDOW_HEIGHT / 2.0f;

		return result;
	}
}
