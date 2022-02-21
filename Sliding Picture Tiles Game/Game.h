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
	
	int		imageNum		= 0;
	int		cursor			= 0;
	bool	arrowPressed	= false;
	bool	spacePressed	= false;
	bool	returnPressed	= false;
	bool	keyPressed		= false;
	bool	gameOver		= false;
	bool	isMoving		= false;
			
	std::vector<Surface> images;

	Timer timer;
	float frameTime	= 0.0f;
	float timeOld	= 0.0f;
	float timeNew	= 0.0f;

	const int XOFF		= 50;
	const int YOFF		= 50;
	const int ROWS		= 5;
	const int COLS		= 5;
	const int SIZE		= ROWS * COLS;
	const int WIDTH		= (Graphics::WINDOW_HEIGHT - YOFF * 2) / ROWS;
	const int HEIGHT	= WIDTH;

	const float XFRACTION = 1.0f / COLS;
	const float YFRACTION = 1.0f / ROWS;

	std::vector<std::array<Math::Vector, 4>> positions;
	std::vector<std::array<Math::Vector, 4>> texCoords;

	std::vector <Tile> tiles;

public:
	void SetTiles();
	void SetImages();
	void SetTextureCoordinates();
	void SetPositions();
	void GetFrameTime();
	void SetTileImage();
	void RandomiseTiles();
	void SetAdjacents();

	void CheckForGameOver();
	void MoveCursor();
	void MoveTile();
	void ResetTiles();

	void DrawTiles();
	void DrawCursor();

	void CloseGame();
};