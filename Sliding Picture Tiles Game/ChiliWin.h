/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	ChiliWin.h																			  *
*	Copyright 2016 PlanetChili <http://www.planetchili.net>								  *
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

// target Windows 7 or later
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>
// The following #defines disable a bunch of unused windows stuff. If you 
// get weird errors when trying to do some windows stuff, try removing some
// (or all) of these defines (it will increase build time though).
#ifndef FULL_WINTARD
#define WIN32_LEAN_AND_MEAN
#define NOATOM
#define NOCOMM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODEFERWINDOWPOS
#define NODRAWTEXT
#define NOGDICAPMASKS
#define NOHELP
#define NOICONS
#define NOIMAGE
#define NOKANJI
#define NOKERNEL
#define NOMCX
#define NOMEMMGR
#define NOMENUS
#define NOMETAFILE
#define NOMINMAX
#define NONLS
#define NOOPENFILE
#define NOPROFILER
#define NOPROXYSTUB
#define NORASTEROPS
#define NORPC
#define NOSYSCOMMANDS
#define NOSYSMETRICS
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTAPE
#define NOTEXTMETRIC
#define NOWH
#define OEMRESOURCE
#endif

#define NOMINMAX

#define STRICT

#include <windowsx.h>
#include <Windows.h>