#pragma once

namespace math
{
	struct Vector2
	{
		float m_coefs[2];
	};

	struct Vector3
	{
		float m_coefs[3];
	};

	struct Vector4
	{
		float m_coefs[3];
	};

	struct Matrix
	{
		float m_coefs[16];

		static const Matrix& GetIdentityMatrix();
		float& GetCoef(int row, int col);

		Matrix Transpose() const;
	};

	Matrix operator*(const Matrix& m1, const Matrix& m2);
}