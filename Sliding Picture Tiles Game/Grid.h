#pragma once

#include "Maths.h"

#include <vector>

class Grid
{
public:
	Grid( const Math::Rect<float>& RECT, const size_t& WIDTH, const size_t& HEIGHT )
		:
		RECT(RECT),
		WIDTH(WIDTH + 1),
		HEIGHT(HEIGHT + 1)
	{
		Set();
	}

public:
	const std::vector<std::vector<Math::Vector>>& GetGrid() const
	{
		return grid;
	}
	const size_t& GetWidth() const
	{
		return WIDTH;
	}
	const size_t& GetHeight() const
	{
		return HEIGHT;
	}

private:
	void Set()
	{
		const float LEFT( RECT.left );
		const float TOP( RECT.top );

		const float CELL_WIDTH = RECT.Width() / (WIDTH - 1u);
		const float CELL_HEIGHT = RECT.Height() / (HEIGHT - 1u);

		std::vector<Math::Vector> row;
		Math::Vector point;
		
		for( size_t iy = 0u; iy < HEIGHT; iy++ )
		{
			row.clear();
			point.y = TOP + iy * CELL_HEIGHT;
			for( size_t ix = 0u; ix < WIDTH; ix++ )
			{
				point.x = LEFT + ix * CELL_WIDTH;
				row.push_back( point );
			}
			grid.push_back( row );
		}
	}
private:
	const size_t WIDTH;
	const size_t HEIGHT;
	const Math::Rect<float> RECT;

	std::vector<std::vector<Math::Vector>> grid;

	std::vector<Math::LineSegment> horizontal;
	std::vector<Math::LineSegment> vertical;
};
