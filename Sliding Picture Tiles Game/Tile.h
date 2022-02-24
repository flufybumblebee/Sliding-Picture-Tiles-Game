#pragma once

#include "Math.h"

class Tile
{
public:
	Tile(
		const int& POSITION,
		const std::array<Math::Vector, 4>& POSITIONS,
		const int& TEX,
		const std::array<Math::Vector, 4>& TEX_COORDS)
		:
		TEX(TEX),
		TEX_COORD(TEX_COORDS)
	{		
		assert(POSITION >= 0);
		assert(TEX >= 0);
		SetPosition(POSITION, POSITIONS);
	}
	~Tile() = default;

private:
	static constexpr const float TIME_FOR_MOVE = 150.0f;

private:
	const int	TEX;
	const std::array<Math::Vector, 4> TEX_COORD;

	int			pos				= 0;
	bool		isMoving		= false;
	float		timeRemaining	= TIME_FOR_MOVE;

	std::array<Math::Vector, 4>	position;
	std::array<Math::Vector, 4>	startPosition;
	std::array<Math::Vector, 4>	endPosition;

public:	
	void	SetPosition(const int& POSITION, const std::array<Math::Vector,4>& POSITIONS)
	{		
		pos = POSITION;
		position = POSITIONS;
	}

	void	SetToMoving(const int& I, const std::array<Math::Vector,4>& END_POSITION)
	{
		if (!isMoving)
		{
			isMoving = true;

			pos = I;
			startPosition = position;
			endPosition = END_POSITION;

			timeRemaining = TIME_FOR_MOVE;
		}
	}
	void	Move(const float& TIME)
	{
		timeRemaining -= TIME;
		float alpha = 1.0f - (timeRemaining / TIME_FOR_MOVE);

		if (timeRemaining >= TIME)
		{
			position[0] = Math::Interpolate(startPosition[0], endPosition[0], alpha);
			position[1] = Math::Interpolate(startPosition[1], endPosition[1], alpha);
			position[2] = Math::Interpolate(startPosition[2], endPosition[2], alpha);
			position[3] = Math::Interpolate(startPosition[3], endPosition[3], alpha);
		}
		else
		{
			position[0] = endPosition[0];
			position[1] = endPosition[1];
			position[2] = endPosition[2];
			position[3] = endPosition[3];

			isMoving = false;
		}
	}

	int		GetTex() const
	{
		return TEX;
	}
	int		GetPos() const
	{
		return pos;
	}
	bool	IsMoving() const
	{
		return isMoving;
	}
	
	const Math::Vector& GetPosition(const int& I) const
	{
		assert(I >= 0);
		assert(I <= 3);

		return position[I];
	}
	const Math::Vector& GetTexCoord(const int& I) const
	{
		assert(I >= 0);
		assert(I <= 3);

		return TEX_COORD[I];
	}
};
