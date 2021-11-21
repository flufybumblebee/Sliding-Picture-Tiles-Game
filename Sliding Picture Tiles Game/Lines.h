#pragma once

#include "Vector.h"
#include "Geometry.h"

#include <algorithm>

namespace Math
{
	class LineSegment
	{
	public:
		LineSegment() = default;
		LineSegment( const float& AX, const float& AY, const float& BX, const float& BY );
		LineSegment( const Vector& A, const Vector& B );
		LineSegment( const LineSegment& LS );
		~LineSegment() = default;

	public:
		LineSegment& operator = ( const LineSegment& LS ) noexcept;

	public:
		void	Set( const float& AX, const float& AY, const float& BX, const float& BY );
		void	Set( const Vector& A, const Vector& B );
		void	Set( const LineSegment& LS );

	public:
		Vector	GetA() const;
		Vector	GetB() const;
		Vector	GetLine();
		Vector	GetNormal();
		RectF	GetAABB();

	public:
		Vector	a;
		Vector	b;
		Vector	line;
		Vector	normal;
		RectF	aabb;

	private:
		bool	isLineSet = false;
		bool	isNormalSet = false;
		bool	isAABBSet = false;

	private:
		void	SetLine();
		void	SetNormal();
		void	SetAABB();
	};

	class Ray
	{
	public:

	};

	class Line
	{
	public:

	};

	// STATIC FUNCTIONS
	static	float	LengthSquared( const Math::LineSegment& LS )
	{
		return LengthSquared( Subtract( LS.b, LS.a ) );
	}
	static	float	Length( const Math::LineSegment& LS )
	{
		return Length( Subtract( LS.b, LS.a ) );
	}
	static	float	Slope( const Math::LineSegment& LS )
	{
		float slope = 0.0f;
		const float RUN = std::move( LS.a.x - LS.b.x );
		const float RISE = std::move( LS.a.y - LS.b.x );

		return (RISE >= RUN) ? RISE / RUN : RUN / RISE;
	}
}


