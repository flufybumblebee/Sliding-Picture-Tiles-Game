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

#include <Mmsystem.h>
#include <mciapi.h>

#pragma comment(lib, "Winmm.lib")

using namespace Math;
using namespace Testing;
using namespace Colors;

using namespace std;

Game::Game(MainWindow& wnd)
	:
	wnd(wnd),
	gfx(wnd)	
{
	SetImages();
	SetTextureCoordinates();
	SetPositions();
	RandomiseImage();

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
	#ifdef NDEBUG
	GetFrameTime();
	#else
	frameTime = 10.0f;
	#endif
	
	if (!isSetUp)
	{	
		if (!isMoving)
		{
			std::vector<int> numbers = { 0,1,2,3 };
			assert(numbers.size() == 4);

			if(lastDir == 1)		numbers.erase(numbers.begin() + 0);
			if(lastDir == -1)		numbers.erase(numbers.begin() + 1);
			if(lastDir == COLS)		numbers.erase(numbers.begin() + 2);
			if(lastDir == -COLS)	numbers.erase(numbers.begin() + 3);

			int randomNum = Random(0, (int)numbers.size() - 1);

			const int GAP = tiles.back().GetPos();

			while (!IsNextToGap(GAP + NUMBERS[numbers[randomNum]]))
			{
				numbers.erase(numbers.begin() + randomNum);
				randomNum = Random(0, (int)numbers.size() - 1);
			}

			lastDir = NUMBERS[numbers[randomNum]];
			const int NEXT_TO_GAP = GAP + NUMBERS[numbers[randomNum]];
			const int INDEX = GetTileIndex(NEXT_TO_GAP);

			tiles.back().SetPosition(NEXT_TO_GAP);
			tiles[INDEX].SetToMoving(GAP);
			isMoving = true;
			
			if (increment < INCR_MAX) increment++;
		}
		else
		{
			int j = 0;
			for (size_t i = 0; i < tiles.size(); i++)
			{
				if (tiles[i].IsMoving())
				{
					tiles[i].Move(frameTime);
					isMoving = true;
					j++;
				}
			}

			if (j == 0)
			{
				cursor = tiles.back().GetPos();
				isMoving = false;
				if(increment >= INCR_MAX) isSetUp = true;
			}
		}			
	}
	else
	{
		if(!gameOver) MoveCursor();
		MoveTile();

		if(!isMoving) CheckForGameOver();
		CheckForGameReset();
	}

	CheckForGameExit();
}

void Game::Draw()
{
	DrawTiles();
		
	if (!gameOver)
	{
		if(isSetUp) DrawCursor();
		DrawTileBorders();
	}
}

//---------------------------------------------------


void Game::SetImages()
{
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
		const float LEFT = XFRACTION * float(i % COLS);
		const float TOP = YFRACTION * float(i / COLS);
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
		const float LEFT = float(WIDTH * (i % COLS));
		const float TOP = float(HEIGHT * (i / COLS));
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

void Game::SetTiles()
{
	tiles.clear();
	assert(tiles.empty() == true);
	for (int i = 0; i < SIZE; i++)
	{
		tiles.push_back({
			i,
			positions,
			i,
			texCoords});
	}
	assert(tiles.size() == SIZE);
}

void Game::NextImage()
{
	imageNum++;
	if (imageNum >= images.size()) imageNum = 0;
}
void Game::RandomiseImage()
{
	if (!images.empty())
	{
		imageNum = Math::Random(0, (int)images.size() - 1);
	}
}

int  Game::GetTileIndex(const int& POS)
{
	for (int i = 0; i < SIZE; i++)
	{
		if (tiles[i].GetPos() == POS)
		{
			return i;
		}
	}

	return -1;
}
bool Game::IsNextToGap(const int& POS)
{
	if (POS < 0 || POS > tiles.size() - 1) return false;
	
	const int GAP = tiles.back().GetPos();

	if (GAP == POS) return false;

	const int GAP_COL = GAP % COLS;
	const int POS_COL = POS % COLS;
	
	if (GAP_COL == POS_COL && (POS == GAP + COLS || POS == GAP - COLS)) return true;
	
	const int GAP_ROW = GAP / COLS;
	const int POS_ROW = POS / COLS;
	
	if (GAP_ROW == POS_ROW && (POS == GAP + 1 || POS == GAP - 1)) return true;
		
	return false;
}

void Game::GetFrameTime()
{
	timeNew = timer.GetTimeMilli();
	frameTime = timeNew - timeOld;
	timeOld = timeNew;
}

void Game::MoveCursor()
{
	if (isKeyboard)
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
	else
	{
		if (wnd.mouse.IsInWindow())
		{
			const int MOUSE_X = wnd.mouse.GetPosX();
			const int MOUSE_Y = wnd.mouse.GetPosY();

			int left = 0;
			int top = 0;
			int right = 0;
			int bottom = 0;

			for (const Tile& T : tiles)
			{
				left = (int)T.GetPosition()[0].x;
				top = (int)T.GetPosition()[0].y;
				right = (int)T.GetPosition()[3].x;
				bottom = (int)T.GetPosition()[3].y;

				if (MOUSE_X >= left && MOUSE_X <= right && MOUSE_Y >= top && MOUSE_Y <= bottom)
				{
					cursor = T.GetPos();
				}
			}
		}
	}	
}
void Game::MoveTile()
{
	if (!isMoving)
	{
		if (isKeyboard)
		{
			if (!spacePressed)
			{
				if (wnd.kbd.KeyIsPressed(VK_SPACE))
				{
					spacePressed = true;

					const int CUR_INDEX = GetTileIndex(cursor);

					const int GAP = tiles.back().GetPos();

					if (IsNextToGap(cursor))
					{
						tiles.back().SetPosition(cursor);
						tiles[CUR_INDEX].SetToMoving(GAP);
						isMoving = true;
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
		}
		else
		{
			if (!mousePressed)
			{
				if (wnd.mouse.LeftIsPressed())
				{
					mousePressed = true;

					const int CUR_INDEX = GetTileIndex(cursor);

					const int GAP = tiles.back().GetPos();

					if (IsNextToGap(cursor))
					{
						tiles.back().SetPosition(cursor);
						tiles[CUR_INDEX].SetToMoving(GAP);
						PlaySound(L"Sounds\\pop1.wav", NULL, SND_ASYNC);
						isMoving = true;
					}
				}
			}
			else
			{
				if (!wnd.mouse.LeftIsPressed())
				{
					mousePressed = false;
				}
			}
		}
	}
	
	int j = 0;
	for (size_t i = 0; i < tiles.size(); i++)
	{
		if (tiles[i].IsMoving())
		{
			tiles[i].Move(frameTime);
			isMoving = true;
			j++;
		}
	}

	if (j == 0)
	{
		isMoving = false;
	}
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

		if (gameOver) soundPlayed = PlaySound(L"Sounds\\success3.wav", NULL, SND_ASYNC);
	}
}
void Game::CheckForGameReset()
{
	if (!returnPressed)
	{
		if (wnd.kbd.KeyIsPressed(VK_RETURN))
		{
			returnPressed = true;
			gameOver = false;
			isSetUp = false;
			increment = 0;
			NextImage();
			SetTiles();
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
void Game::CheckForGameExit()
{
	if (wnd.kbd.KeyIsPressed(VK_ESCAPE))
	{
		wnd.Kill();
	}
}

void Game::DrawTiles()
{
	for( const Tile& T : tiles )
	{
		if (T.GetTex() != tiles.size() - 1 || gameOver)
		{
			gfx.DrawTile(T, images[imageNum]);
		}
	}
}
void Game::DrawCursor()
{
	gfx.DrawRectangle(
		false,
		cursor % COLS * WIDTH  + 10,
		cursor / COLS * HEIGHT + 10,
		cursor % COLS * WIDTH  + WIDTH  - 10,
		cursor / COLS * HEIGHT + HEIGHT - 10,
		Red);
}
void Game::DrawTileBorders()
{
	for (const Tile& T : tiles)
	{
		if (T.GetTex() != tiles.size() - 1 || gameOver)
		{
			gfx.DrawRectangle(
				false,
				T.GetPosition()[0],
				T.GetPosition()[3],
				Black);
		}
	}
}