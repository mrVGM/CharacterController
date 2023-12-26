#include "MathUtils.h"

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