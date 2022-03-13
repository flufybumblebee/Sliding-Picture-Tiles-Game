#pragma once

#include "Vector.h"

#include <cmath>

namespace Math
{
	template <class T>
	class Rect
	{
	public:
		Rect() = default;
		Rect( T left, T top, T right, T bottom )
		{
			this->left /****/ = std::min( left, right );
			this->right /***/ = std::max( left, right );
			this->top /*****/ = std::min( top, bottom );
			this->bottom /**/ = std::max( top, bottom );
		}
		Rect( const Vector& A, const Vector& B )
		{
			this->left /****/ = std::min( (T)A.x, (T)B.x );
			this->right /***/ = std::max( (T)A.x, (T)B.x );
			this->top /*****/ = std::min( (T)A.y, (T)B.y );
			this->bottom /**/ = std::max( (T)A.y, (T)B.y );
		}
		Rect( const Rect& RECT )
		{
			this->left /****/ = std::min<float>( RECT.left, RECT.right );
			this->right /***/ = std::max<float>( RECT.left, RECT.right );
			this->top /*****/ = std::min<float>( RECT.top, RECT.bottom );
			this->bottom /**/ = std::max<float>( RECT.top, RECT.bottom );
		}
		~Rect() = default;
	public:
				
		Rect& operator=( const Rect& RHS )
		{
			this->left = RHS.left;
			this->top = RHS.top;
			this->right = RHS.right;
			this->bottom = RHS.bottom;

			return *this;
		}
		void    Set( const Rect& RECT )
		{
			this->left /****/ = std::min( left, right );
			this->right /***/ = std::max( left, right );
			this->top /*****/ = std::min( top, bottom );
			this->bottom /**/ = std::max( top, bottom );
		}
		float   Width() const
		{
			return this->right - this->left;
		}
		float   Height() const
		{
			return this->bottom - this->top;
		}
	public:
		T left = (T)0.0;
		T top = (T)0.0;
		T right = (T)0.0;
		T bottom = (T)0.0;
	};
		
	using RectF = Rect<float>;
	using RectI = Rect<int>;

	class AABB
	{
	public:
		AABB(
			const float& MIN_X, const float& MIN_Y,
			const float& MAX_X, const float& MAX_Y );
		AABB( const Vector& MIN, const Vector& MAX );
	public:
		float minX = 0.0f;
		float minY = 0.0f;
		float maxX = 0.0f;
		float maxY = 0.0f;
	};

	class Circle
	{
	public:
		Circle() = default;
		Circle( const Circle& CIRCLE );
		Circle( const float& X, const float& Y, const float& RADIUS );
		Circle( const Vector& POSITION, const float& RADIUS );
		~Circle() = default;
	public:
		void	Set( const Circle& CIRCLE );
		void	Set( const float& X, const float& Y, const float& RADIUS );
		void	Set( const Vector& POSITION, const float& RADIUS );
		Vector	GetPosition() const;
		float	GetRadius() const;
	public:
		Vector	position;
		float	radius = 0.0f;
	};

	class Triangle
	{
	public:
		Triangle() = default;
		Triangle( const Triangle& T );
		Triangle( const Vector& A, const Vector& B, const Vector& C );
		Triangle(
			const float& AX, const float& AY, const float& AZ,
			const float& BX, const float& BY, const float& BZ,
			const float& CX, const float& CY, const float& CZ );
		~Triangle() = default;

	public:
		Vector GetA() const;
		Vector GetB() const;
		Vector GetC() const;

	public:
		Vector A;
		Vector B;
		Vector C;
	};
}
