#include "Geometry.h"

#include <algorithm>

using namespace std;
using namespace Math;

// ----------------------------------------------

// RECT CLASS



// -----------------------------------------------------

// CIRCLE CLASS

Circle::Circle( const Circle& CIRCLE )
{
    this->position = CIRCLE.position;
    this->radius = CIRCLE.radius;
}
Circle::Circle( const float& X, const float& Y, const float& RADIUS )
{
    this->position = {X,Y};
    this->radius = RADIUS;
}
Circle::Circle( const Vector& POSITION, const float& RADIUS )
{
    this->position = POSITION;
    this->radius = RADIUS;
}

void    Circle::Set( const Circle& CIRCLE )
{
    this->position = CIRCLE.position;
    this->radius = CIRCLE.radius;
}
void    Circle::Set( const float& X, const float& Y, const float& RADIUS )
{
    this->position = {X,Y};
    this->radius = RADIUS;
}
void    Circle::Set( const Vector& POSITION, const float& RADIUS )
{
    this->position = POSITION;
    this->radius = RADIUS;
}

Vector  Circle::GetPosition() const
{
    return this->position;
}
float   Circle::GetRadius() const
{
    return this->radius;
}

// ---------------------------------------------------

// TRIANGLE CLASS

Triangle::Triangle( const Triangle& T )
{
    this->A = T.GetA();
    this->B = T.GetB();
    this->C = T.GetC();
}
Triangle::Triangle( const Vector& A, const Vector& B, const Vector& C )
{
    this->A = A;
    this->B = B;
    this->C = C;
}
Triangle::Triangle( const float& AX, const float& AY, const float& AZ, const float& BX, const float& BY, const float& BZ, const float& CX, const float& CY, const float& CZ )
{
    this->A.x = AX;
    this->A.y = AY;
    this->A.z = AZ;

    this->B.x = BX;
    this->B.y = BY;
    this->B.z = BZ;

    this->C.x = CX;
    this->C.y = CY;
    this->C.z = CZ;
}

Vector  Triangle::GetA() const
{
    return this->A;
}
Vector  Triangle::GetB() const
{
    return this->B;
}
Vector  Triangle::GetC() const
{
    return this->C;
}

// ---------------------------------------------------
