/******************************************************************************************
 *	Chili DirectX Framework Version 16.07.20											  *
 *	Game.cpp																			  *
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
#include "MainWindow.h"
#include "Game.h"

#include "Testing.h"

#include <utility>
#include <random>
#include <string>
#include <sstream>

using namespace Math;
using namespace Testing;
using namespace Colors;

using namespace std;

Game::Game(MainWindow& wnd)
	:
	wnd(wnd),
	gfx(wnd)	
{
	SetTiles();

	timer.StartWatch();
}

void Game::Go()
{
	gfx.BeginFrame();
	Update();
	Draw();
	gfx.EndFrame();
}

void Game::Update()
{
	GetFrameTime();

	MoveCursor();	
	MoveTile();
	CheckForGameOver();
	ResetTiles();
	CloseGame();
}

void Game::Draw()
{
	DrawTiles();
		
	if (!gameOver) DrawCursor();
}

//---------------------------------------------------

void Game::SetTiles()
{
	SetImages();
	SetTextureCoordinates();
	SetPositions();
	SetTileImage();
	RandomiseTiles();
	//SetAdjacents();
}

void Game::SetImages()
{
	images.push_back(Surface::FromFile(L"Images\\numbersquare.png"));
	images.push_back(Surface::FromFile(L"Images\\mountain.jpg"));
	images.push_back(Surface::FromFile(L"Images\\earth.jpg"));
	images.push_back(Surface::FromFile(L"Images\\squirrel.jpg"));
	images.push_back(Surface::FromFile(L"Images\\pretty.jpg"));
}

void Game::SetTextureCoordinates()
{
	texCoords.clear();

	Vector topLeft;
	Vector topRight;
	Vector bottomLeft;
	Vector bottomRight;

	for (int i = 0; i < SIZE; i++)
	{
		const float LEFT = XFRACTION * (i % COLS);
		const float TOP = YFRACTION * (i / COLS);
		const float RIGHT = LEFT + XFRACTION;
		const float BOTTOM = TOP + YFRACTION;

		topLeft = { 
			std::max(LEFT, 0.0f),
			std::max(TOP, 0.0f) };

		topRight = { 
			std::min(RIGHT, 1.0f),
			std::max(TOP, 0.0f) };

		bottomLeft = { 
			std::max(LEFT, 0.0f),
			std::min(BOTTOM, 1.0f) };

		bottomRight = { 
			std::min(RIGHT, 1.0f),
			std::min(BOTTOM, 1.0f)};

		texCoords.push_back({
			topLeft,
			topRight,
			bottomLeft,
			bottomRight });
	}
}

void Game::SetPositions()
{
	positions.clear();

	Vector topLeft;
	Vector topRight;
	Vector bottomLeft;
	Vector bottomRight;

	for (int i = 0; i < SIZE; i++)
	{
		const float LEFT = (float)XOFF + WIDTH * (i % COLS);
		const float TOP = (float)YOFF + HEIGHT * (i / COLS);
		const float RIGHT = LEFT + WIDTH;
		const float BOTTOM = TOP + HEIGHT;

		topLeft = {	LEFT, TOP };
		topRight = { RIGHT, TOP };
		bottomLeft = { LEFT, BOTTOM };
		bottomRight = {	RIGHT, BOTTOM };

		positions.push_back({
			topLeft,
			topRight,
			bottomLeft,
			bottomRight });
	}
}

void Game::SetTileImage()
{
	imageNum++;
	if (imageNum >= images.size()) imageNum = 0;
}

void Game::RandomiseTiles()
{
	tiles.clear();

	std::vector<int> numbers;

	for (int i = 0; i < SIZE; i++)
	{
		numbers.push_back(i);
	}

	assert(numbers.size() == SIZE);

	for (int i = 0; i < SIZE; i++)
	{
		const int RANDOM_NUMBER = Random(0, (int)numbers.size() - 1);

		tiles.push_back({
			numbers[RANDOM_NUMBER],
			positions,
			i,
			texCoords});

		numbers.erase(numbers.begin() + RANDOM_NUMBER);
	}

	assert(numbers.empty() == true);
	assert(tiles.size() == SIZE);
}

void Game::SetAdjacents()
{
	assert(tiles.size() == SIZE);

	// set pointers to tiles on all sides
	// a nullptr means there is no tile on that side
	for (size_t i = 0; i < SIZE; i++)
	{
		const bool L = i % COLS == 0;
		const bool U = i < COLS;
		const bool R = i % (COLS - 1) == 0 && i > 0;
		const bool D = i > (SIZE - COLS - 1);

		if (L) tiles[i].SetLeft(nullptr);
		else   tiles[i].SetLeft(&tiles[i - 1]);

		if (U) tiles[i].SetUp(nullptr);
		else   tiles[i].SetUp(&tiles[i - COLS]);

		if (R) tiles[i].SetRight(nullptr);
		else   tiles[i].SetRight(&tiles[i + 1]);

		if (D) tiles[i].SetDown(nullptr);
		else   tiles[i].SetDown(&tiles[i + COLS]);
	}
}

void Game::GetFrameTime()
{
	timeNew = timer.GetTimeMilli();
	frameTime = timeNew - timeOld;
	timeOld = timeNew;
}

void Game::CheckForGameOver()
{
	if (!gameOver)
	{
		int count = 0;
		for (int i = 0; i < SIZE; i++)
		{
			if (tiles[i].GetPos() == i)
			{
				count++;
			}
			else
			{
				gameOver = false;
				break;
			}
		}

		if (count == SIZE) gameOver = true;
	}
}

void Game::MoveCursor()
{
	if (!arrowPressed)
	{
		/**/ if (wnd.kbd.KeyIsPressed(VK_LEFT))
		{
			arrowPressed = true;
			int count = 0;
			for (int i = 0; i < COLS; i++)
			{
				if (cursor != i * COLS)
				{
					count++;
				}
			}
			if (count == COLS) cursor--;
		}
		else if (wnd.kbd.KeyIsPressed(VK_RIGHT))
		{
			arrowPressed = true;
			int count = 0;
			for (int i = 1; i <= COLS; i++)
			{
				int j = i * COLS - 1;
				if (cursor != j)
				{
					count++;
				}
			}
			if (count == COLS) cursor++;
		}
		else if (wnd.kbd.KeyIsPressed(VK_UP))
		{
			arrowPressed = true;
			if (cursor > COLS - 1)
			{
				cursor -= COLS;
			}
		}
		else if (wnd.kbd.KeyIsPressed(VK_DOWN))
		{
			arrowPressed = true;
			if (cursor < (ROWS - 1) * COLS)
			{
				cursor += COLS;
			}
		}		
	}
	else
	{
		if (!wnd.kbd.KeyIsPressed(VK_LEFT) &&
			!wnd.kbd.KeyIsPressed(VK_RIGHT) &&
			!wnd.kbd.KeyIsPressed(VK_UP) &&
			!wnd.kbd.KeyIsPressed(VK_DOWN))
		{
			arrowPressed = false;
		}
	}
}

void Game::MoveTile()
{
	if (!spacePressed && !isMoving)
	{
		if (wnd.kbd.KeyIsPressed(VK_SPACE))
		{
			spacePressed = true;

			int cursorTileIndex = 0;
			for (size_t i = 0; i < SIZE; i++)
			{
				if (tiles[i].GetPos() == cursor)
				{
					cursorTileIndex = i;
				}
			}

			const int GAP = tiles[SIZE - 1].GetPos();
	
			const int GAP_COL = GAP % COLS;
			const int GAP_ROW = GAP / COLS;

			const int CUR_COL = cursor % COLS;
			const int CUR_ROW = cursor / COLS;

			const bool CURSOR_LEFT_OF_GAP = cursor == GAP - 1;
			const bool CURSOR_RIGHT_OF_GAP = cursor == GAP + 1;
			const bool CURSOR_IN_SAME_ROW_AS_GAP = CUR_ROW == GAP_ROW;

			const bool CURSOR_ABOVE_GAP = cursor == GAP - COLS;
			const bool CURSOR_BELOW_GAP = cursor == GAP + COLS;
			const bool CURSOR_IN_SAME_COLUMN_AS_GAP = CUR_COL == GAP_COL;

			if (GAP != cursor)
			{
				if (
						(
							(CURSOR_LEFT_OF_GAP || CURSOR_RIGHT_OF_GAP)  
							&& 
							CURSOR_IN_SAME_ROW_AS_GAP
						)
						||
						(	
							(CURSOR_ABOVE_GAP || CURSOR_BELOW_GAP)
							&& 
							CURSOR_IN_SAME_COLUMN_AS_GAP
						)
					)
				{
					tiles[SIZE - 1].SetPosition(cursor);
					tiles[cursorTileIndex].SetMove(GAP);
					isMoving = true;
				}
			}
		}		
	}
	else
	{
		if (!wnd.kbd.KeyIsPressed(VK_SPACE))
		{
			spacePressed = false;
		}
	}
				
	// do movement
	for (Tile& t : tiles)
	{
		if (t.IsMoving())
		{
			t.Move(frameTime);
		}
		else
		{
			isMoving = false;
		}
	}	
}

void Game::ResetTiles()
{
	if (!returnPressed)
	{
		if (wnd.kbd.KeyIsPressed(VK_RETURN))
		{
			returnPressed = true;
			SetTileImage();
			RandomiseTiles();
			SetAdjacents();
			gameOver = false;
		}
	}
	else
	{
		if (!wnd.kbd.KeyIsPressed(VK_RETURN))
		{
			returnPressed = false;
		}
	}
}

void Game::DrawTiles()
{
	for( const Tile& T : tiles )
	{
		if (T.GetTex() != SIZE - 1 || gameOver)
		{
			gfx.DrawTile(T, images[imageNum]);
		}
	}
}

void Game::DrawCursor()
{
	gfx.DrawRectangle(
		false,
		XOFF + cursor % COLS * WIDTH  + 10,
		YOFF + cursor / COLS * HEIGHT + 10,
		XOFF + cursor % COLS * WIDTH  + WIDTH  - 10,
		YOFF + cursor / COLS * HEIGHT + HEIGHT - 10,
		Red);
}

void Game::CloseGame()
{
	if (wnd.kbd.KeyIsPressed(VK_ESCAPE))
	{
		wnd.Kill();
	}
}
