#include "Vector.h"

#include "Math.h"

using namespace Math;
using namespace std;

Vector::Vector( const Vector& V )
    :
    x( V.x ),
    y( V.y ),
    z( V.z ),
    w( V.w )
{}
Vector::Vector( const pair<int, int>& PAIR )
    :
    x((float)PAIR.first),
    y((float)PAIR.second)
{}
Vector::Vector( const float& X, const float& Y )
    :
    x( X ),
    y( Y )
{}
Vector::Vector( const float& X, const float& Y, const float& Z )
    :
    x( X ),
    y( Y ),
    z( Z )
{}
Vector::Vector( const float& X, const float& Y, const float& Z, const float& W )
    :
    x( X ),
    y( Y ),
    z( Z ),
    w( W )
{}

Vector& Vector::operator= ( const Vector& RHS )
{
    this->x = RHS.x;
    this->y = RHS.y;
    this->z = RHS.z;
    this->w = RHS.w;

    return *this;
}
Vector  Vector::operator+ ( const Vector& RHS ) const
{
    Vector result;

    result.x = this->x + RHS.x;
    result.y = this->y + RHS.y;
    result.z = this->z + RHS.z;
    result.w = this->w + RHS.w;

    return result;
}
Vector  Vector::operator- ( const Vector& RHS ) const
{
    Vector result;

    result.x = this->x - RHS.x;
    result.y = this->y - RHS.y;
    result.z = this->z - RHS.z;
    result.w = this->w - RHS.w;

    return result;
}
Vector& Vector::operator+=( const Vector& RHS )
{
    this->x += RHS.x;
    this->y += RHS.y;
    this->z += RHS.z;
    this->w += RHS.w;

    return *this;
}
Vector& Vector::operator-=( const Vector& RHS )
{
    this->x += RHS.x;
    this->y += RHS.y;
    this->z += RHS.z;
    this->w += RHS.w;

    return *this;
}
Vector& Vector::operator*=( const float& RHS )
{
    this->x *= RHS;
    this->y *= RHS;
    this->z *= RHS;
    this->w *= RHS;

    return *this;
}
Vector  Vector::operator* ( const float& RHS ) const
{
    Vector result = *this;

    result.x *= RHS;
    result.y *= RHS;
    result.z *= RHS;
    result.w *= RHS;

    return result;
}
Vector& Vector::operator/=( const float& RHS )
{
    this->x /= RHS;
    this->y /= RHS;
    this->z /= RHS;
    this->w /= RHS;

    return *this;
}
bool    Vector::operator==( const Vector& RHS ) const
{
    return (
        this->x == RHS.x &&
        this->y == RHS.y &&
        this->z == RHS.z &&
        this->w == RHS.w);
}

bool    Vector::operator!=( const Vector& RHS ) const
{
    return (
        this->x != RHS.x &&
        this->y != RHS.y &&
        this->z != RHS.z &&
        this->w != RHS.w);
}

float   Vector::DotProduct( const Vector& RHS ) const
{
    return this->x * RHS.x + this->y * RHS.y;
}
Vector& Vector::Normalise()
{
    float length = this->Length();

    this->x /= length;
    this->y /= length;
    this->z /= length;

    return *this;
}
float   Vector::Length() const
{
    return sqrt( Square( this->x ) + Square( this->y ) );
}