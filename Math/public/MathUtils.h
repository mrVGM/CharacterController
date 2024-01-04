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

		Vector3 Normalize() const;
	};

	struct Vector4
	{
		float m_coefs[4];
	};

	struct Matrix
	{
		float m_coefs[16];

		static const Matrix& GetIdentityMatrix();
		float& GetCoef(int row, int col);

		Matrix Transpose() const;
		Matrix FlipYZAxis() const;
	};

	struct Transform
	{
		Vector3 m_position;
		Vector3 m_rotation;
		Vector3 m_scale;

		Matrix ToMatrix() const;
	};

	Matrix operator*(const Matrix& m1, const Matrix& m2);
	Vector4 operator*(const Matrix& m, const Vector4& v);

	Vector3 operator^(const Vector3& v1, const Vector3& v2);
	Vector3 operator*(float coef, const Vector3& v);
	Vector3 operator+(const Vector3& v1, const Vector3& v2);
}