#include "Vector.h"

namespace Math
{
	class TextureVertex
	{
	public:
		TextureVertex() = default;
		TextureVertex(const Vector& pos, const Vector& tc)
			:
			pos(pos),
			tc(tc)
		{}
		TextureVertex InterpolateTo(const TextureVertex& dest, float alpha) const
		{
			return{
				Math::Interpolate(pos, dest.pos,alpha),
				Math::Interpolate(tc, dest.tc,alpha)
			};
		}
		TextureVertex& operator+=(const TextureVertex& rhs)
		{
			pos += rhs.pos;
			tc += rhs.tc;
			return *this;
		}
		TextureVertex operator+(const TextureVertex& rhs) const
		{
			return TextureVertex(*this) += rhs;
		}
		TextureVertex& operator-=(const TextureVertex& rhs)
		{
			pos -= rhs.pos;
			tc -= rhs.tc;
			return *this;
		}
		TextureVertex operator-(const TextureVertex& rhs) const
		{
			return TextureVertex(*this) -= rhs;
		}
		TextureVertex& operator*=(float rhs)
		{
			pos *= rhs;
			tc *= rhs;
			return *this;
		}
		TextureVertex operator*(float rhs) const
		{
			return TextureVertex(*this) *= rhs;
		}
		TextureVertex& operator/=(float rhs)
		{
			pos /= rhs;
			tc /= rhs;
			return *this;
		}
		TextureVertex operator/(float rhs) const
		{
			return TextureVertex(*this) /= rhs;
		}
	public:
		Vector pos;
		Vector tc;
	};
}
