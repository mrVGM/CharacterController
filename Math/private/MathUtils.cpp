#include "MathUtils.h"

#include <math.h>

const math::Matrix& math::Matrix::GetIdentityMatrix()
{
	static Matrix idMat{
		{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1,
		}
	};

	return idMat;
}

float& math::Matrix::GetCoef(int row, int col)
{
	int index = row * 4 + col;

	return m_coefs[index];
}

math::Matrix math::operator*(const Matrix& m1, const Matrix& m2)
{
	Matrix res;

	for (int r = 0; r < 4; ++r)
	{
		for (int c = 0; c < 4; ++c)
		{
			float coef = 0;
			for (int i = 0; i < 4; ++i)
			{
				coef += const_cast<Matrix&>(m1).GetCoef(r, i) * const_cast<Matrix&>(m2).GetCoef(i, c);
			}

			res.GetCoef(r, c) = coef;
		}
	}

	return res;
}

math::Vector4 math::operator*(const Matrix& m, const Vector4& v)
{
	Vector4 res;
	Matrix& mat = const_cast<Matrix&>(m);

	for (int i = 0; i < 4; ++i)
	{
		res.m_coefs[i] = 0;
		for (int j = 0; j < 4; ++j)
		{
			res.m_coefs[i] += mat.GetCoef(i, j) * v.m_coefs[j];
		}
	}

	return res;
}

math::Matrix math::Matrix::Transpose() const
{
	Matrix* self = const_cast<Matrix*>(this);

	Matrix res;
	for (int r = 0; r < 4; ++r)
	{
		for (int c = 0; c < 4; ++c)
		{
			res.GetCoef(c, r) = self->GetCoef(r, c);
		}
	}

	return res;
}

math::Matrix math::Matrix::FlipYZAxis() const
{
	Matrix changeBasis = Matrix::GetIdentityMatrix();
	changeBasis.GetCoef(1, 2) = 1;
	changeBasis.GetCoef(1, 1) = 0;
	changeBasis.GetCoef(2, 1) = 1;
	changeBasis.GetCoef(2, 2) = 0;

	return changeBasis * (*this) * changeBasis;
}

math::Vector3 math::operator^(const math::Vector3& v1, const math::Vector3& v2)
{
	math::Vector3 res;
	res.m_coefs[0] =  v1.m_coefs[1] * v2.m_coefs[2] - v1.m_coefs[2] * v2.m_coefs[1];
	res.m_coefs[1] = -v1.m_coefs[0] * v2.m_coefs[2] + v1.m_coefs[2] * v2.m_coefs[0];
	res.m_coefs[2] =  v1.m_coefs[0] * v2.m_coefs[1] - v1.m_coefs[1] * v2.m_coefs[0];

	return res;
}

math::Vector3 math::Vector3::Normalize() const
{
	float l = 0;

	for (int i = 0; i < 3; ++i)
	{
		l += m_coefs[i] * m_coefs[i];
	}

	if (l < 0.00001)
	{
		return Vector3{ 0, 0, 0 };
	}

	l = sqrt(l);
	Vector3 res = (1 / l) * (*this);
	return res;
}

math::Vector3 math::operator*(float coef, const math::Vector3& v)
{
	Vector3 res{ coef * v.m_coefs[0], coef * v.m_coefs[1], coef * v.m_coefs[2] };
	return res;
}

math::Vector3 math::operator+(const math::Vector3& v1, const math::Vector3& v2)
{
	Vector3 res{ v1.m_coefs[0] + v2.m_coefs[0], v1.m_coefs[1] + v2.m_coefs[1], v1.m_coefs[2] + v2.m_coefs[2] };
	return res;
}


math::Matrix math::Transform::ToMatrix() const
{
	Matrix scale = {
		{
			m_scale.m_coefs[0],	0,					0,					0,
			0,					m_scale.m_coefs[1],	0,					0,
			0,					0,					m_scale.m_coefs[2],	0,
			0,					0,					0,					1
		}
	};

	Matrix translate = Matrix::GetIdentityMatrix();

	translate.GetCoef(0, 3) = m_position.m_coefs[0];
	translate.GetCoef(1, 3) = m_position.m_coefs[1];
	translate.GetCoef(2, 3) = m_position.m_coefs[2];

	return translate * scale;
}