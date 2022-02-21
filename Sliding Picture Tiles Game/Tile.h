#pragma once

#include "Math.h"

class Tile
{
public:
	Tile(
		const int& POSITION,
		const std::vector < std::array<Math::Vector, 4>>& POSITIONS,
		const int& TEX,
		const std::vector<std::array<Math::Vector, 4>>& TEX_COORDS)
		:
		POSITIONS(POSITIONS),
		TEX_COORDS(TEX_COORDS),
		TEX(TEX)
	{		
		assert(!POSITIONS.empty());
		assert(!TEX_COORDS.empty());

		SetTexCoord();
		SetPosition(POSITION);
	}
	~Tile()
	{
		for (size_t i = 0; i < pTile.size(); i++)
		{
			pTile[i] = nullptr;
		}
	}

private:
	static constexpr const float TIME_FOR_MOVE = 100.0f;

private:
	const std::vector < std::array<Math::Vector, 4>>& POSITIONS;
	const std::vector < std::array<Math::Vector, 4>>& TEX_COORDS;

	const int	TEX;
	int			pos				= 0;
	int			posStart		= 0;
	int			posEnd			= 0;
	bool		isMoving		= false;
	float		timeRemaining	= TIME_FOR_MOVE;

	std::array<Math::Vector, 4>	position;
	std::array<Math::Vector, 4>	texCoord;
	std::array<Tile*, 4>		pTile;

private:
	void	SetTexCoord()
	{
		assert(!TEX_COORDS.empty());
		texCoord[0] = TEX_COORDS[TEX][0];
		texCoord[1] = TEX_COORDS[TEX][1];
		texCoord[2] = TEX_COORDS[TEX][2];
		texCoord[3] = TEX_COORDS[TEX][3];
	}

public:	
	void	SetPosition(const int& POSITION)
	{		
		pos = POSITION;
		position[0] = POSITIONS[POSITION][0];
		position[1] = POSITIONS[POSITION][1];
		position[2] = POSITIONS[POSITION][2];
		position[3] = POSITIONS[POSITION][3];
	}
	void	SetLeft(Tile* t)
	{
		pTile[0] = t;
	}
	void	SetUp(Tile* t)
	{
		pTile[1] = t;
	}
	void	SetRight(Tile* t)
	{
		pTile[2] = t;
	}
	void	SetDown(Tile* t)
	{
		pTile[3] = t;
	}

	void	SetMove(const int& I)
	{
		if (!isMoving)
		{
			isMoving = true;

			posStart = pos;
			pos = I;
			posEnd = I;

			timeRemaining = TIME_FOR_MOVE;
		}
	}
	void	Move(const float& TIME)
	{
		timeRemaining -= TIME;
		float alpha = 1.0f - timeRemaining / TIME_FOR_MOVE;

		if (timeRemaining >= TIME)
		{
			position[0] = Math::Interpolate(POSITIONS[posStart][0], POSITIONS[posEnd][0], alpha);
			position[1] = Math::Interpolate(POSITIONS[posStart][1], POSITIONS[posEnd][1], alpha);
			position[2] = Math::Interpolate(POSITIONS[posStart][2], POSITIONS[posEnd][2], alpha);
			position[3] = Math::Interpolate(POSITIONS[posStart][3], POSITIONS[posEnd][3], alpha);
		}
		else
		{
			position[0] = POSITIONS[posEnd][0];
			position[1] = POSITIONS[posEnd][1];
			position[2] = POSITIONS[posEnd][2];
			position[3] = POSITIONS[posEnd][3];

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
	Tile*	GetLeft() const
	{
		return pTile[0];
	}
	Tile*	GetUp() const
	{
		return pTile[1];
	}
	Tile*	GetRight() const
	{
		return pTile[2];
	}
	Tile*	GetDown() const
	{
		return pTile[3];
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

		return texCoord[I];
	}
};
