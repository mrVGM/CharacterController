#pragma once

#include <string>

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

		std::string ToString() const;
	};

	struct TransformEuler
	{
		Vector3 m_position;
		Vector3 m_rotation;
		Vector3 m_scale;

		Matrix ToMatrix() const;
	};

	Matrix operator*(const Matrix& m1, const Matrix& m2);
	Vector4 operator*(const Matrix& m, const Vector4& v);

	Vector3 operator^(const Vector3& v1, const Vector3& v2);

#define SUM_DEF(Type)\
	Type operator+(const Type& v1, const Type& v2);\

	SUM_DEF(Vector2)
	SUM_DEF(Vector3)
	SUM_DEF(Vector4)
	
#undef SUM_DEF

#define MULT_DEF(Type)\
	Type operator*(float coef, const Type& v);\

	MULT_DEF(Vector2)
	MULT_DEF(Vector3)
	MULT_DEF(Vector4)

#undef SUM_DEF


#define DOT_DEF(Type)\
	float Dot(const Type& v1, const Type& v2);\

	DOT_DEF(Vector2)
	DOT_DEF(Vector3)
	DOT_DEF(Vector4)

#undef DOT_DEF
}