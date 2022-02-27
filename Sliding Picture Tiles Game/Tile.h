#pragma once

#include "Math.h"

class Tile
{
public:
	Tile(
		const int& POSITION,
		const std::vector<std::array<Math::Vector, 4>>& POSITIONS,
		const int& TEX,
		const std::vector<std::array<Math::Vector, 4>>& TEX_COORDS)
		:
		POSITIONS(POSITIONS),
		TEX(TEX),
		TEX_COORDS(TEX_COORDS)
	{		
		assert(POSITION >= 0);
		assert(TEX >= 0);
		SetPosition(POSITION);
	}
	~Tile() = default;

private:
	static constexpr const float TIME_FOR_MOVE = 250.0f;

private:
	const int	TEX;

	int			pos				= 0;
	int			startPos		= 0;
	int			endPos			= 0;
	bool		isMoving		= false;
	float		timeRemaining	= TIME_FOR_MOVE;

	std::array<Math::Vector, 4>	position;

	const std::vector<std::array<Math::Vector, 4>>& POSITIONS;
	const std::vector<std::array<Math::Vector, 4>>& TEX_COORDS;

public:	
	void	SetPosition(const int& POSITION)
	{		
		pos = POSITION;
		position = POSITIONS[POSITION];
	}
	void	SetToMoving(const int& I)
	{
		if (!isMoving)
		{
			isMoving = true;

			startPos = pos;
			pos = I;
			endPos = I;

			timeRemaining = TIME_FOR_MOVE;
		}
	}
	void	Move(const float& TIME)
	{
		timeRemaining -= TIME;
		float alpha = 1.0f - (timeRemaining / TIME_FOR_MOVE);

		if (timeRemaining >= TIME)
		{
			position[0] = Math::Interpolate(POSITIONS[startPos][0], POSITIONS[endPos][0], alpha);
			position[1] = Math::Interpolate(POSITIONS[startPos][1], POSITIONS[endPos][1], alpha);
			position[2] = Math::Interpolate(POSITIONS[startPos][2], POSITIONS[endPos][2], alpha);
			position[3] = Math::Interpolate(POSITIONS[startPos][3], POSITIONS[endPos][3], alpha);
		}
		else
		{
			position[0] = POSITIONS[endPos][0];
			position[1] = POSITIONS[endPos][1];
			position[2] = POSITIONS[endPos][2];
			position[3] = POSITIONS[endPos][3];

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

	const std::array<Math::Vector, 4>& GetPosition() const
	{
		return position;
	}
	const std::array<Math::Vector, 4>& GetTexCoord() const
	{
		return TEX_COORDS[TEX];
	}
};
