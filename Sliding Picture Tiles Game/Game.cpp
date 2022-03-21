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

#include <iostream>
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
	controlState = ControlState::Mouse; // default = mouse
	gameState = GameState::Setup; // default = Setup

	SetImageBackgrounds();
	SetImageButtons();
	SetImageDigits();
	SetImageSymbols();
	SetImageTiles();

	RandomiseTileImage();

	SetButtonPositions();
	SetDigitPositions();

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
	const int MOVES_MAX = rows * cols * 3 - 1;
	GetFrameTime();
	#else
	const int MOVES_MAX = /*rows * cols * */1;
	frameTime = 15.0f;
	#endif
	
	switch(gameState)
	{
		case GameState::Settings:
		{
			switch (controlState)
			{
			case ControlState::Mouse:

				const float MOUSE_X = (float)wnd.mouse.GetPosX();
				const float MOUSE_Y = (float)wnd.mouse.GetPosY();
				const float BUTTON_L = buttonPositions[2][0].x;
				const float BUTTON_R = buttonPositions[2][3].x;
				const float BUTTON_T = buttonPositions[2][0].y;
				const float BUTTON_B = buttonPositions[2][3].y;
				bool mouseOver = false;

				if (MOUSE_X >= BUTTON_L &&
					MOUSE_X <= BUTTON_R &&
					MOUSE_Y >= BUTTON_T &&
					MOUSE_Y <= BUTTON_B)
				{
					// mouse over settings button
					mouseOver = true;
				}
				else mouseOver = false;

				if (mouseOver)
				{
					if (!mousePressed)
					{
						if (wnd.mouse.LeftIsPressed())
						{
							mousePressed = true;
							controlState = ControlState::Keyboard;
							gameState = GameState::Play;
						}
					}
					else
					{
						if (!wnd.mouse.LeftIsPressed())
						{
							mousePressed = false;
							// WARNING: this may cause issues
							// as there is a seperate function that also does
							// this check. it might be ok though
						}
					}
				}
				break;
			}
			break;
		}
		case GameState::Setup:
		{
			if (!isMoving)
			{
				const int COLS = cols;
				const std::array<int, 4> DIRECTION = { -1,1,-COLS,COLS };

				std::vector<int> numbers = { 0,1,2,3 };
				assert(numbers.size() == 4);

				if (prevDir == 1)		numbers.erase(numbers.begin() + 0);
				if (prevDir == -1)		numbers.erase(numbers.begin() + 1);
				if (prevDir == COLS)	numbers.erase(numbers.begin() + 2);
				if (prevDir == -COLS)	numbers.erase(numbers.begin() + 3);

				int randomNum = Random(0, (int)numbers.size() - 1);

				const int GAP = tiles.back().GetPos();

				while (!IsNextToGap(GAP + DIRECTION[numbers[randomNum]]))
				{
					numbers.erase(numbers.begin() + randomNum);
					randomNum = Random(0, (int)numbers.size() - 1);
				}

				prevDir = DIRECTION[numbers[randomNum]];
				const int NEXT_TO_GAP = GAP + DIRECTION[numbers[randomNum]];
				const int INDEX = GetTileIndex(NEXT_TO_GAP);

				tiles.back().SetPosition(NEXT_TO_GAP);
				tiles[INDEX].SetToMoving(GAP);
				isMoving = true;

				if (nMoves < MOVES_MAX) nMoves++;
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
					if (nMoves >= MOVES_MAX) isSetUp = true;

					if (nMoves >= MOVES_MAX) gameState = GameState::Play;
				}
			}

			break;
		}
		case GameState::Play:
		{
			switch (controlState)
			{
			case ControlState::Mouse:
			{
				const float MOUSE_X = (float)wnd.mouse.GetPosX();
				const float MOUSE_Y = (float)wnd.mouse.GetPosY();

				mouseOver.clear();
				mouseOver.assign(10, false);

				{
					const float L = buttonPositions[BUTTON::INCREASE_ROWS][0].x;
					const float R = buttonPositions[BUTTON::INCREASE_ROWS][3].x;
					const float T = buttonPositions[BUTTON::INCREASE_ROWS][0].y;
					const float B = buttonPositions[BUTTON::INCREASE_ROWS][3].y;

					if (MOUSE_X >= L &&
						MOUSE_X <= R &&
						MOUSE_Y >= T &&
						MOUSE_Y <= B)
					{
						mouseOver[BUTTON::INCREASE_ROWS] = true;
					}
				}				

				{
					const float L = buttonPositions[BUTTON::DECREASE_ROWS][0].x;
					const float R = buttonPositions[BUTTON::DECREASE_ROWS][3].x;
					const float T = buttonPositions[BUTTON::DECREASE_ROWS][0].y;
					const float B = buttonPositions[BUTTON::DECREASE_ROWS][3].y;

					if (MOUSE_X >= L &&
						MOUSE_X <= R &&
						MOUSE_Y >= T &&
						MOUSE_Y <= B)
					{
						mouseOver[BUTTON::DECREASE_ROWS] = true;
					}
				}

				{
					const float L = buttonPositions[BUTTON::NEW_GAME][0].x;
					const float R = buttonPositions[BUTTON::NEW_GAME][3].x;
					const float T = buttonPositions[BUTTON::NEW_GAME][0].y;
					const float B = buttonPositions[BUTTON::NEW_GAME][3].y;

					if (MOUSE_X >= L &&
						MOUSE_X <= R &&
						MOUSE_Y >= T &&
						MOUSE_Y <= B)
					{
						mouseOver[BUTTON::NEW_GAME] = true;
					}
				}

				{
					const float L = buttonPositions[BUTTON::INCREASE_COLS][0].x;
					const float R = buttonPositions[BUTTON::INCREASE_COLS][3].x;
					const float T = buttonPositions[BUTTON::INCREASE_COLS][0].y;
					const float B = buttonPositions[BUTTON::INCREASE_COLS][3].y;

					if (MOUSE_X >= L &&
						MOUSE_X <= R &&
						MOUSE_Y >= T &&
						MOUSE_Y <= B)
					{
						mouseOver[BUTTON::INCREASE_COLS] = true;
					}
				}

				{
					const float L = buttonPositions[BUTTON::DECREASE_COLS][0].x;
					const float R = buttonPositions[BUTTON::DECREASE_COLS][3].x;
					const float T = buttonPositions[BUTTON::DECREASE_COLS][0].y;
					const float B = buttonPositions[BUTTON::DECREASE_COLS][3].y;

					if (MOUSE_X >= L &&
						MOUSE_X <= R &&
						MOUSE_Y >= T &&
						MOUSE_Y <= B)
					{
						mouseOver[BUTTON::DECREASE_COLS] = true;
					}
				}

				bool mouseOverTiles = false;
				if (MOUSE_X >= tilesRect.left &&
					MOUSE_X <= tilesRect.right &&
					MOUSE_Y >= tilesRect.top &&
					MOUSE_Y <= tilesRect.bottom)
				{
					mouseOverTiles = true;
				}

				if(mouseOverTiles)
				{
					int l = 0; // left
					int t = 0; // top
					int r = 0; // right
					int b = 0; // bottom

					for (const Tile& T : tiles)
					{
						l = (int)T.GetPosition()[0].x;
						t = (int)T.GetPosition()[0].y;
						r = (int)T.GetPosition()[3].x;
						b = (int)T.GetPosition()[3].y;

						if (MOUSE_X >= l &&
							MOUSE_X <= r &&
							MOUSE_Y >= t &&
							MOUSE_Y <= b)
						{
							cursor = T.GetPos();
						}
					}
				}

				if (!mousePressed)
				{
					if (wnd.mouse.LeftIsPressed())
					{
						mousePressed = true;

						if (mouseOver[BUTTON::INCREASE_ROWS])
						{
							if (nextRows < 9)
							{
								nextRows++;
							}
						}
						else if (mouseOver[BUTTON::DECREASE_ROWS])
						{
							if (nextRows > 2)
							{
								nextRows--;
							}
						}
						else if (mouseOver[BUTTON::INCREASE_COLS])
						{
							if (nextCols < 9)
							{
								nextCols++;
							}
						}
						else if (mouseOver[BUTTON::DECREASE_COLS])
						{
							if (nextCols > 2)
							{
								nextCols--;
							}
						}
						else if (mouseOver[BUTTON::NEW_GAME])
						{
							NewGame();
						}
						else if (mouseOverTiles)
						{
							const int CUR_INDEX = GetTileIndex(cursor);

							const int GAP = tiles.back().GetPos();

							if (IsNextToGap(cursor))
							{
								tiles.back().SetPosition(cursor);
								tiles[CUR_INDEX].SetToMoving(GAP);
								PlaySound(L"Sounds\\click3.wav", NULL, SND_ASYNC);
								isMoving = true;
							}
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

				if (gameState == GameState::Play)
				{
					MoveTile();

					if (!isMoving) CheckForGameOver();
				}

				break;
			}
			case ControlState::Keyboard:
			{
				if (wnd.kbd.KeyIsPressed(VK_ESCAPE))
				{
					wnd.Kill();
				}

				if (!returnPressed)
				{
					if (wnd.kbd.KeyIsPressed(VK_RETURN))
					{
						returnPressed = true;

						NewGame();
					}
					else
					{
						MoveTile();

						if (!isMoving) CheckForGameOver();
					}
				}
				else
				{
					if (!wnd.kbd.KeyIsPressed(VK_RETURN))
					{
						returnPressed = false;
					}
				}

				if (!arrowPressed)
				{
					/**/ if (wnd.kbd.KeyIsPressed(VK_LEFT))
					{
						arrowPressed = true;
						int count = 0;
						for (unsigned int i = 0; i < cols; i++)
						{
							if (cursor != i * cols)
							{
								count++;
							}
						}
						if (count == cols) cursor--;
					}
					else if (wnd.kbd.KeyIsPressed(VK_RIGHT))
					{
						arrowPressed = true;
						int count = 0;
						for (unsigned int i = 1; i <= cols; i++)
						{
							int j = i * cols - 1;
							if (cursor != j)
							{
								count++;
							}
						}
						if (count == cols) cursor++;
					}
					else if (wnd.kbd.KeyIsPressed(VK_UP))
					{
						arrowPressed = true;
						if (cursor > cols - 1)
						{
							cursor -= cols;
						}
					}
					else if (wnd.kbd.KeyIsPressed(VK_DOWN))
					{
						arrowPressed = true;
						if (cursor < (rows - 1) * cols)
						{
							cursor += cols;
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
							PlaySound(L"Sounds\\click3.wav", NULL, SND_ASYNC);
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

				break;
			}
			case ControlState::Controller:
			{
				// placeholder
				break;
			}
			}		

			break;
		}
	}
}

void Game::Draw()
{
	switch (gameState)
	{
		case GameState::Settings:
			DrawBackground();
			gfx.DrawRectangle(true, 25, 25, 675, 475, { 100,255,255,255 });
			DrawButtonMouse();
			DrawButtonKeyboard();
			DrawButtonController();
			break;

		case GameState::Setup:
			DrawBackground();
			DrawTiles();
			DrawTileBorders();
			break;
		
		case GameState::Play:
			DrawBackground();
			DrawTiles();
			if(!gameOver) DrawCursor();
			if(!gameOver) DrawTileBorders();
			//DrawButtonSettings();

			DrawButtonIncreaseRows();
			DrawDigit(digitPositions[0], nextRows);
			DrawButtonDecreaseRows();

			DrawButtonIncreaseCols();
			DrawDigit(digitPositions[1], nextCols);
			DrawButtonDecreaseCols();

			DrawButtonNewGame();

			DrawSymbolRows();
			DrawSymbolCols();

			break;
	}
}

//---------------------------------------------------

void Game::ReadDirectory(const std::wstring& FILENAME, std::vector<std::wstring>& v)
{
	// REQUIRES: Windows.h, vector.h, string.h
	
	std::wstring pattern(FILENAME);
	pattern.append(L"\\*");
	WIN32_FIND_DATA data;
	HANDLE hFind;
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			v.push_back(std::wstring(data.cFileName));
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}

	/*
	EXAMPLE: LOAD IMAGES INTO MEMORY
	const wstring FOLDER_NAME = L"DriveLetter:\\FolderName";
	std::vector<wstring> v;
	ReadDirectory(FOLDER_NAME, v);

	for (size_t i = 2; i < v.size(); i++)
	{
		tileImages.push_back(Surface::FromFile(L"Images\\Tiles\\" + v[i]));
	}

	NOTES:
	i = 2 because the first two items are not files
	*/
}

void Game::SetImageButtons()
{
	buttonImages.push_back(Surface::FromFile(L"Images\\Buttons\\Settings.png"));
	buttonImages.push_back(Surface::FromFile(L"Images\\Buttons\\Mouse.png"));
	buttonImages.push_back(Surface::FromFile(L"Images\\Buttons\\Keyboard.png"));
	buttonImages.push_back(Surface::FromFile(L"Images\\Buttons\\Controller.png"));
	buttonImages.push_back(Surface::FromFile(L"Images\\Buttons\\Triangle.png"));
	buttonImages.push_back(Surface::FromFile(L"Images\\Buttons\\NewGame.png"));
}
void Game::SetImageTiles()
{
	const wstring FOLDER_NAME = L"C:\\Users\\Bumble\\source\\repos\\Sliding Picture Tiles Game\\Sliding Picture Tiles Game\\Images\\Tiles";
	std::vector<wstring> v;
	ReadDirectory(FOLDER_NAME, v);
	assert(v.size() > 2);

	for (size_t i = 2; i < v.size(); i++)
	{
		tileImages.push_back(Surface::FromFile(L"Images\\Tiles\\" + v[i]));
	}

	/*tileImages.push_back(Surface::FromFile(L"Images\\Tiles\\mountain.jpg"));
	tileImages.push_back(Surface::FromFile(L"Images\\Tiles\\earth.jpg"));
	tileImages.push_back(Surface::FromFile(L"Images\\Tiles\\squirrel.jpg"));
	tileImages.push_back(Surface::FromFile(L"Images\\Tiles\\pretty.jpg"));*/
}
void Game::SetImageDigits()
{
	digitImages.push_back(Surface::FromFile(L"Images\\Digits\\digit_0.png"));
	digitImages.push_back(Surface::FromFile(L"Images\\Digits\\digit_1.png"));
	digitImages.push_back(Surface::FromFile(L"Images\\Digits\\digit_2.png"));
	digitImages.push_back(Surface::FromFile(L"Images\\Digits\\digit_3.png"));
	digitImages.push_back(Surface::FromFile(L"Images\\Digits\\digit_4.png"));
	digitImages.push_back(Surface::FromFile(L"Images\\Digits\\digit_5.png"));
	digitImages.push_back(Surface::FromFile(L"Images\\Digits\\digit_6.png"));
	digitImages.push_back(Surface::FromFile(L"Images\\Digits\\digit_7.png"));
	digitImages.push_back(Surface::FromFile(L"Images\\Digits\\digit_8.png"));
	digitImages.push_back(Surface::FromFile(L"Images\\Digits\\digit_9.png"));
}
void Game::SetImageSymbols()
{
	symbolImages.push_back(Surface::FromFile(L"Images\\Symbols\\Rows.png"));
	symbolImages.push_back(Surface::FromFile(L"Images\\Symbols\\Cols.png"));
}
void Game::SetImageBackgrounds()
{
	backgroundImages.push_back(Surface::FromFile(L"Images\\Backgrounds\\woven.jpg"));
	backgroundImages.push_back(Surface::FromFile(L"Images\\Backgrounds\\swirl.jpg"));
	backgroundImages.push_back(Surface::FromFile(L"Images\\Backgrounds\\wooden_boards.jpg"));
	backgroundImages.push_back(Surface::FromFile(L"Images\\Backgrounds\\shell.jpg"));
	backgroundImages.push_back(Surface::FromFile(L"Images\\Backgrounds\\pebble.jpg"));
	backgroundImages.push_back(Surface::FromFile(L"Images\\Backgrounds\\floral_red.jpg"));
}

void Game::SetButtonPositions()
{
	// Settings Button
	{
		const float L = 550.0f;
		const float R = L + 100.0f;
		const float T = 25.0f;
		const float B = T + 100.0f;

		const Vector LT = { L,T };
		const Vector RT = { R,T };
		const Vector LB = { L,B };
		const Vector RB = { R,B };

		buttonPositions.push_back({ LT, RT ,LB ,RB });
	}

	// Mouse Button
	{
		const float L = 350.0f;
		const float R = L + 100.0f;
		const float T = 100.0f;
		const float B = T + 100.0f;

		const Vector LT = { L,T };
		const Vector RT = { R,T };
		const Vector LB = { L,B };
		const Vector RB = { R,B };

		buttonPositions.push_back({ LT, RT ,LB ,RB });
	}

	// Keyboard Button
	{
		const float L = 50.0f;
		const float R = L + 200.0f;
		const float T = 100.0f;
		const float B = T + 100.0f;

		const Vector LT = { L,T };
		const Vector RT = { R,T };
		const Vector LB = { L,B };
		const Vector RB = { R,B };

		buttonPositions.push_back({ LT, RT ,LB ,RB });
	}

	// Controller Button
	{
		const float L = 500.0f;
		const float R = L + 150.0f;
		const float T = 100.0f;
		const float B = T + 100.0f;

		const Vector LT = { L,T };
		const Vector RT = { R,T };
		const Vector LB = { L,B };
		const Vector RB = { R,B };

		buttonPositions.push_back({ LT, RT ,LB ,RB });
	}

	// Increase Rows Button
	{
		const float R = Graphics::WINDOW_RIGHT - 25.0f - 50.0f;
		const float L = R - 50.0f;
		const float T = 200.0f;
		const float B = T + 50.0f;

		const Vector LT = { L,T };
		const Vector RT = { R,T };
		const Vector LB = { L,B };
		const Vector RB = { R,B };

		buttonPositions.push_back({ LT, RT ,LB ,RB });
	}

	// Decrease Rows Button
	{
		const float R = Graphics::WINDOW_RIGHT - 25.0f - 50.0f;
		const float L = R - 50.0f;
		const float T = 300.0f;
		const float B = T + 50.0f;

		const Vector LT = { L,T };
		const Vector RT = { R,T };
		const Vector LB = { L,B };
		const Vector RB = { R,B };

		buttonPositions.push_back({ LT, RT ,LB ,RB });
	}

	// NewGame Button
	{
		const float R = Graphics::WINDOW_RIGHT - 25.0f;
		const float L = R - 100.0f;
		const float B = Graphics::WINDOW_BOTTOM - 25.0f;
		const float T = B - 100.0f;

		const Vector LT = { L,T };
		const Vector RT = { R,T };
		const Vector LB = { L,B };
		const Vector RB = { R,B };

		buttonPositions.push_back({ LT, RT ,LB ,RB });
	}

	// Increase Cols Button
	{
		const float R = Graphics::WINDOW_RIGHT - 25.0f;
		const float L = R - 50.0f;
		const float T = 200.0f;
		const float B = T + 50.0f;

		const Vector LT = { L,T };
		const Vector RT = { R,T };
		const Vector LB = { L,B };
		const Vector RB = { R,B };

		buttonPositions.push_back({ LT, RT ,LB ,RB });
	}

	// Decrease Cols Button
	{
		const float R = Graphics::WINDOW_RIGHT - 25.0f;
		const float L = R - 50.0f;
		const float T = 300.0f;
		const float B = T + 50.0f;

		const Vector LT = { L,T };
		const Vector RT = { R,T };
		const Vector LB = { L,B };
		const Vector RB = { R,B };

		buttonPositions.push_back({ LT, RT ,LB ,RB });
	}
}
void Game::SetTileTextureCoordinates()
{
	tileTexCoords.clear();

	const float XFRACTION = 1.0f / cols;
	const float YFRACTION = 1.0f / rows;
	const int SIZE = rows * cols;

	Vector topLeft;
	Vector topRight;
	Vector bottomLeft;
	Vector bottomRight;

	float left		= 0;
	float top		= 0;
	float right		= 0;
	float bottom	= 0;

	for (int i = 0; i < SIZE; i++)
	{
		left	= XFRACTION * float(i % cols);
		top		= YFRACTION * float(i / cols);
		right	= left + XFRACTION;
		bottom	= top + YFRACTION;

		topLeft = { left, top };
		topRight = { right, top };
		bottomLeft = { left, bottom };
		bottomRight = { right, bottom };

		tileTexCoords.push_back({
			topLeft,
			topRight,
			bottomLeft,
			bottomRight });
	}

	assert(tileTexCoords.size() == SIZE);
}
void Game::SetTilePositions()
{
	tilePositions.clear();

	const int OFFSET = 25;
	const int TILE_WIDTH = (Graphics::WINDOW_WIDTH - (OFFSET * 2) - 200) / cols;
	const int TILE_HEIGHT = (Graphics::WINDOW_HEIGHT - (OFFSET * 2)) / rows;
	const int SIZE = rows * cols;

	Vector topLeft;
	Vector topRight;
	Vector bottomLeft;
	Vector bottomRight;

	float left = 0;
	float top = 0;
	float right = 0;
	float bottom = 0;

	for (int i = 0; i < SIZE; i++)
	{
		left = float(TILE_WIDTH * (i % cols)) + OFFSET;
		top = float(TILE_HEIGHT * (i / cols)) + OFFSET;
		right = left + TILE_WIDTH;
		bottom = top + TILE_HEIGHT;

		topLeft = {	left, top };
		topRight = { right, top }; 
		bottomLeft = { left, bottom };
		bottomRight = {	right, bottom };

		tilePositions.push_back({
			topLeft,
			topRight,
			bottomLeft,
			bottomRight });
	}

	assert(tilePositions.size() == SIZE);
}
void Game::SetDigitPositions()
{	
	{
		// Digit Rows

		const float R = Graphics::WINDOW_RIGHT - 25.0f - 50.0f;
		const float L = R - 50.0f;
		const float T = 250.0f;
		const float B = T + 50.0f;

		const Vector LT = { L,T };
		const Vector RT = { R,T };
		const Vector LB = { L,B };
		const Vector RB = { R,B };

		digitPositions.push_back({ LT, RT ,LB ,RB });
	}

	{
		// Digit Cols

		const float R = Graphics::WINDOW_RIGHT - 25.0f;
		const float L = R - 50.0f;
		const float T = 250.0f;
		const float B = T + 50.0f;

		const Vector LT = { L,T };
		const Vector RT = { R,T };
		const Vector LB = { L,B };
		const Vector RB = { R,B };

		digitPositions.push_back({ LT, RT ,LB ,RB });
	}
}

void Game::SetTiles()
{
	SetTileTextureCoordinates();
	SetTilePositions();

	tiles.clear();
	assert(tiles.empty() == true);

	const int SIZE = rows * cols;
	const float	TILE_SPEED = 150.0f;

	for (int i = 0; i < SIZE; i++)
	{
		tiles.push_back({
			TILE_SPEED,
			i,
			tilePositions,
			i,
			tileTexCoords});
	}

	assert(tiles.size() == SIZE);

	SetTilesRect();
}

void Game::SetTilesRect()
{
	assert(tilePositions.size() >= 2);
	const int L = (int)tilePositions.front()[0].x;
	const int T = (int)tilePositions.front()[0].y;
	const int R = (int)tilePositions.back()[3].x;
	const int B = (int)tilePositions.back()[3].y;
	tilesRect = { L,T,R,B };
}

void Game::NextTileImage()
{
	imageNum++;
	if (imageNum >= tileImages.size()) imageNum = 0;
}
void Game::RandomiseTileImage()
{
	if (!tileImages.empty())
	{
		imageNum = Math::Random(0, (int)tileImages.size() - 1);
	}
}

int  Game::GetTileIndex(const int& POS)
{
	for (int i = 0; i < tiles.size(); i++)
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

	const int GAP_COL = GAP % cols;
	const int POS_COL = POS % cols;
	
	if (GAP_COL == POS_COL && (POS == GAP + cols || POS == GAP - cols)) return true;
	
	const int GAP_ROW = GAP / cols;
	const int POS_ROW = POS / cols;
	
	if (GAP_ROW == POS_ROW && (POS == GAP + 1 || POS == GAP - 1)) return true;
		
	return false;
}

void Game::GetFrameTime()
{
	timeNew = timer.GetTimeMilli();
	frameTime = timeNew - timeOld;
	timeOld = timeNew;
}

void Game::MoveTile()
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
		isMoving = false;
	}
}

void Game::RestartGame()
{
	gameOver = false;
	isSetUp = false;
	nMoves = 0;
	SetTiles();
	gameState = GameState::Setup;
}
void Game::NewGame()
{
	rows = nextRows;
	cols = nextCols;
	gameOver = false;
	isSetUp = false;
	nMoves = 0;
	NextTileImage();
	SetTiles();
	gameState = GameState::Setup;
}

void Game::CheckForGameOver()
{
	if (!gameOver)
	{
		int count = 0;
		for (int i = 0; i < tiles.size(); i++)
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

		if (count == tiles.size()) gameOver = true;

		if (gameOver) soundPlayed = PlaySound(L"Sounds\\confirmation.wav", NULL, SND_ASYNC);
	}
}

void Game::DrawBackground()
{
	Math::TextureVertex lt; // left  top
	Math::TextureVertex rt; // right top
	Math::TextureVertex lb; // left  bottom
	Math::TextureVertex rb; // right bottom

	// Whole Window: ROWS = 1, COLS = 1
	const unsigned int ROWS = 2;
	const unsigned int COLS = 3;
	const float H = (float)Graphics::WINDOW_HEIGHT / ROWS; // tile width
	const float W = (float)Graphics::WINDOW_WIDTH / COLS;  // tile height

	// texture coordinates
	const Vector LT = { 0.0f,0.0f };
	const Vector RT = { 1.0f,0.0f };
	const Vector LB = { 0.0f,1.0f };
	const Vector RB = { 1.0f,1.0f };

	// tile coordinates
	float l = 0;
	float t = 0;
	float r = 0;
	float b = 0;

	for (unsigned int row = 0; row < ROWS; row++)
	{
		for (unsigned int col = 0; col < COLS; col++)
		{
			l = col	* W;
			t = row	* H;
			r = l	+ W;
			b = t	+ H;

			lt = { {l,t}, LT };
			rt = { {r,t}, RT };
			lb = { {l,b}, LB };
			rb = { {r,b}, RB };

			const int NUM = 2; // this needs to be a variable
			gfx.DrawTriangleTex(lt, rt, lb, backgroundImages[NUM]);
			gfx.DrawTriangleTex(rt, rb, lb, backgroundImages[NUM]);
		}
	}
}

void Game::DrawButtonSettings()
{
	TextureVertex tv0{ buttonPositions[0][0], { 0.0f,0.0f } };
	TextureVertex tv1{ buttonPositions[0][1], { 1.0f,0.0f } };
	TextureVertex tv2{ buttonPositions[0][2], { 0.0f,1.0f } };
	TextureVertex tv3{ buttonPositions[0][3], { 1.0f,1.0f } };

	gfx.DrawTriangleTex(tv0, tv1, tv2, buttonImages[0]);
	gfx.DrawTriangleTex(tv1, tv3, tv2, buttonImages[0]);

	gfx.DrawRectangle(false, tv0.pos, tv3.pos, Red);
}
void Game::DrawButtonMouse()
{
	TextureVertex tv0{ buttonPositions[1][0], { 0.0f,0.0f } };
	TextureVertex tv1{ buttonPositions[1][1], { 1.0f,0.0f } };
	TextureVertex tv2{ buttonPositions[1][2], { 0.0f,1.0f } };
	TextureVertex tv3{ buttonPositions[1][3], { 1.0f,1.0f } };

	gfx.DrawTriangleTex(tv0, tv1, tv2, buttonImages[1]);
	gfx.DrawTriangleTex(tv1, tv3, tv2, buttonImages[1]);

	gfx.DrawRectangle(false, tv0.pos, tv3.pos, Red);
}
void Game::DrawButtonKeyboard()
{
	TextureVertex tv0{ buttonPositions[2][0], { 0.0f,0.0f } };
	TextureVertex tv1{ buttonPositions[2][1], { 1.0f,0.0f } };
	TextureVertex tv2{ buttonPositions[2][2], { 0.0f,1.0f } };
	TextureVertex tv3{ buttonPositions[2][3], { 1.0f,1.0f } };

	gfx.DrawTriangleTex(tv0, tv1, tv2, buttonImages[2]);
	gfx.DrawTriangleTex(tv1, tv3, tv2, buttonImages[2]);

	gfx.DrawRectangle(false, tv0.pos, tv3.pos, Red);
}
void Game::DrawButtonController()
{
	TextureVertex tv0{ buttonPositions[3][0], { 0.0f,0.0f } };
	TextureVertex tv1{ buttonPositions[3][1], { 1.0f,0.0f } };
	TextureVertex tv2{ buttonPositions[3][2], { 0.0f,1.0f } };
	TextureVertex tv3{ buttonPositions[3][3], { 1.0f,1.0f } };

	gfx.DrawTriangleTex(tv0, tv1, tv2, buttonImages[3]);
	gfx.DrawTriangleTex(tv1, tv3, tv2, buttonImages[3]);

	gfx.DrawRectangle(false, tv0.pos, tv3.pos, Red);
}
void Game::DrawButtonIncreaseRows()
{
	TextureVertex tv0{ buttonPositions[BUTTON::INCREASE_ROWS][0], { 0.0f,0.0f } };
	TextureVertex tv1{ buttonPositions[BUTTON::INCREASE_ROWS][1], { 1.0f,0.0f } };
	TextureVertex tv2{ buttonPositions[BUTTON::INCREASE_ROWS][2], { 0.0f,1.0f } };
	TextureVertex tv3{ buttonPositions[BUTTON::INCREASE_ROWS][3], { 1.0f,1.0f } };

	gfx.DrawTriangleTex(tv0, tv1, tv2, buttonImages[IMAGE::TRIANGLE]);
	gfx.DrawTriangleTex(tv1, tv3, tv2, buttonImages[IMAGE::TRIANGLE]);

	if(isTesting) gfx.DrawRectangle(false, tv0.pos, tv3.pos, Red);
}
void Game::DrawButtonDecreaseRows()
{
	TextureVertex tv0{ buttonPositions[BUTTON::DECREASE_ROWS][0], { 0.0f,1.0f } };
	TextureVertex tv1{ buttonPositions[BUTTON::DECREASE_ROWS][1], { 1.0f,1.0f } };
	TextureVertex tv2{ buttonPositions[BUTTON::DECREASE_ROWS][2], { 0.0f,0.0f } };
	TextureVertex tv3{ buttonPositions[BUTTON::DECREASE_ROWS][3], { 1.0f,0.0f } };

	gfx.DrawTriangleTex(tv0, tv1, tv2, buttonImages[IMAGE::TRIANGLE]);
	gfx.DrawTriangleTex(tv1, tv3, tv2, buttonImages[IMAGE::TRIANGLE]);

	if(isTesting) gfx.DrawRectangle(false, tv0.pos, tv3.pos, Red);
}
void Game::DrawButtonNewGame()
{
	TextureVertex tv0{ buttonPositions[BUTTON::NEW_GAME][0], { 0.0f,0.0f } };
	TextureVertex tv1{ buttonPositions[BUTTON::NEW_GAME][1], { 1.0f,0.0f } };
	TextureVertex tv2{ buttonPositions[BUTTON::NEW_GAME][2], { 0.0f,1.0f } };
	TextureVertex tv3{ buttonPositions[BUTTON::NEW_GAME][3], { 1.0f,1.0f } };

	gfx.DrawTriangleTex(tv0, tv1, tv2, buttonImages[IMAGE::NEW_GAME]);
	gfx.DrawTriangleTex(tv1, tv3, tv2, buttonImages[IMAGE::NEW_GAME]);

	if(isTesting) gfx.DrawRectangle(false, tv0.pos, tv3.pos, Red);
}
void Game::DrawButtonIncreaseCols()
{
	TextureVertex tv0{ buttonPositions[BUTTON::INCREASE_COLS][0], { 0.0f,0.0f } };
	TextureVertex tv1{ buttonPositions[BUTTON::INCREASE_COLS][1], { 1.0f,0.0f } };
	TextureVertex tv2{ buttonPositions[BUTTON::INCREASE_COLS][2], { 0.0f,1.0f } };
	TextureVertex tv3{ buttonPositions[BUTTON::INCREASE_COLS][3], { 1.0f,1.0f } };

	gfx.DrawTriangleTex(tv0, tv1, tv2, buttonImages[IMAGE::TRIANGLE]);
	gfx.DrawTriangleTex(tv1, tv3, tv2, buttonImages[IMAGE::TRIANGLE]);

	if (isTesting) gfx.DrawRectangle(false, tv0.pos, tv3.pos, Red);
}
void Game::DrawButtonDecreaseCols()
{
	TextureVertex tv0{ buttonPositions[BUTTON::DECREASE_COLS][0], { 0.0f,1.0f } };
	TextureVertex tv1{ buttonPositions[BUTTON::DECREASE_COLS][1], { 1.0f,1.0f } };
	TextureVertex tv2{ buttonPositions[BUTTON::DECREASE_COLS][2], { 0.0f,0.0f } };
	TextureVertex tv3{ buttonPositions[BUTTON::DECREASE_COLS][3], { 1.0f,0.0f } };

	gfx.DrawTriangleTex(tv0, tv1, tv2, buttonImages[IMAGE::TRIANGLE]);
	gfx.DrawTriangleTex(tv1, tv3, tv2, buttonImages[IMAGE::TRIANGLE]);

	if (isTesting) gfx.DrawRectangle(false, tv0.pos, tv3.pos, Red);
}

void Game::DrawDigit(const std::array<Math::Vector, 4>& POSITION, const int& number)
{
	TextureVertex tv0{ POSITION[0], { 0.0f,0.0f } };
	TextureVertex tv1{ POSITION[1], { 1.0f,0.0f } };
	TextureVertex tv2{ POSITION[2], { 0.0f,1.0f } };
	TextureVertex tv3{ POSITION[3], { 1.0f,1.0f } };

	gfx.DrawTriangleTex(tv0, tv1, tv2, digitImages[number]);
	gfx.DrawTriangleTex(tv1, tv3, tv2, digitImages[number]);

	if (isTesting) gfx.DrawRectangle(false, tv0.pos, tv3.pos, Red);
}

void Game::DrawSymbolRows()
{
	const float R = Graphics::WINDOW_RIGHT - 25.0f - 50.0f;
	const float L = R - 50.0f;
	const float T = 150.0f;
	const float B = T + 50.0f;

	const TextureVertex LT{ {L,T}, { 0.0f,0.0f } };
	const TextureVertex RT{ {R,T}, { 1.0f,0.0f } };
	const TextureVertex LB{ {L,B}, { 0.0f,1.0f } };
	const TextureVertex RB{ {R,B}, { 1.0f,1.0f } };

	gfx.DrawTriangleTex(LT, RT, RB, symbolImages[0]);
	gfx.DrawTriangleTex(LT, RB, LB, symbolImages[0]);
	 
	if (isTesting) gfx.DrawRectangle(false, LT.pos, RB.pos, Red);
}
void Game::DrawSymbolCols()
{
	const float R = Graphics::WINDOW_RIGHT - 25.0f;
	const float L = R - 50.0f;
	const float T = 150.0f;
	const float B = T + 50.0f;

	const TextureVertex LT{ {L,T}, { 0.0f,0.0f } };
	const TextureVertex RT{ {R,T}, { 1.0f,0.0f } };
	const TextureVertex LB{ {L,B}, { 0.0f,1.0f } };
	const TextureVertex RB{ {R,B}, { 1.0f,1.0f } };

	gfx.DrawTriangleTex(LT, RT, RB, symbolImages[1]);
	gfx.DrawTriangleTex(LT, RB, LB, symbolImages[1]);

	if (isTesting) gfx.DrawRectangle(false, LT.pos, RB.pos, Red);
}

void Game::DrawTiles()
{
	for( const Tile& T : tiles )
	{
		if (T.GetTex() != tiles.size() - 1 || gameOver)
		{
			gfx.DrawTile(T, tileImages[imageNum]);
		}
	}
}
void Game::DrawCursor()
{
	const int CURSOR_OFFSET = 10;

	gfx.DrawRectangle(
		false,
		(int)tilePositions[cursor][0].x + CURSOR_OFFSET,
		(int)tilePositions[cursor][0].y + CURSOR_OFFSET,
		(int)tilePositions[cursor][3].x - CURSOR_OFFSET,
		(int)tilePositions[cursor][3].y - CURSOR_OFFSET,
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


