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

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	image( Surface::FromFile( L"Images\\numbersquare.png"))
{
	RandomiseTiles();
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
	if (!keyPressed)
	{
		if (wnd.kbd.KeyIsPressed(VK_LEFT))
		{
			keyPressed = true;
			if (cursor != 0 && cursor != 3 && cursor != 6)
			{
				cursor--;
			}
		}
		else if (wnd.kbd.KeyIsPressed(VK_RIGHT))
		{
			keyPressed = true;
			if (cursor != 2 && cursor != 5 && cursor != 8)
			{
				cursor++;
			}
		}
		else if (wnd.kbd.KeyIsPressed(VK_UP))
		{
			keyPressed = true;
			if (cursor != 0 && cursor != 1 && cursor != 2)
			{
				cursor -= 3;
			}
		}
		else if (wnd.kbd.KeyIsPressed(VK_DOWN))
		{
			keyPressed = true;
			if (cursor != 6 && cursor != 7 && cursor != 8)
			{
				cursor += 3;
			}
		}

		if (wnd.kbd.KeyIsPressed(VK_RETURN))
		{
			keyPressed = true;

			/**/ if (cursor == 0)
			{
				if (nums[cursor + 1] == 8) // gap one to right of cursor
				{
					swap(nums[cursor + 0], nums[cursor + 1]);
				}
				else if (nums[cursor + 2] == 8) // gap two to right of cursor
				{
					swap(nums[cursor + 1], nums[cursor + 0]);
					swap(nums[cursor + 0], nums[cursor + 2]);
				}
				else if (nums[cursor + 3 * 1] == 8) // gap is one below cursor
				{
					swap(nums[cursor + 3 * 0], nums[cursor + 3 * 1]);
				}
				else if (nums[cursor + 3 * 2] == 8) // gap is two below cursor
				{
					swap(nums[cursor + 3 * 1], nums[cursor + 3 * 0]);
					swap(nums[cursor + 3 * 0], nums[cursor + 3 * 2]);
				}
			}
			else if (cursor == 1)
			{
				if (nums[cursor - 1] == 8) // gap one to left  of cursor
				{
					swap(nums[cursor - 0], nums[cursor - 1]);
				}
				else if (nums[cursor + 1] == 8) // gap one to right of cursor
				{
					swap(nums[cursor + 0], nums[cursor + 1]);
				}
				else if (nums[cursor + 3 * 1] == 8) // gap is one below cursor
				{
					swap(nums[cursor + 3 * 0], nums[cursor + 3 * 1]);
				}
				else if (nums[cursor + 3 * 2] == 8) // gap is two below cursor
				{
					swap(nums[cursor + 3 * 1], nums[cursor + 3 * 0]);
					swap(nums[cursor + 3 * 0], nums[cursor + 3 * 2]);
				}
			}
			else if (cursor == 2)
			{
				if (nums[cursor - 1] == 8) // gap one to left  of cursor
				{
					swap(nums[cursor - 0], nums[cursor - 1]);
				}
				else if (nums[cursor - 2] == 8) // gap two to left of cursor
				{
					swap(nums[cursor - 1], nums[cursor - 0]);
					swap(nums[cursor - 0], nums[cursor - 2]);
				}
				else if (nums[cursor + 3 * 1] == 8) // gap is one below cursor
				{
					swap(nums[cursor + 3 * 0], nums[cursor + 3 * 1]);
				}
				else if (nums[cursor + 3 * 2] == 8) // gap is two below cursor
				{
					swap(nums[cursor + 3 * 1], nums[cursor + 3 * 0]);
					swap(nums[cursor + 3 * 0], nums[cursor + 3 * 2]);
				}
			}
			else if (cursor == 3)
			{
				if (nums[cursor + 1] == 8) // gap one to right of cursor
				{
					swap(nums[cursor + 0], nums[cursor + 1]);
				}
				else if (nums[cursor + 2] == 8) // gap two to right of cursor
				{
					swap(nums[cursor + 1], nums[cursor + 0]);
					swap(nums[cursor + 0], nums[cursor + 2]);
				}
				else if (nums[cursor - 3 * 1] == 8) // gap is one above cursor
				{
					swap(nums[cursor - 3 * 0], nums[cursor - 3 * 1]);
				}
				else if (nums[cursor + 3 * 1] == 8) // gap is one below cursor
				{
					swap(nums[cursor + 3 * 0], nums[cursor + 3 * 1]);
				}
			}
			else if (cursor == 4)
			{
				if (nums[cursor - 1] == 8) // gap one to left  of cursor
				{
					swap(nums[cursor - 0], nums[cursor - 1]);
				}
				else if (nums[cursor + 1] == 8) // gap one to right of cursor
				{
					swap(nums[cursor + 0], nums[cursor + 1]);
				}
				else if (nums[cursor - 3 * 1] == 8) // gap is one above cursor
				{
					swap(nums[cursor - 3 * 0], nums[cursor - 3 * 1]);
				}
				else if (nums[cursor + 3 * 1] == 8) // gap is one below cursor
				{
					swap(nums[cursor + 3 * 0], nums[cursor + 3 * 1]);
				}
			}
			else if (cursor == 5)
			{
				if (nums[cursor - 1] == 8) // gap one to left  of cursor
				{
					swap(nums[cursor - 0], nums[cursor - 1]);
				}
				else if (nums[cursor - 2] == 8) // gap two to left of cursor
				{
					swap(nums[cursor - 1], nums[cursor - 0]);
					swap(nums[cursor - 0], nums[cursor - 2]);
				}
				else if (nums[cursor - 3 * 1] == 8) // gap is one above cursor
				{
					swap(nums[cursor - 3 * 0], nums[cursor - 3 * 1]);
				}
				else if (nums[cursor + 3 * 1] == 8) // gap is one below cursor
				{
					swap(nums[cursor + 3 * 0], nums[cursor + 3 * 1]);
				}
			}
			else if (cursor == 6)
			{
				if (nums[cursor + 1] == 8) // gap one to right of cursor
				{
					swap(nums[cursor + 0], nums[cursor + 1]);
				}
				else if (nums[cursor + 2] == 8) // gap two to right of cursor
				{
					swap(nums[cursor + 1], nums[cursor + 0]);
					swap(nums[cursor + 0], nums[cursor + 2]);
				}
				else if (nums[cursor - 3 * 1] == 8) // gap is one above cursor
				{
					swap(nums[cursor - 3 * 0], nums[cursor - 3 * 1]);
				}
				else if (nums[cursor - 3 * 2] == 8) // gap is two above cursor
				{
					swap(nums[cursor - 3 * 1], nums[cursor - 3 * 0]);
					swap(nums[cursor - 3 * 0], nums[cursor - 3 * 2]);
				}
			}
			else if (cursor == 7)
			{
				if (nums[cursor - 1] == 8) // gap one to left  of cursor
				{
					swap(nums[cursor - 0], nums[cursor - 1]);
				}
				else if (nums[cursor + 1] == 8) // gap one to right of cursor
				{
					swap(nums[cursor + 0], nums[cursor + 1]);
				}
				else if (nums[cursor - 3 * 1] == 8) // gap is one above cursor
				{
					swap(nums[cursor - 3 * 0], nums[cursor - 3 * 1]);
				}
				else if (nums[cursor - 3 * 2] == 8) // gap is two above cursor
				{
					swap(nums[cursor - 3 * 1], nums[cursor - 3 * 0]);
					swap(nums[cursor - 3 * 0], nums[cursor - 3 * 2]);
				}
			}
			else if (cursor == 8)
			{
				if (nums[cursor - 1] == 8) // gap one to left  of cursor
				{
					swap(nums[cursor - 0], nums[cursor - 1]);
				}
				else if (nums[cursor - 2] == 8) // gap two to left of cursor
				{
					swap(nums[cursor - 1], nums[cursor - 0]);
					swap(nums[cursor - 0], nums[cursor - 2]);
				}
				else if (nums[cursor - 3 * 1] == 8) // gap is one above cursor
				{
					swap(nums[cursor - 3 * 0], nums[cursor - 3 * 1]);
				}
				else if (nums[cursor - 3 * 2] == 8) // gap is two above cursor
				{
					swap(nums[cursor - 3 * 1], nums[cursor - 3 * 0]);
					swap(nums[cursor - 3 * 0], nums[cursor - 3 * 2]);
				}
			}
		}

		if (wnd.kbd.KeyIsPressed(VK_SPACE))
		{
			keyPressed = true;
			RandomiseTiles();
		}

		if (wnd.kbd.KeyIsPressed(VK_ESCAPE))
		{
			wnd.Kill();
		}		
	}
	else
	{
		if (!wnd.kbd.KeyIsPressed(VK_SPACE) && 
			!wnd.kbd.KeyIsPressed(VK_RETURN) &&
			!wnd.kbd.KeyIsPressed(VK_LEFT) &&
			!wnd.kbd.KeyIsPressed(VK_RIGHT) &&
			!wnd.kbd.KeyIsPressed(VK_UP) &&
			!wnd.kbd.KeyIsPressed(VK_DOWN))
		{
			keyPressed = false;
		}
	}

	if (nums[0] == 0 &&
		nums[1] == 1 &&
		nums[2] == 2 &&
		nums[3] == 3 &&
		nums[4] == 4 &&
		nums[5] == 5 &&
		nums[6] == 6 &&
		nums[7] == 7)
	{
		gameOver = true;
	}
	else gameOver = false;
}

void Game::Draw()
{
	DrawTiles();
	if (!gameOver) DrawCursor();	
}

void Game::RandomiseTiles()
{
	nums.clear();

	for (int i = 0; i < 9; i++)
	{
		int num = Math::Random(0, 8);

		for (int j = 0; j < nums.size(); j++)
		{
			while (num == nums[j])
			{
				num = Math::Random(0, 8);
				j = 0;
			}
		}
		nums.push_back(num);
	}
}

void Game::DrawImageSection(const int X, const int Y, const int I)
{
	const int COL = I % 3u;
	const int ROW = I / 3u;

	const int XSTART = 100 * COL;
	const int YSTART = 100 * ROW;

	const int XEND = XSTART + 100;
	const int YEND = YSTART + 100;

	for (int iy = YSTART; iy < YEND; iy++)
	{
		for (int ix = XSTART; ix < XEND; ix++)
		{
			gfx.DrawPixel(X + ix - XSTART, Y + iy - YSTART, image.GetPixel(ix, iy));
		}
	}
}

void Game::DrawTiles()
{
	if (!gameOver)
	{
		for (int iy = 0; iy < 3; iy++)
		{
			for (int ix = 0; ix < 3; ix++)
			{
				int i = iy * 3 + ix;
				if (nums[i] != 8)
				{
					DrawImageSection(100 + 100 * ix, 100 + 100 * iy, nums[i]);
				}
			}
		}
	}
	else
	{
		gfx.DrawImage(false, 100, 100, image);
	}
}

void Game::DrawCursor()
{
	int x = cursor % 3u;
	int y = cursor / 3u;

	gfx.DrawRectangle(
		false,
		100 + x * 100 + 10,
		100 + y * 100 + 10,
		200 + x * 100 - 10,
		200 + y * 100 - 10,
		Red);
}
