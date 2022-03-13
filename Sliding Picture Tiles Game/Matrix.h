#pragma once

#include "Vector.h" // Math::Vector

#include <array>	// array
#include <vector>	// vector
#include <cmath>	// sin, cos

namespace Math
{
	class Matrix
	{
	public:
		Matrix()
		{
			matrix[0].fill( 0.0f );
			matrix[1].fill( 0.0f );
			matrix[2].fill( 0.0f );
			matrix[3].fill( 0.0f );

			matrix[0][0] = 1.0f;
			matrix[1][1] = 1.0f;
			matrix[2][2] = 1.0f;
			matrix[3][3] = 1.0f;
		}
		Matrix( const Matrix& MATRIX )
			:
			matrix( MATRIX.matrix )
		{}
		~Matrix() = default;

	public:
		Matrix	operator * ( const Matrix& RHS ) const
		{
			Matrix result;
			for(size_t j = 0; j < 4; j++)
			{
				for(size_t k = 0; k < 4; k++)
				{
					float sum = 0.0f;
					for(size_t i = 0; i < 4; i++)
					{
						sum += this->matrix[j][i] * RHS.matrix[i][k];
					}
					result.matrix[j][k] = sum;
				}
			}
			return result;
		}
		Matrix&	operator * ( const Matrix& RHS )
		{
			const Matrix COPY = *this;

			for(size_t j = 0; j < 4; j++)
			{
				for(size_t k = 0; k < 4; k++)
				{
					float sum = 0.0f;
					for(size_t i = 0; i < 4; i++)
					{
						sum += COPY.matrix[j][i] * RHS.matrix[i][k];
					}
					this->matrix[j][k] = sum;
				}
			}

			return *this;

			/*Matrix result;
			for(size_t j = 0; j < 4; j++)
			{
				for(size_t k = 0; k < 4; k++)
				{
					float sum = 0.0f;
					for(size_t i = 0; i < 4; i++)
					{
						sum += this->matrix[j][i] * RHS.matrix[i][k];
					}
					result.matrix[j][k] = sum;
				}
			}

			*this = result;

			return *this;*/
		}

	public:
		
		/*Matrix Translate( const Vector& TRANSLATE )
		{
			matrix[0] = { 1.0f, 0.0f, 0.0f, TRANSLATE.x };
			matrix[1] = { 0.0f, 1.0f, 0.0f, TRANSLATE.y };
			matrix[2] = { 0.0f, 0.0f, 1.0f, TRANSLATE.z };
			matrix[3] = { 0.0f, 0.0f, 0.0f, 1.0f };

			return Matrix( *this );
		}*/

	public:
		static constexpr size_t SIZE = 4u;
		std::array<std::array<float, SIZE>, SIZE> matrix;
	};

	static Matrix	operator *  ( const Matrix& A, const Matrix& B )
	{
		Matrix result;

		for(size_t j = 0; j < 4; j++)
		{
			for(size_t k = 0; k < 4; k++)
			{
				float sum = 0.0f;
				for(size_t i = 0; i < 4; i++)
				{
					sum += A.matrix[j][i] * B.matrix[i][k];
				}
				result.matrix[j][k] = sum;
			}
		}
		return result;

		//Matrix result;
		//
		//for( size_t i = 0u; i < Matrix::SIZE; i++ )
		//{
		//	for( size_t row = 0u; row < Matrix::SIZE; row++ )
		//	{
		//		for( size_t col = 0u; col < Matrix::SIZE; col++ )
		//		{
		//			result.matrix[i][row] += A.matrix[i][col] * B.matrix[col][row];
		//		}
		//	}
		//}
		//
		//return result;
	}
	
	
	/*static Matrix	Multiply( const Matrix& A, const Matrix& B )
	{
		Matrix result;

		for(size_t i = 0u; i < 4; i++)
		{
			for(size_t row = 0u; row < 4; row++)
			{
				for(size_t col = 0u; col < 4; col++)
				{
					result.matrix[i][row] += A.matrix[i][col] * B.matrix[col][row];
				}
			}
		}

		return result;
	}*/
	static Matrix	Identity()
	{
		Matrix result;

		result.matrix[0] = { 1.0f, 0.0f, 0.0f, 0.0f };
		result.matrix[1] = { 0.0f, 1.0f, 0.0f, 0.0f };
		result.matrix[2] = { 0.0f, 0.0f, 1.0f, 0.0f };
		result.matrix[3] = { 0.0f, 0.0f, 0.0f, 1.0f };

		return result;
	}
	static Matrix	Scale( const float& SCALE )
	{
		Matrix result;

		result.matrix[0] = { SCALE, 0.0f, 0.0f, 0.0f };
		result.matrix[1] = { 0.0f, SCALE, 0.0f, 0.0f };
		result.matrix[2] = { 0.0f, 0.0f, SCALE, 0.0f };
		result.matrix[3] = { 0.0f, 0.0f, 0.0f, 1.0f };

		return result;
	}
	static Matrix	RotateX( const float& ANGLE )
	{
		const float SINE_ANGLE = sin( ANGLE );
		const float COS_ANGLE = cos( ANGLE );

		Matrix result;

		result.matrix[0] = {1.0f, 0.0f, 0.0f, 0.0f};
		result.matrix[1] = {0.0f, COS_ANGLE, SINE_ANGLE, 0.0f};
		result.matrix[2] = {0.0f, -SINE_ANGLE, COS_ANGLE, 0.0f};
		result.matrix[3] = {0.0f, 0.0f, 0.0f, 1.0f};

		return result;
	}
	static Matrix	RotateY( const float& ANGLE )
	{
		const float SINE_ANGLE = sin( ANGLE );
		const float COS_ANGLE = cos( ANGLE );

		Matrix result;

		result.matrix[0] = {COS_ANGLE, 0.0f, -SINE_ANGLE, 0.0f};
		result.matrix[1] = {0.0f, 1.0f, 0.0f, 0.0f};
		result.matrix[2] = {SINE_ANGLE, 0.0f, COS_ANGLE, 0.0f};
		result.matrix[3] = {0.0f, 0.0f, 0.0f, 1.0f};

		return result;
	}
	static Matrix	RotateZ( const float& ANGLE )
	{
		const float COS_ANGLE = std::cos( ANGLE );
		const float SINE_ANGLE = std::sin( ANGLE );

		Matrix result;

		result.matrix[0] = {COS_ANGLE, SINE_ANGLE, 0.0f, 0.0f};
		result.matrix[1] = {-SINE_ANGLE, COS_ANGLE, 0.0f, 0.0f};
		result.matrix[2] = {0.0f, 0.0f, 1.0f, 0.0f};
		result.matrix[3] = {0.0f, 0.0f, 0.0f, 1.0f};

		return result;
	}
	static Matrix	Rotate( const float& ANGLE_X, const float& ANGLE_Y, const float& ANGLE_Z )
	{
		return
			RotateX( ANGLE_X ) *
			RotateY( ANGLE_Y ) *
			RotateZ( ANGLE_Z );
	}

	/*static Matrix	Translate( const Vector& TRANSLATE )
	{
		Matrix result;

		result.matrix[0] = {1.0f, 0.0f, 0.0f, TRANSLATE.x};
		result.matrix[1] = {0.0f, 1.0f, 0.0f, TRANSLATE.y};
		result.matrix[2] = {0.0f, 0.0f, 1.0f, TRANSLATE.z};
		result.matrix[3] = {0.0f, 0.0f, 0.0f, 1.0f};

		return result;
	}*/

	/*static Matrix	Translate( const Vector& TRANSLATE )
	{
		Matrix result;

		result.matrix[0] = {1.0f, 0.0f, 0.0f, 0.0f};
		result.matrix[1] = {0.0f, 1.0f, 0.0f, 0.0f};
		result.matrix[2] = {0.0f, 0.0f, 1.0f, 0.0f};
		result.matrix[3] = {TRANSLATE.x, TRANSLATE.y, TRANSLATE.z, 1.0f};

		return result;
	}*/

	static	Matrix	Translate( const Vector& TRANSLATE )
	{
		Matrix result;

		result.matrix[ 0 ] = { 1.0f, 0.0f, 0.0f, TRANSLATE.x };
		result.matrix[ 1 ] = { 0.0f, 1.0f, 0.0f, TRANSLATE.y };
		result.matrix[ 2 ] = { 0.0f, 0.0f, 1.0f, TRANSLATE.z };
		result.matrix[ 3 ] = { 0.0f, 0.0f, 0.0f, 1.0f };

		return result;
	}

	static	Vector	operator *  ( const Vector& V, const Matrix& M )
	{
		/*return {
			V.x * M.matrix[0][0] + V.y * M.matrix[1][0] + V.z * M.matrix[2][0] + V.w * M.matrix[3][0],
			V.x * M.matrix[0][1] + V.y * M.matrix[1][1] + V.z * M.matrix[2][1] + V.w * M.matrix[3][1],
			V.x * M.matrix[0][2] + V.y * M.matrix[1][2] + V.z * M.matrix[2][2] + V.w * M.matrix[3][2],
			V.x * M.matrix[0][3] + V.y * M.matrix[1][3] + V.z * M.matrix[2][3] + V.w * M.matrix[3][3]};*/

			/*
			sum = 0.0f;
			sum += v[0] * M[0][0];
			sum += v[1] * M[1][0];
			sum += v[2] * M[2][0];
			sum += v[3] * M[3][0];
			result[0] = sum;

			sum = 0.0f;
			sum += v[0] * M[0][1];
			sum += v[1] * M[1][1];
			sum += v[2] * M[2][1];
			sum += v[3] * M[3][1];
			result[1] = sum;

			sum = 0.0f;
			sum += v[0] * M[0][2];
			sum += v[1] * M[1][2];
			sum += v[2] * M[2][2];
			sum += v[3] * M[3][2];
			result[2] = sum;

			sum = 0.0f;
			sum += v[0] * M[0][3];
			sum += v[1] * M[1][3];
			sum += v[2] * M[2][3];
			sum += v[3] * M[3][3];
			result[2] = sum;
			*/

		std::array<const float, 4> copy( { V.x,V.y,V.z,V.w } );
		std::array<float, 4> result{};

		float sum = 0.0f;
		for( size_t row = 0u; row < 4u; row++ )
		{
			sum = 0.0f;
			for( size_t col = 0u; col < 4u; col++ )
			{
				sum += copy[ col ] * M.matrix[ row ][ col ];
			}
			result[ row ] = sum;
		}

		return { result[ 0 ],result[ 1 ],result[ 2 ],result[ 3 ] };

		/*std::array<float, 4> copy( { V.x, V.y, V.z, V.w } );
		std::array<float, 4> total( {0.0f,0.0f,0.0f,0.0f} );

		for( size_t i = 0u; i < 4; i++ )
		{
			for( size_t row = 0u; row < 4; row++ )
			{
				for( size_t col = 0u; col < 4; col++ )
				{
					total[row] += copy[col] * M.matrix[col][row];
				}
			}
		}

		return { total[0], total[1], total[2], total[3] };*/
	}
	static	Vector& operator *= ( Vector& v, const Matrix& M )
	{
		v = v * M;

		return v;
	}
}
