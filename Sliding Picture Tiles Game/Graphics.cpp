/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.cpp																		  *
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
#include "Graphics.h"
#include "DXErr.h"
#include "ChiliException.h"

#include <assert.h>
#include <string>
#include <array>

// Ignore the intellisense error "cannot open source file" for .shh files.
// They will be created during the build sequence before the preprocessor runs.
namespace FramebufferShaders
{
#include "FramebufferPS.shh"
#include "FramebufferVS.shh"
}

#pragma comment( lib,"d3d11.lib" )

#define CHILI_GFX_EXCEPTION( hr,note ) Graphics::Exception( hr,note,_CRT_WIDE(__FILE__),__LINE__ )

using Microsoft::WRL::ComPtr;
using namespace Math;
using namespace std;

Graphics::Graphics( HWNDKey& key )
{
	assert( key.hWnd != nullptr );

	//////////////////////////////////////////////////////
	// create device and swap chain/get render target view
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Graphics::WINDOW_WIDTH;
	sd.BufferDesc.Height = Graphics::WINDOW_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 1;
	sd.BufferDesc.RefreshRate.Denominator = 60;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = key.hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	HRESULT				hr;
	UINT				createFlags = 0u;
#ifdef CHILI_USE_D3D_DEBUG_LAYER
#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#endif
	
	// create device and front/back buffers
	if( FAILED( hr = D3D11CreateDeviceAndSwapChain( 
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pImmediateContext ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating device and swap chain" );
	}

	// get handle to backbuffer
	ComPtr<ID3D11Resource> pBackBuffer;
	if( FAILED( hr = pSwapChain->GetBuffer(
		0,
		__uuidof( ID3D11Texture2D ),
		(LPVOID*)&pBackBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Getting back buffer" );
	}

	// create a view on backbuffer that we can render to
	if( FAILED( hr = pDevice->CreateRenderTargetView( 
		pBackBuffer.Get(),
		nullptr,
		&pRenderTargetView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating render target view on backbuffer" );
	}


	// set backbuffer as the render target using created view
	pImmediateContext->OMSetRenderTargets( 1,pRenderTargetView.GetAddressOf(),nullptr );


	// set viewport dimensions
	D3D11_VIEWPORT vp;
	vp.Width = float( Graphics::WINDOW_WIDTH );
	vp.Height = float( Graphics::WINDOW_HEIGHT );
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pImmediateContext->RSSetViewports( 1,&vp );


	///////////////////////////////////////
	// create texture for cpu render target
	D3D11_TEXTURE2D_DESC sysTexDesc;
	sysTexDesc.Width = Graphics::WINDOW_WIDTH;
	sysTexDesc.Height = Graphics::WINDOW_HEIGHT;
	sysTexDesc.MipLevels = 1;
	sysTexDesc.ArraySize = 1;
	sysTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sysTexDesc.SampleDesc.Count = 1;
	sysTexDesc.SampleDesc.Quality = 0;
	sysTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	sysTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sysTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sysTexDesc.MiscFlags = 0;
	// create the texture
	if( FAILED( hr = pDevice->CreateTexture2D( &sysTexDesc,nullptr,&pSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sysbuffer texture" );
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = sysTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	// create the resource view on the texture
	if( FAILED( hr = pDevice->CreateShaderResourceView( pSysBufferTexture.Get(),
		&srvDesc,&pSysBufferTextureView ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating view on sysBuffer texture" );
	}


	////////////////////////////////////////////////
	// create pixel shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreatePixelShader(
		FramebufferShaders::FramebufferPSBytecode,
		sizeof( FramebufferShaders::FramebufferPSBytecode ),
		nullptr,
		&pPixelShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating pixel shader" );
	}
	

	/////////////////////////////////////////////////
	// create vertex shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateVertexShader(
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		nullptr,
		&pVertexShader ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex shader" );
	}
	

	//////////////////////////////////////////////////////////////
	// create and fill vertex buffer with quad for rendering frame
	const FSQVertex vertices[] =
	{
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,1.0f,0.5f,1.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,1.0f,0.5f,0.0f,0.0f },
		{ 1.0f,-1.0f,0.5f,1.0f,1.0f },
		{ -1.0f,-1.0f,0.5f,0.0f,1.0f },
	};
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( FSQVertex ) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	if( FAILED( hr = pDevice->CreateBuffer( &bd,&initData,&pVertexBuffer ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating vertex buffer" );
	}

	
	//////////////////////////////////////////
	// create input layout for fullscreen quad
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	// Ignore the intellisense error "namespace has no member"
	if( FAILED( hr = pDevice->CreateInputLayout( ied,2,
		FramebufferShaders::FramebufferVSBytecode,
		sizeof( FramebufferShaders::FramebufferVSBytecode ),
		&pInputLayout ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating input layout" );
	}


	////////////////////////////////////////////////////
	// Create sampler state for fullscreen textured quad
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if( FAILED( hr = pDevice->CreateSamplerState( &sampDesc,&pSamplerState ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Creating sampler state" );
	}

	// allocate memory for sysbuffer (16-byte aligned for faster access)
	pSysBuffer = reinterpret_cast<Color*>( 
		_aligned_malloc( sizeof( Color ) * Graphics::WINDOW_WIDTH * Graphics::WINDOW_HEIGHT,16u ) );
}
Graphics::~Graphics()
{
	// free sysbuffer memory (aligned free)
	if( pSysBuffer )
	{
		_aligned_free( pSysBuffer );
		pSysBuffer = nullptr;
	}
	// clear the state of the device context before destruction
	if( pImmediateContext ) pImmediateContext->ClearState();
}

//void Graphics::DrawLine(float x1, float y1, float x2, float y2, const Color& COLOR)
//{
//	// Bresenham's line algorithm
//	const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
//	if (steep)
//	{
//		std::swap(x1, y1);
//		std::swap(x2, y2);
//	}
//
//	if (x1 > x2)
//	{
//		std::swap(x1, x2);
//		std::swap(y1, y2);
//	}
//
//	const float dx = x2 - x1;
//	const float dy = fabs(y2 - y1);
//
//	float error = dx / 2.0f;
//	const int ystep = (y1 < y2) ? 1 : -1;
//	int y = (int)y1;
//
//	const int maxX = (int)x2;
//
//	for (int x = (int)x1; x <= maxX; x++)
//	{
//		if (steep)
//		{
//			DrawPixel(y, x, COLOR);
//		}
//		else
//		{
//			DrawPixel(x, y, COLOR);
//		}
//
//		error -= dy;
//		if (error < 0)
//		{
//			y += ystep;
//			error += dx;
//		}
//	}
//}

// EXCEPTION
Graphics::Exception::Exception( HRESULT hr,const wstring& note,const wchar_t* file,unsigned int line )
	:
	ChiliException( file,line,note ),
	hr( hr )
{}
wstring Graphics::Exception::GetFullMessage() const
{
	const wstring empty = L"";
	const wstring errorName = GetErrorName();
	const wstring errorDesc = GetErrorDescription();
	const wstring& note = GetNote();
	const wstring location = GetLocation();
	return    (!errorName.empty() ? wstring( L"Error: " ) + errorName + L"\n"
		: empty)
		+ (!errorDesc.empty() ? wstring( L"Description: " ) + errorDesc + L"\n"
			: empty)
		+ (!note.empty() ? wstring( L"Note: " ) + note + L"\n"
			: empty)
		+ (!location.empty() ? wstring( L"Location: " ) + location
			: empty);
}
wstring Graphics::Exception::GetErrorName() const
{
	return DXGetErrorString( hr );
}
wstring Graphics::Exception::GetErrorDescription() const
{
	array<wchar_t,512> wideDescription;
	DXGetErrorDescription( hr,wideDescription.data(),wideDescription.size() );
	return wideDescription.data();
}
wstring Graphics::Exception::GetExceptionType() const
{
	return L"Chili Graphics Exception";
}

// FRAME FUNCTIONS
void Graphics::BeginFrame()
{
	// clear the sysbuffer
	memset( pSysBuffer, COLOR, sizeof( Color ) * Graphics::WINDOW_HEIGHT * Graphics::WINDOW_WIDTH );
}
void Graphics::EndFrame()
{
	HRESULT hr;

	// lock and map the adapter memory for copying over the sysbuffer
	if( FAILED( hr = pImmediateContext->Map( pSysBufferTexture.Get(),0u,
		D3D11_MAP_WRITE_DISCARD,0u,&mappedSysBufferTexture ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Mapping sysbuffer" );
	}

	// setup parameters for copy operation
	Color* pDst = reinterpret_cast<Color*>(mappedSysBufferTexture.pData );
	const size_t dstPitch = mappedSysBufferTexture.RowPitch / sizeof( Color );
	const size_t srcPitch = Graphics::WINDOW_WIDTH;
	const size_t rowBytes = srcPitch * sizeof( Color );

	// perform the copy line-by-line
	for( size_t y = 0u; y < Graphics::WINDOW_HEIGHT; y++ )
	{
		memcpy( &pDst[ y * dstPitch ],&pSysBuffer[y * srcPitch],rowBytes );
	}

	// release the adapter memory
	pImmediateContext->Unmap( pSysBufferTexture.Get(),0u );

	// render offscreen scene texture to back buffer
	pImmediateContext->IASetInputLayout( pInputLayout.Get() );
	pImmediateContext->VSSetShader( pVertexShader.Get(),nullptr,0u );
	pImmediateContext->PSSetShader( pPixelShader.Get(),nullptr,0u );
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	const UINT stride = sizeof( FSQVertex );
	const UINT offset = 0u;
	pImmediateContext->IASetVertexBuffers( 0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset );
	pImmediateContext->PSSetShaderResources( 0u,1u,pSysBufferTextureView.GetAddressOf() );
	pImmediateContext->PSSetSamplers( 0u,1u,pSamplerState.GetAddressOf() );
	pImmediateContext->Draw( 6u,0u );

	// flip back/front buffers
	if( FAILED( hr = pSwapChain->Present( 1u,0u ) ) )
	{
		if( hr == DXGI_ERROR_DEVICE_REMOVED )
		{
			throw CHILI_GFX_EXCEPTION( pDevice->GetDeviceRemovedReason(),L"Presenting back buffer [device removed]" );
		}
		else
		{
			throw CHILI_GFX_EXCEPTION( hr,L"Presenting back buffer" );
		}
	}
}

// DRAW FUNCTIONS
void Graphics::DrawLineSegment( const int& X0, const int& Y0, const int& X1, const int& Y1, const Color& COLOR )
{
	int x0 = X0;
	int y0 = Y0;

	int x1 = X1;
	int y1 = Y1;

	const int DX = int( X1 - X0 );
	const int DY = int( Y1 - Y0 );

	if(DY == 0 && DX == 0)
	{
		DrawPixel( x1, y1, COLOR );
	}
	else if(abs( DY ) > abs( DX ))
	{
		if(DY < 0)
		{
			std::swap( x0, x1 );
			std::swap( y0, y1 );
		}

		const float M = (float)DX / (float)DY;
		const float B = x0 - M * y0;

		int x = 0;
		for(int y = y0; y <= y1; y += 1)
		{
			x = (int)(M * y + B + 0.5f);
			DrawPixel( x, y, COLOR );
		}
	}
	else
	{
		if(DX < 0)
		{
			std::swap( x0, x1 );
			std::swap( y0, y1 );
		}

		const float M = (float)DY / (float)DX;
		const float B = y0 - M * x0;

		int y = 0;
		for(int x = x0; x <= x1; x = x + 1)
		{
			y = (int)(M * x + B + 0.5f);
			DrawPixel( x, y, COLOR );
		}
	}
}
void Graphics::DrawLineSegment( const Vector& A, const Vector& B, const float& WIDTH, const Color& COLOR )
{
	const Vector NORMAL( Normalize( B - A ) );
	const Vector OFFSET( Multiply( Rotate90CCW( NORMAL ), max(1.0f,WIDTH / 2.0f )) );
	const Vector V0( A - OFFSET );
	const Vector V1( A + OFFSET );
	const Vector V2( B - OFFSET );
	const Vector V3( B + OFFSET );

	if(WIDTH > 1)
	{
		DrawTriangle( true, V0, V1, V2, COLOR );
		DrawTriangle( true, V1, V2, V3, COLOR );
	}
	else
	{
		DrawLineSegment( A, B, COLOR );
	}
}
void Graphics::DrawRectangle( const bool& FILLED, const int& X0, const int& Y0, const int& X1, const int& Y1, const Color& COLOR )
{
	const Math::Vector A( (float)X0, (float)Y0 ); // top left
	const Math::Vector B( (float)X1, (float)Y0 ); // top right
	const Math::Vector C( (float)X1, (float)Y1 ); // bottom right
	const Math::Vector D( (float)X0, (float)Y1 ); // bottom left

	if(FILLED)
	{
		DrawTriangle( true, A, B, C, COLOR );
		DrawTriangle( true, C, D, A, COLOR );
	}
	else
	{
		DrawLineSegment( A, B, COLOR ); // top    left  to top    right
		DrawLineSegment( B, C, COLOR ); // top    right to bottom right
		DrawLineSegment( C, D, COLOR ); // bottom right to bottom left
		DrawLineSegment( D, A, COLOR ); // bottom left  to top    left
	}
}
void Graphics::DrawCircle( const bool& FILLED, const int& X, const int& Y, const int& RADIUS, const Color& COLOR )
{
	if(FILLED)
	{
		for(int y = Y - int(RADIUS + 0.5f); y < Y + int(RADIUS + 0.5f); y++)
		{
			for(int x = X - int(RADIUS + 0.5f); x < X + int(RADIUS + 0.5f); x++)
			{
				if( ((x - X) * (x - X) + (y - Y) * (y - Y)) < RADIUS * RADIUS )
				{
					DrawPixel( x, y, COLOR );
				}

				/*if(Math::DistanceBetween( {X,Y}, {(float)x,(float)y} ) <= RADIUS)
				{
					DrawPixel( x, y, COLOR );
				}*/
			}
		}
	}
	else
	{
		const double RADIUS_SQUARED = double( RADIUS ) * RADIUS;
		const int XPIVOT = int( RADIUS * 0.70710678118f + 0.5f );
		int y = 0;
		for(int x = 0; x <= XPIVOT; x++)
		{
			y = (int)(sqrt( RADIUS_SQUARED - double( x ) * x ) + 0.5);
			DrawPixel( (int)X + x, (int)Y + y, COLOR );
			DrawPixel( (int)X - x, (int)Y + y, COLOR );
			DrawPixel( (int)X + x, (int)Y - y, COLOR );
			DrawPixel( (int)X - x, (int)Y - y, COLOR );
			DrawPixel( (int)X + y, (int)Y + x, COLOR );
			DrawPixel( (int)X - y, (int)Y + x, COLOR );
			DrawPixel( (int)X + y, (int)Y - x, COLOR );
			DrawPixel( (int)X - y, (int)Y - x, COLOR );
		}
	}
}
void Graphics::DrawTriangle( const bool& FILLED, const Vector& A, const Vector& B, const Vector& C, const Color& COLOR )
{	
	if( FILLED )
	{
		DrawTriangle( A, B, C, COLOR );
	}
	else
	{
		DrawLineSegment( A, B, COLOR );
		DrawLineSegment( B, C, COLOR );
		DrawLineSegment( C, A, COLOR );
	}	
}
void Graphics::DrawTile(const Tile& T, const Surface& IMAGE)
{
	const TextureVertex T0 = { T.GetPosition(0),T.GetTexCoord(0) };
	const TextureVertex T1 = { T.GetPosition(1),T.GetTexCoord(1) };
	const TextureVertex T2 = { T.GetPosition(2),T.GetTexCoord(2) };
	const TextureVertex T3 = { T.GetPosition(3),T.GetTexCoord(3) };
	DrawTriangleTex(T0, T1, T3, IMAGE);
	DrawTriangleTex(T0, T3, T2, IMAGE);
}
void Graphics::DrawVector( const Vector& V, const Color& COLOR )
{
	DrawLineSegment( 0, 0, (int)V.x, (int)V.y, COLOR );

	const Vector NORMAL( Normalize( V ) );
	const Vector A( Multiply( NORMAL, 100.0f ) );
	const Vector B( Add( A, Rotate( Multiply( NORMAL, 20.0f ), 60.0f ) ) );
	const Vector C( Add( A, Rotate( Multiply( NORMAL, 20.0f ), -60.0f ) ) );

	DrawTriangle( true, A, B, C, Colors::White );
}
void Graphics::DrawVector( const Vector& TAIL, const Vector& TOP, const Color& COLOR )
{
	const Vector VECTOR( Subtract( TOP, TAIL ) );
	const float	 LENGTH( Length(VECTOR) );
	const Vector NORMAL( Normalize( VECTOR ) );
	const Vector A( Add(TAIL, Multiply( NORMAL, LENGTH ) ) );
	const Vector B( Add( A, Rotate( Multiply( NORMAL, LENGTH / 10.0f ), 60.0f ) ) );
	const Vector C( Add( A, Rotate( Multiply( NORMAL, LENGTH / 10.0f ), -60.0f ) ) );
	const float  D( Length( Subtract( B, C ) ) );
	const Vector X0 = TAIL;
	const Vector X1 = Subtract( TOP, Multiply( NORMAL, LENGTH - LENGTH * 0.95f ) );
	
	DrawCircle( true, X0, D / 2.0f, COLOR );
	DrawLineSegment(X0, X1, D / 4.0f, COLOR );
	DrawTriangle( true, A, B, C, COLOR );
}

void Graphics::DrawImage( const bool& ALPHA, const int& LEFT, const int& TOP, const int& RIGHT, const int& BOTTOM, const Surface& S )
{
	assert( LEFT <= RIGHT );
	assert( TOP <= BOTTOM );

	const RectI IMAGE = {LEFT,TOP,RIGHT,BOTTOM};
	const RectI WINDOW = {WINDOW_LEFT,WINDOW_TOP,WINDOW_RIGHT,WINDOW_BOTTOM};

	if(Overlap( IMAGE, WINDOW ))
	{
		const UINT	IX_START /**/ = max( WINDOW_LEFT, /****/ LEFT );
		const UINT	IY_START /**/ = max( WINDOW_TOP, /*****/ TOP );
		const UINT	IY_END /****/ = min( WINDOW_BOTTOM, /**/ BOTTOM );
		const UINT	IX_END /****/ = min( WINDOW_RIGHT, /***/ RIGHT );
		const float	XOFF = Normalise( (float)LEFT, (float)RIGHT, (float)IX_START );
		const float	YOFF = Normalise( (float)TOP, (float)BOTTOM, (float)IY_START );
		const float PROPORTION_X = S.GetWidth() / float(RIGHT - LEFT);
		const float PROPORTION_Y = S.GetHeight() / float(BOTTOM - TOP);
		const float X_START = XOFF * S.GetWidth();
		const float Y_START = YOFF * S.GetHeight();

		UINT x = 0u;
		UINT y = 0u;
		
		if(ALPHA)
		{
			for(UINT iy = IY_START; iy < IY_END; iy++)
			{
				for(UINT ix = IX_START; ix < IX_END; ix++)
				{
					x = UINT( X_START + (ix - IX_START) * PROPORTION_X + 0.5f );
					y = UINT( Y_START + (iy - IY_START) * PROPORTION_Y + 0.5f );

					const Color C0 = GetPixel( ix, iy );
					const Color C1 = S.GetPixel( x, y );

					DrawPixel( (int)ix, (int)iy, AlphaBlend( C0, C1 ) );
				}
			}
		}
		else
		{
			for(UINT iy = IY_START; iy < IY_END; iy++)
			{
				for(UINT ix = IX_START; ix < IX_END; ix++)
				{
					x = UINT( X_START + (ix - IX_START) * PROPORTION_X );
					y = UINT( Y_START + (iy - IY_START) * PROPORTION_Y );

					DrawPixel( (int)ix, (int)iy, S.GetPixel( x, y ) );
				}
			}
		}
	}
}
void Graphics::DrawImage( const bool& ALPHA, const int& X, const int& Y, const Surface& S )
{
	const int	LEFT = X;
	const int	TOP = Y;
	const int	RIGHT = LEFT + S.GetWidth();
	const int	BOTTOM = TOP + S.GetHeight();

	DrawImage( ALPHA, LEFT, TOP, RIGHT, BOTTOM, S );
}

// PRIVATE TRIANGLE FUNCTIONS
void Graphics::DrawTriangle( const Vector& A, const Vector& B, const Vector& C, const Color& COLOR )
{
	// copies that can be swapped around
	Vector a = A;
	Vector b = B;
	Vector c = C;

	// using pointers so we can swap (for sorting purposes)
	const Vector* pv0 = &a;
	const Vector* pv1 = &b;
	const Vector* pv2 = &c;

	// sorting vertices by y
	if( pv1->y < pv0->y ) swap( pv0, pv1 );
	if( pv2->y < pv1->y ) swap( pv1, pv2 );
	if( pv1->y < pv0->y ) swap( pv0, pv1 );

	if( pv0->y == pv1->y ) // natural flat top
	{
		// sorting top vertices by x
		if( pv1->x < pv0->x ) swap( pv0, pv1 );

		DrawFlatTopTriangle( *pv0, *pv1, *pv2, COLOR );
	}
	else if( pv1->y == pv2->y ) // natural flat bottom
	{
		// sorting bottom vertices by x
		if( pv2->x < pv1->x ) swap( pv1, pv2 );

		DrawFlatBottomTriangle( *pv0, *pv1, *pv2, COLOR );
	}
	else // general triangle
	{
		// find splitting vertex interpolant
		const float alphaSplit =
			(pv1->y - pv0->y) /
			(pv2->y - pv0->y);
		const auto vi = Interpolate( *pv0, *pv2, alphaSplit );

		if( pv1->x < vi.x ) // major right
		{
			DrawFlatBottomTriangle( *pv0, *pv1, vi, COLOR );
			DrawFlatTopTriangle( *pv1, vi, *pv2, COLOR );
		}
		else // major left
		{
			DrawFlatBottomTriangle( *pv0, vi, *pv1, COLOR );
			DrawFlatTopTriangle( vi, *pv1, *pv2, COLOR );
		}
	}
}
void Graphics::DrawFlatTopTriangle( const Vector& it0, const Vector& it1, const Vector& it2, const Color& COLOR)
{
	// calulcate dVertex / dy
	// change in interpolant for every 1 change in y
	const float delta_y = it2.y - it0.y;
	const auto dit0 = Divide( Subtract( it2, it0 ), delta_y );
	const auto dit1 = Divide( Subtract( it2, it1 ), delta_y );

	// create right edge interpolant
	const auto& itEdge1 = it1;

	// call the flat triangle render routine
	DrawFlatTriangle( it0, it1, it2, dit0, dit1, itEdge1, COLOR );
}
void Graphics::DrawFlatBottomTriangle( const Vector& it0, const Vector& it1, const Vector& it2, const Color& COLOR)
{
	// calulcate dVertex / dy
	// change in interpolant for every 1 change in y
	const float delta_y = it2.y - it0.y;
	const auto dit0 = Divide(Subtract(it1, it0), delta_y);
	const auto dit1 = Divide(Subtract(it2, it0), delta_y);

	// create right edge interpolant
	const auto& itEdge1 = it0;

	// call the flat triangle render routine
	DrawFlatTriangle( it0, it1, it2, dit0, dit1, itEdge1, COLOR );
}
void Graphics::DrawFlatTriangle( const Vector& it0,	const Vector& it1, const Vector& it2, const Vector& dv0, const Vector& dv1, Vector itEdge1, const Color& COLOR)
{
	// create edge interpolant for left edge (always v0)
	auto itEdge0 = it0;

	// calculate start and end scanlines
	const int yStart = std::max( (int)ceil( it0.y - 0.5f ), 0 );
	const int yEnd = std::min( (int)ceil( it2.y - 0.5f ), (int)Graphics::WINDOW_HEIGHT - 1 ); // the scanline AFTER the last line drawn

	// do interpolant prestep
	itEdge0 += Multiply(dv0, (float( yStart ) + 0.5f - it0.y));
	itEdge1 += Multiply(dv1, (float( yStart ) + 0.5f - it0.y));

	for( int y = yStart; y < yEnd; y++, itEdge0 += dv0, itEdge1 += dv1 )
	{
		// calculate start and end pixels
		const int xStart = max( (int)ceil( itEdge0.x - 0.5f ), 0 );
		const int xEnd = min( (int)ceil( itEdge1.x - 0.5f ), WINDOW_WIDTH - 1 ); // the pixel AFTER the last pixel drawn

		// create scanline interpolant startpoint
		// (some waste for interpolating x,y,z, but makes life easier not having
		//  to split them off, and z will be needed in the future anyways...)
		auto iLine = itEdge0;

		// calculate delta scanline interpolant / dx
		const float dx = itEdge1.x - itEdge0.x;
		const auto diLine = Divide((itEdge1 - iLine), dx);

		// prestep scanline interpolant
		iLine += Multiply(diLine, (float( xStart ) + 0.5f - itEdge0.x));

		for( int x = xStart; x < xEnd; x++, iLine += diLine )
		{			
			DrawPixel( x, y, COLOR );			
		}
	}
}

//void Graphics::DrawTriangle(const Vector& v0, const Vector& v1, const Vector& v2, Color c)
//{
//	// using pointers so we can swap (for sorting purposes)
//	const Vector* pv0 = &v0;
//	const Vector* pv1 = &v1;
//	const Vector* pv2 = &v2;
//
//	// sorting vertices by y
//	if (pv1->y < pv0->y) std::swap(pv0, pv1);
//	if (pv2->y < pv1->y) std::swap(pv1, pv2);
//	if (pv1->y < pv0->y) std::swap(pv0, pv1);
//
//	if (pv0->y == pv1->y) // natural flat top
//	{
//		// sorting top vertices by x
//		if (pv1->x < pv0->x) std::swap(pv0, pv1);
//		DrawFlatTopTriangle(*pv0, *pv1, *pv2, c);
//	}
//	else if (pv1->y == pv2->y) // natural flat bottom
//	{
//		// sorting bottom vertices by x
//		if (pv2->x < pv1->x) std::swap(pv1, pv2);
//		DrawFlatBottomTriangle(*pv0, *pv1, *pv2, c);
//	}
//	else // general triangle
//	{
//		// find splitting vertex
//		const float alphaSplit =
//			(pv1->y - pv0->y) /
//			(pv2->y - pv0->y);
//		const Vector vi = *pv0 + (*pv2 - *pv0) * alphaSplit;
//
//		if (pv1->x < vi.x) // major right
//		{
//			DrawFlatBottomTriangle(*pv0, *pv1, vi, c);
//			DrawFlatTopTriangle(*pv1, vi, *pv2, c);
//		}
//		else // major left
//		{
//			DrawFlatBottomTriangle(*pv0, vi, *pv1, c);
//			DrawFlatTopTriangle(vi, *pv1, *pv2, c);
//		}
//	}
//}

void Graphics::DrawTriangleTex(const Math::TextureVertex& v0, const Math::TextureVertex& v1, const Math::TextureVertex& v2, const Surface& tex)
{
	// using pointers so we can swap (for sorting purposes)
	const Math::TextureVertex* pv0 = &v0;
	const Math::TextureVertex* pv1 = &v1;
	const Math::TextureVertex* pv2 = &v2;

	// sorting vertices by y
	if (pv1->pos.y < pv0->pos.y) std::swap(pv0, pv1);
	if (pv2->pos.y < pv1->pos.y) std::swap(pv1, pv2);
	if (pv1->pos.y < pv0->pos.y) std::swap(pv0, pv1);

	if (pv0->pos.y == pv1->pos.y) // natural flat top
	{
		// sorting top vertices by x
		if (pv1->pos.x < pv0->pos.x) std::swap(pv0, pv1);
		DrawFlatTopTriangleTex(*pv0, *pv1, *pv2, tex);
	}
	else if (pv1->pos.y == pv2->pos.y) // natural flat bottom
	{
		// sorting bottom vertices by x
		if (pv2->pos.x < pv1->pos.x) std::swap(pv1, pv2);
		DrawFlatBottomTriangleTex(*pv0, *pv1, *pv2, tex);
	}
	else // general triangle
	{
		// find splitting vertex
		const float alphaSplit =
			(pv1->pos.y - pv0->pos.y) /
			(pv2->pos.y - pv0->pos.y);
		const Math::TextureVertex vi = pv0->InterpolateTo(*pv2, alphaSplit);

		if (pv1->pos.x < vi.pos.x) // major right
		{
			DrawFlatBottomTriangleTex(*pv0, *pv1, vi, tex);
			DrawFlatTopTriangleTex(*pv1, vi, *pv2, tex);
		}
		else // major left
		{
			DrawFlatBottomTriangleTex(*pv0, vi, *pv1, tex);
			DrawFlatTopTriangleTex(vi, *pv1, *pv2, tex);
		}
	}
}

//void Graphics::DrawFlatTopTriangle(const Vector& v0, const Vector& v1, const Vector& v2, Color c)
//{
//	// calulcate slopes in screen space
//	const float m0 = (v2.x - v0.x) / (v2.y - v0.y);
//	const float m1 = (v2.x - v1.x) / (v2.y - v1.y);
//
//	// calculate start and end scanlines
//	const int yStart = (int)ceil(v0.y - 0.5f);
//	const int yEnd = (int)ceil(v2.y - 0.5f); // the scanline AFTER the last line drawn
//
//	for (int y = yStart; y < yEnd; y++)
//	{
//		// caluclate start and end points (x-coords)
//		// add 0.5 to y value because we're calculating based on pixel CENTERS
//		const float px0 = m0 * (float(y) + 0.5f - v0.y) + v0.x;
//		const float px1 = m1 * (float(y) + 0.5f - v1.y) + v1.x;
//
//		// calculate start and end pixels
//		const int xStart = (int)ceil(px0 - 0.5f);
//		const int xEnd = (int)ceil(px1 - 0.5f); // the pixel AFTER the last pixel drawn
//
//		for (int x = xStart; x < xEnd; x++)
//		{
//			PutPixel(x, y, c);
//		}
//	}
//}

//void Graphics::DrawFlatBottomTriangle(const Vector& v0, const Vector& v1, const Vector& v2, Color c)
//{
//	// calulcate slopes in screen space
//	const float m0 = (v1.x - v0.x) / (v1.y - v0.y);
//	const float m1 = (v2.x - v0.x) / (v2.y - v0.y);
//
//	// calculate start and end scanlines
//	const int yStart = (int)ceil(v0.y - 0.5f);
//	const int yEnd = (int)ceil(v2.y - 0.5f); // the scanline AFTER the last line drawn
//
//	for (int y = yStart; y < yEnd; y++)
//	{
//		// caluclate start and end points
//		// add 0.5 to y value because we're calculating based on pixel CENTERS
//		const float px0 = m0 * (float(y) + 0.5f - v0.y) + v0.x;
//		const float px1 = m1 * (float(y) + 0.5f - v0.y) + v0.x;
//
//		// calculate start and end pixels
//		const int xStart = (int)ceil(px0 - 0.5f);
//		const int xEnd = (int)ceil(px1 - 0.5f); // the pixel AFTER the last pixel drawn
//
//		for (int x = xStart; x < xEnd; x++)
//		{
//			PutPixel(x, y, c);
//		}
//	}
//}

void Graphics::DrawFlatTopTriangleTex(const Math::TextureVertex& v0, const Math::TextureVertex& v1, const Math::TextureVertex& v2, const Surface& tex)
{
	// calulcate dVertex / dy
	const float delta_y = v2.pos.y - v0.pos.y;
	const Math::TextureVertex dv0 = (v2 - v0) / delta_y;
	const Math::TextureVertex dv1 = (v2 - v1) / delta_y;

	// create right edge interpolant
	Math::TextureVertex itEdge1 = v1;

	// call the flat triangle render routine
	DrawFlatTriangleTex(v0, v1, v2, tex, dv0, dv1, itEdge1);
}

void Graphics::DrawFlatBottomTriangleTex(const Math::TextureVertex& v0, const Math::TextureVertex& v1, const Math::TextureVertex& v2, const Surface& tex)
{
	// calulcate dVertex / dy
	const float delta_y = v2.pos.y - v0.pos.y;
	const Math::TextureVertex dv0 = (v1 - v0) / delta_y;
	const Math::TextureVertex dv1 = (v2 - v0) / delta_y;

	// create right edge interpolant
	Math::TextureVertex itEdge1 = v0;

	// call the flat triangle render routine
	DrawFlatTriangleTex(v0, v1, v2, tex, dv0, dv1, itEdge1);
}

void Graphics::DrawFlatTriangleTex(const Math::TextureVertex& v0, const Math::TextureVertex& v1, const Math::TextureVertex& v2, const Surface& tex,
	const Math::TextureVertex& dv0, const Math::TextureVertex& dv1, Math::TextureVertex& itEdge1)
{
	// create edge interpolant for left edge (always v0)
	Math::TextureVertex itEdge0 = v0;

	// calculate start and end scanlines
	const int yStart = (int)ceil(v0.pos.y - 0.5f);
	const int yEnd = (int)ceil(v2.pos.y - 0.5f); // the scanline AFTER the last line drawn

	// do interpolant prestep
	itEdge0 += dv0 * (float(yStart) + 0.5f - v0.pos.y);
	itEdge1 += dv1 * (float(yStart) + 0.5f - v0.pos.y);

	// init tex width/height and clamp values
	const float tex_width = float(tex.GetWidth());
	const float tex_height = float(tex.GetHeight());
	const float tex_clamp_x = tex_width - 1.0f;
	const float tex_clamp_y = tex_height - 1.0f;

	for (int y = yStart; y < yEnd; y++, itEdge0 += dv0, itEdge1 += dv1)
	{
		// calculate start and end pixels
		const int xStart = (int)ceil(itEdge0.pos.x - 0.5f);
		const int xEnd = (int)ceil(itEdge1.pos.x - 0.5f); // the pixel AFTER the last pixel drawn

		// calculate scanline dTexCoord / dx
		const Vector dtcLine = (itEdge1.tc - itEdge0.tc) / (itEdge1.pos.x - itEdge0.pos.x);

		// create scanline tex coord interpolant and prestep
		Vector itcLine = itEdge0.tc + dtcLine * (float(xStart) + 0.5f - itEdge0.pos.x);

		for (int x = xStart; x < xEnd; x++, itcLine += dtcLine)
		{
			DrawPixel(x, y, tex.GetPixel(
				int(std::min(itcLine.x * tex_width, tex_clamp_x)),
				int(std::min(itcLine.y * tex_height, tex_clamp_y))));
			// need std::min b/c tc.x/y == 1.0, we'll read off edge of tex
			// and with fp err, tc.x/y can be > 1.0 (by a tiny amount)
		}
	}
}