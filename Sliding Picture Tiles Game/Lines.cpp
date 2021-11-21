#include "Lines.h"

#include "Math.h"

using namespace Math;

// CONSTRUCTORS & DESTRUCTOR
LineSegment::LineSegment( const float& AX, const float& AY, const float& BX, const float& BY )
{
    Set( AX, AY, BX, BY );
}
LineSegment::LineSegment( const Vector& A, const Vector& B )
{
    Set( A, B );
}
LineSegment::LineSegment( const LineSegment& LS )
{
    Set( LS );
}

// OPERATOR OVERLOADS
LineSegment& LineSegment::operator=( const LineSegment& LS ) noexcept
{
    Set( LS );
    return *this;
}

// FUNCTIONS
void    LineSegment::Set( const float& AX, const float& AY, const float& BX, const float& BY )
{
    this->a.x = AX;
    this->a.y = AY;
    this->b.x = BX;
    this->b.y = BY;

    isLineSet = false;
    isNormalSet = false;
    isAABBSet = false;
}
void    LineSegment::Set( const Vector& A, const Vector& B )
{
    this->a.x = A.x;
    this->a.y = A.y;
    this->b.x = B.x;
    this->b.y = B.y;

    isLineSet = false;
    isNormalSet = false;
    isAABBSet = false;
}
void    LineSegment::Set( const LineSegment& LS )
{
    this->a.x = LS.a.x;
    this->a.y = LS.a.y;
    this->b.x = LS.b.x;
    this->b.y = LS.b.y;

    isLineSet = false;
    isNormalSet = false;
    isAABBSet = false;
}
Vector  LineSegment::GetA() const
{
    return this->a;
}
Vector  LineSegment::GetB() const
{
    return this->b;
}
Vector  LineSegment::GetLine()
{
    if(isLineSet)
    {
        return this->line;
    }
    else
    {
        SetLine();
        return this->line;
    }
}
Vector  LineSegment::GetNormal()
{
    if(isNormalSet)
    {
        return this->normal;
    }
    else
    {
        SetNormal();
        return this->normal;
    }
}
RectF   LineSegment::GetAABB()
{
    if(isAABBSet)
    {
        return this->aabb;
    }
    else
    {
        SetAABB();
        return this->aabb;
    }
}
void    LineSegment::SetLine()
{
    this->line = {this->b.x - this->a.x,this->b.y - this->a.y};
    isLineSet = true;
}
void    LineSegment::SetNormal()
{
    if(isLineSet)
    {
        this->normal = Rotate90CCW( Normalise( line ) );
    }
    else
    {
        SetLine();
        this->normal = Rotate90CCW( Normalise( line ) );
    }

    isNormalSet = true;
}
void    LineSegment::SetAABB()
{
    this->aabb = RectF( this->a, this->b );
    isAABBSet = true;
}