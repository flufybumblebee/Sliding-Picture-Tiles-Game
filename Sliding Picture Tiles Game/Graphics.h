/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	Graphics.h																			  *
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
#include <d3d11.h>
#include <wrl.h>
#include "ChiliException.h"
#include "Colors.h"
#include "GDIPlusManager.h"
#include "Surface.h"
#include "Math.h"
#include "Tile.h"

class Graphics
{
public:
	class Exception: public ChiliException
	{
	public:
		Exception( HRESULT hr, const std::wstring& note, const wchar_t* file, unsigned int line );
		std::wstring GetErrorName() const;
		std::wstring GetErrorDescription() const;
		virtual std::wstring GetFullMessage() const override;
		virtual std::wstring GetExceptionType() const override;
	private:
		HRESULT hr;
	};
private:
	// vertex format for the framebuffer fullscreen textured quad
	struct FSQVertex
	{
		float x, y, z;		// position
		float u, v;			// texcoords
	};

public:
	Graphics( class HWNDKey& key );
	Graphics( const Graphics& ) = delete;
	~Graphics();
	Graphics& operator=( const Graphics& ) = delete;

public:
	void BeginFrame();
	void EndFrame();

public:
	// PIXEL
	void	DrawPixel( const int& X, const int& Y, const Color& COLOR )
	{
		/*assert( X >= 0 );
		assert( X < int( Graphics::WINDOW_WIDTH ) );
		assert( Y >= 0 );
		assert( Y < int( Graphics::WINDOW_HEIGHT ) );

		pSysBuffer[Graphics::WINDOW_WIDTH * Y + X] = COLOR;*/

		if( X >= 0 &&
			Y >= 0 &&
			X < WINDOW_WIDTH &&
			Y < WINDOW_HEIGHT )
		{
			pSysBuffer[Graphics::WINDOW_WIDTH * Y + X] = COLOR;
		}
	}
	Color	GetPixel( const unsigned int& X, const unsigned int& Y )
	{
		return pSysBuffer[WINDOW_WIDTH * Y + X];
	}

	template <typename T>
	void	DrawPixel( const T& X, const T& Y, const Color& COLOR )
	{
		DrawPixel( (int)X, (int)Y, COLOR );
	}
	template <typename T0, typename T1>
	void	DrawPixel( const T0& X, const T0& Y, const T1& R, const T1& G, const T1& B )
	{
		DrawPixel( (int)X, (int)Y, { (UCHAR)R, (UCHAR)G, (UCHAR)B } );
	}
	void	DrawPixel( const Math::Vector& V, const Color& COLOR)
	{
		DrawPixel((int)V.x, (int)V.y, COLOR);
	}


	// LINE SEGMENT
	void DrawLineSegment(const int& X0, const int& Y0, const int& X1, const int& Y1, const Color& COLOR);
	void DrawLineSegment(const Math::Vector& A, const Math::Vector& B, const Color& COLOR)
	{
		DrawLineSegment((int)A.x, (int)A.y, (int)B.x, (int)B.y, COLOR);
	}
	void DrawLineSegment(const Math::LineSegment& LS, const Color& COLOR)
	{
		DrawLineSegment( (int)LS.a.x, (int)LS.a.y, (int)LS.b.x, (int)LS.b.y, COLOR);
	}
	
	void DrawLineSegment(const Math::Vector& A, const Math::Vector& B, const float& WIDTH, const Color& COLOR );
	void DrawLineSegment(const int& X0, const int& Y0, const int& X1, const int& Y1, const int& WIDTH, const Color& COLOR )
	{
		DrawLineSegment( {(float)X0,(float)Y0}, {(float)X1, (float)Y1}, (float)WIDTH, COLOR );
	}
	
	void DrawLineSegments(const std::vector<Math::LineSegment>& LINES, const Color& COLOR)
	{
		for ( const auto& L : LINES)
		{
			DrawLineSegment(L, COLOR);
		}
	}
	

	// RECTANGLE
	void DrawRectangle( const bool& FILLED, const int& LEFT, const int& TOP, const int& RIGHT, const int& BOTTOM, const Color& COLOR);
	void DrawRectangle( const bool& FILLED, const Math::Vector& A, const Math::Vector& B, const Color& COLOR)
	{
		DrawRectangle( FILLED, (int)A.x, (int)A.y, (int)B.x, (int)B.y, COLOR);
	}
	void DrawRectangle( const bool& FILLED, const Math::LineSegment& LS, const Color& COLOR)
	{
		DrawRectangle(FILLED, (int)LS.a.x, (int)LS.a.y, (int)LS.b.x, (int)LS.b.y, COLOR);
	}
	void DrawRectangle( const bool& FILLED, const Math::RectF& RECT, const Color& COLOR)
	{
		DrawRectangle( FILLED, (int)RECT.left, (int)RECT.top, (int)RECT.right, (int)RECT.bottom, COLOR);
	}
	

	// CIRCLE
	void DrawCircle(const bool& FILLED, const int& X, const int& Y, const int& RADIUS, const Color& COLOR);
	void DrawCircle(const bool& FILLED, const Math::Circle& CIRCLE, const Color& COLOR)
	{
		DrawCircle(FILLED, CIRCLE.position, CIRCLE.radius, COLOR);
	}
	void DrawCircle(const bool& FILLED, const Math::Vector& V, const float& RADIUS, const Color& COLOR)
	{
		DrawCircle(FILLED, (int)V.x, (int)V.y, (int)RADIUS, COLOR);
	}
	void DrawCircle( const bool& FILLED, const float& X, const float& Y, const float &RADIUS, const UCHAR& R, const UCHAR& G, const UCHAR& B )
	{
		DrawCircle( FILLED, (int)X, (int)Y, (int)RADIUS, {R,G,B} );
	}
	
	void DrawCircles(const bool& FILLED, const std::vector<Math::Circle>& CIRCLES, const Color& COLOR)
	{
		for (const auto& C : CIRCLES)
		{
			DrawCircle(FILLED, C, COLOR);
		}
	}
	void DrawCircles(const bool& FILLED, const std::vector<Math::Vector>& VECTORS, const float& RADIUS, const Color& COLOR)
	{
		for (const auto& V : VECTORS)
		{
			DrawCircle(FILLED, V, RADIUS, COLOR);
		}
	}
		

	// SPECIAL
	void DrawTile(
		const std::vector<std::array<Math::Vector, 4>>& POSITIONS,
		const std::vector<std::array<Math::Vector, 4>>& TEX_COORDS,
		const Tile& T,
		const Surface& IMAGE);
	void DrawVector( const Math::Vector& V, const Color& COLOR );
	void DrawVector( const Math::Vector& A, const Math::Vector& B, const Color& COLOR );


	// IMAGE
	void DrawImage( const bool& ALPHA, const int& X0, const int& Y0, const int& X1, const int& Y1, const Surface& S );
	void DrawImage( const bool& ALPHA, const float& X0, const float& Y0, const float& X1, const float& Y1, const Surface& S )
	{
		DrawImage( ALPHA, (int)X0, (int)Y0, (int)X1, (int)Y1, S );
	}
	void DrawImage( const bool& ALPHA, const Math::RectI& R, const Surface& S )
	{
		DrawImage( ALPHA, R.left, R.top, R.right, R.bottom, S );
	}
	void DrawImage( const bool& ALPHA, const Math::RectF& R, const Surface& S )
	{
		DrawImage( ALPHA, (int)R.left, (int)R.top, (int)R.right, (int)R.bottom, S );
	}
	void DrawImage( const bool& ALPHA, const Math::Vector& A, const Math::Vector& B, const Surface& S )
	{
		DrawImage( ALPHA, (int)A.x, (int)A.y, (int)B.x, (int)B.y, S );
	}
		
	void DrawImage( const bool& ALPHA, const int& X, const int& Y, const Surface& S );
	void DrawImage( const bool& ALPHA, const float& X, const float& Y, const Surface& S )
	{
		DrawImage( ALPHA, (int)X, (int)Y, S );
	}
	void DrawImage( const bool& ALPHA, const Math::Vector& V, const Surface& S )
	{
		DrawImage( ALPHA, V.x, V.y, S );
	}
	

	// TRIANGLE	
	void DrawTriangle(const bool& FILLED, const Math::Vector& A, const Math::Vector& B, const Math::Vector& C, const Color& COLOR );
	void DrawTriangle( const bool& FILLED, const Math::Triangle& T, const Color& COLOR )
	{
		DrawTriangle( FILLED, T.GetA(), T.GetB(), T.GetC(), COLOR );
	}
	void DrawTriangleTex(const Math::TextureVertex& v0, const Math::TextureVertex& v1, const Math::TextureVertex& v2, const Surface& tex);

private:
	void DrawTriangle( const Math::Vector& A, const Math::Vector& B, const Math::Vector& C, const Color& COLOR );
	void DrawFlatTopTriangle( const Math::Vector& it0, const Math::Vector& it1,	const Math::Vector& it2, const Color& COLOR );
	void DrawFlatBottomTriangle( const Math::Vector& it0, const Math::Vector& it1, const Math::Vector& it2, const Color& COLOR );
	void DrawFlatTriangle( const Math::Vector& it0, const Math::Vector& it1, const Math::Vector& it2, const Math::Vector& dv0, const Math::Vector& dv1, Math::Vector itEdge1, const Color& COLOR );
	void DrawFlatTopTriangleTex(const Math::TextureVertex& v0, const Math::TextureVertex& v1, const Math::TextureVertex& v2, const Surface& tex);
	void DrawFlatBottomTriangleTex(const Math::TextureVertex& v0, const Math::TextureVertex& v1, const Math::TextureVertex& v2, const Surface& tex);
	void DrawFlatTriangleTex(const Math::TextureVertex& v0, const Math::TextureVertex& v1, const Math::TextureVertex& v2, const Surface& tex,
		const Math::TextureVertex& dv0, const Math::TextureVertex& dv1, Math::TextureVertex& itEdge1);


private:
	GDIPlusManager										gdipMan;
	Microsoft::WRL::ComPtr<IDXGISwapChain>				pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device>				pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			pImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				pSysBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pSysBufferTextureView;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			pSamplerState;
	D3D11_MAPPED_SUBRESOURCE							mappedSysBufferTexture = { nullptr,0u,0u };
	Color*	                                            pSysBuffer;

public:
	static constexpr int WINDOW_WIDTH = 500;
	static constexpr int WINDOW_HEIGHT = 500;
	static constexpr int WINDOW_LEFT = 0;
	static constexpr int WINDOW_TOP = 0;
	static constexpr int WINDOW_RIGHT = WINDOW_WIDTH - 1;
	static constexpr int WINDOW_BOTTOM = WINDOW_HEIGHT - 1;
	static constexpr const unsigned int	COLOR = 255u;
};