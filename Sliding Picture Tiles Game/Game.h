/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.h																				  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#pragma once

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include "Timer.h"
#include "Surface.h"

#include "Math.h"
#include "Tile.h"

#include <array>
#include <vector>
#include <string>
#include <list>
#include <queue>

class Game
{
public:
	Game( class MainWindow& wnd );
	Game( const Game& ) = delete;
	Game& operator=( const Game& ) = delete;
	void Go();
private:
	void Update();
	void Draw();
private:
	MainWindow& wnd;
	Graphics gfx;

public:
	int		increment		= 0;
	int		imageNum		= 0;
	int		cursor			= 0;

	bool	isKeyboard		= false;

	bool	arrowPressed	= false;
	bool	spacePressed	= false;
	bool	returnPressed	= false;
	bool	keyPressed		= false;

	bool	mousePressed	= false;

	bool	gameOver		= false;
	bool	isMoving		= false;
	bool	isSetUp			= false;
			
	std::vector<Surface> images;

	Timer timer;
	float frameTime	= 0.0f;
	float timeOld	= 0.0f;
	float timeNew	= 0.0f;

	const int ROWS		= 3;
	const int COLS		= 3;
	const int SIZE		= ROWS * COLS;
	const int INCR_MAX	= SIZE * 3;
	const int WIDTH		= (Graphics::WINDOW_WIDTH - 200) / COLS ;
	const int HEIGHT	= Graphics::WINDOW_HEIGHT / ROWS;

	const float XFRACTION = 1.0f / COLS;
	const float YFRACTION = 1.0f / ROWS;

	const std::array<int, 4> NUMBERS = { -1,1,-COLS,COLS };
	int lastDir = 0;

	std::vector<std::array<Math::Vector, 4>> positions;
	std::vector<std::array<Math::Vector, 4>> texCoords;

	std::vector <Tile> tiles;

public:
	void SetTiles();
	void SetImages();
	void SetTextureCoordinates();
	void SetPositions();
	void RandomiseImage();
	void NextImage();

	int	 GetTileIndex(const int& POS);
	bool IsNextToGap(const int& POS);

	void GetFrameTime();

	void MoveCursor();
	void MoveTile();

	void CheckForGameOver();
	void CheckForGameReset();
	void CheckForGameExit();

	void DrawTiles();
	void DrawCursor();
	void DrawTileBorders();
};

/*

NOTES:

TO DO (BUGS):
FIXED: unsolvable patterns
FIXED: tiles no longer sliding
FIXED: AI shouldn't be moving the same tile as the last moved tile
FIXED: borders dissapearing before the last tile finishes moving
FIXED: gap tile appearing before the last tile finishes moving
FIXED: cursor now appears only after setup in the gap square

TO DO (FEATURES):
sounds
victory animation
move counter
timer (start when first tile moved, stops when all tiles in correct possitions)
convert tiles to greyscale that are in the wrong place
auto solver
mouse control
controller control
select the difficulty by increasing or decreasing the number of tiles moved at start
select the number of rows and columns (min 2x2)
save/load game
be able to add custom images
looping video tiles / moving gif tiles

*/