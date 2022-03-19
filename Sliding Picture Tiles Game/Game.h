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

#include <Maths.h>
#include "Tile.h"

#include <array>
#include <vector>
#include <string>

namespace BUTTON
{
	const unsigned int SETTINGS			= 0u;
	const unsigned int MOUSE			= 1u;
	const unsigned int KEYBOARD			= 2u;
	const unsigned int CONTROLLER		= 3u;
	const unsigned int INCREASE_ROWS	= 4u;
	const unsigned int DECREASE_ROWS	= 5u;
	const unsigned int NEW_GAME			= 6u;
	const unsigned int INCREASE_COLS	= 7u;
	const unsigned int DECREASE_COLS	= 8u;
}

namespace IMAGE
{
	const unsigned int SETTINGS = 0u;
	const unsigned int MOUSE = 1u;
	const unsigned int KEYBOARD = 2u;
	const unsigned int CONTROLLER = 3u;
	const unsigned int TRIANGLE = 4u;
	const unsigned int NEW_GAME = 5u;
}

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
	Timer timer;

	float	frameTime	= 0.0f;
	float	timeOld		= 0.0f;
	float	timeNew		= 0.0f;

	int		nMoves		= 0;
	int		prevDir		= 0;

	unsigned int imageNum	= 0;
	unsigned int cursor		= 0;	
	
	unsigned int rows		= 3; // default = 3
	unsigned int cols		= 3; // default = 3

	unsigned int nextRows	= rows;
	unsigned int nextCols	= cols;			
	

	bool	isKeyboard		= false;

	bool	arrowPressed	= false;
	bool	spacePressed	= false;
	bool	returnPressed	= false;

	bool	keyPressed		= false;
	bool	mousePressed	= false;

	bool	gameOver		= false;
	bool	isMoving		= false;
	bool	isSetUp			= false;

	bool	soundPlayed		= false;

	bool isTesting = false;

	std::vector<bool> mouseOver;

	std::vector<Surface> backgroundImages;
	std::vector<Surface> buttonImages;
	std::vector<Surface> tileImages;
	std::vector<Surface> digitImages;
	std::vector<Surface> symbolImages;

	std::vector<std::array<Math::Vector, 4>> digitPositions;
	std::vector<std::array<Math::Vector, 4>> buttonPositions;
	std::vector<std::array<Math::Vector, 4>> tilePositions;
	std::vector<std::array<Math::Vector, 4>> tileTexCoords;
	std::vector <Tile> tiles;

	Math::RectI tilesRect;

	enum class ControlState
	{
		Mouse,
		Keyboard,
		Controller
	} controlState;

	enum class GameState
	{
		Settings,
		Setup,
		Play
	} gameState;

public:

	void ReadDirectory(const std::wstring& FILENAME, std::vector<std::wstring>& v);

	void SetBackgroundImages();
	void SetButtonImages();
	void SetTileImages();
	void SetDigitImages();
	void SetSymbolImages();

	void SetDigitPositions();
	void SetButtonPositions();
	void SetTileTextureCoordinates();
	void SetTilePositions();
	void SetTiles();
	void SetTilesRect();
	void RandomiseTileImage();
	void NextTileImage();

	int	 GetTileIndex(const int& POS);
	bool IsNextToGap(const int& POS);

	void GetFrameTime();

	void MoveTile();

	void RestartGame();
	void NewGame();

	void CheckForGameOver();

	void DrawBackground();

	void DrawButtonSettings();
	void DrawButtonMouse();
	void DrawButtonKeyboard();
	void DrawButtonController();
	void DrawButtonIncreaseRows();
	void DrawButtonDecreaseRows();
	void DrawButtonNewGame();
	void DrawButtonIncreaseCols();
	void DrawButtonDecreaseCols();

	void DrawDigit(const std::array<Math::Vector,4>& POSITION, const int& number);

	void DrawSymbolRows();
	void DrawSymbolCols();

	void DrawTiles();
	void DrawCursor();
	void DrawTileBorders();
};

/*

NOTES:

TO DO (BUGS):
keyboard can't access access settings

TO DO (FEATURES):
move counter
timer (start when first tile moved, stops when all tiles in correct possitions)
select the number of tiles
select the number of tile shuffles

TO DO (SPECIAL FEATURES):
be able to add custom tile images DONE!
save/load game
controller support
auto solver
convert tiles to greyscale that are in the wrong place
victory animation
looping video tiles / moving gif tiles
use still image from a video for the tiles...
...then play the video when completed 

TO DO: (ACCESSABILITY):
audio for tiles (presses, cursor over)
audio for buttons (presses, cursor over)
audio for rows and column counters
audio move counter
color blind options
larger fonts options
*/