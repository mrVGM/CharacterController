#include "MathUtils.h"

#include <math.h>

#include <sstream>

const float math::GetFloatEPS()
{
	return 0.0000000001f;
}

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
	float l = Dot(*this, *this);

	if (l < GetFloatEPS())
	{
		return Vector3{ 0, 0, 0 };
	}

	l = sqrt(l);
	Vector3 res = (1 / l) * (*this);
	return res;
}

math::Matrix math::TransformEuler::ToMatrix() const
{
	float angleX = M_PI * m_rotation.m_coefs[0] / 180;
	float angleY = M_PI * m_rotation.m_coefs[1] / 180;
	float angleZ = M_PI * m_rotation.m_coefs[2] / 180;

	angleX /= 2;
	angleY /= 2;
	angleZ /= 2;

	Vector4 xQ{
		cos(angleX),
		-sin(angleX),
		0,
		0,
	};

	Vector4 yQ{
		cos(angleY),
		0,
		0,
		-sin(angleY),
	};

	Vector4 zQ{
		cos(angleZ),
		0,
		-sin(angleZ),
		0,
	};

	Transform tr {
		m_position,
		zQ ^ yQ ^ xQ,
		m_scale
	};

	return tr.ToMatrix();
}

math::Matrix math::Transform::ToMatrix() const
{
	Matrix translate {
		1,	0,	0,	m_position.m_coefs[0],
		0,	1,	0,	m_position.m_coefs[1],
		0,	0,	1,	m_position.m_coefs[2],
		0,	0,	0,	1
	};

	Vector3 x = m_rotation.Rotate(Vector3{1,0,0});
	Vector3 y = m_rotation.Rotate(Vector3{0,1,0});
	Vector3 z = m_rotation.Rotate(Vector3{0,0,1});

	Matrix rotate {
		x.m_coefs[0],	y.m_coefs[0],	z.m_coefs[0],	0,
		x.m_coefs[1],	y.m_coefs[1],	z.m_coefs[1],	0,
		x.m_coefs[2],	y.m_coefs[2],	z.m_coefs[2],	0,
		0,				0,				0,				1
	};

	Matrix scale {
		m_scale.m_coefs[0],	0,					0,					0,
		0,					m_scale.m_coefs[1],	0,					0,
		0,					0,					m_scale.m_coefs[2],	0,
		0,					0,					0,					1
	};

	return translate * rotate * scale;
}


math::Transform math::Matrix::ToTransform() const
{
	const Matrix& m = *this;

	Vector4 O{ 0, 0, 0, 1 };
	Vector4 X{ 1, 0, 0, 1 };
	Vector4 Y{ 0, 1, 0, 1 };
	Vector4 Z{ 0, 0, 1, 1 };

	Vector4 offset = m * O;

	X = m * X + -1 * offset;
	Y = m * Y + -1 * offset;
	Z = m * Z + -1 * offset;

	Vector3 x = Vector3{ X.m_coefs[0], X.m_coefs[1], X.m_coefs[2] };
	Vector3 y = Vector3{ Y.m_coefs[0], Y.m_coefs[1], Y.m_coefs[2] };
	Vector3 z = Vector3{ Z.m_coefs[0], Z.m_coefs[1], Z.m_coefs[2] };

	Vector3 scale{ sqrt(Dot(x, x)), sqrt(Dot(y, y)), sqrt(Dot(z, z)) };

	if (Dot(x ^ y, z) < 0)
	{
		scale.m_coefs[2] *= -1;
		z = -1 * z;
	}

	x = x.Normalize();
	y = y.Normalize();
	z = z.Normalize();

	auto calcRot = [](const Vector3& from, const Vector3& to) -> Vector4 {
		Vector3 pole = from ^ to;
		pole = pole.Normalize();

		if (Dot(pole, pole) == 0)
		{
			return Vector4{ 1, 0, 0, 0 };
		}

		float c = Dot(from.Normalize(), to.Normalize());

		if (c < -1)
		{
			c = -1;
		}
		if (c > 1)
		{
			c = 1;
		}

		float angle = acos(c);
		angle /= 2;

		return Vector4 {
			cos(angle),
			-sin(angle) * pole.m_coefs[0],
			-sin(angle) * pole.m_coefs[1],
			-sin(angle) * pole.m_coefs[2]
		};
	};

	Vector4 q1 = calcRot(Vector3{ 0, 0, 1 }, z);
	Vector4 q2 = calcRot(q1.Rotate(Vector3{ 1, 0, 0 }), x);

	return Transform{
		Vector3{ offset.m_coefs[0], offset.m_coefs[1], offset.m_coefs[2] },
		q1 ^ q2,
		scale
	};
}

math::Vector3 math::Transform::TransformPoint(const Vector3& p) const
{
	Vector3 res {
		m_scale.m_coefs[0] * p.m_coefs[0],
		m_scale.m_coefs[1] * p.m_coefs[1],
		m_scale.m_coefs[2] * p.m_coefs[2]
	};

	Vector4 qRes{
		0,
		res.m_coefs[0],
		res.m_coefs[1],
		res.m_coefs[2]
	};

	qRes = m_rotation ^ qRes ^ m_rotation.Conjugate();
	
	res = {
		qRes.m_coefs[1],
		qRes.m_coefs[2],
		qRes.m_coefs[3]
	};

	res = res + m_position;

	return res;
}

std::string math::Matrix::ToString() const
{
	std::stringstream ss;

	int index = 0;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			ss << m_coefs[index++] << "\t\t";
		}
		ss << '\n';
	}

	return ss.str();
}

math::Vector4 math::operator^(const Vector4& v1, const Vector4& v2)
{
	float a = v1.m_coefs[0] * v2.m_coefs[0] - v1.m_coefs[1] * v2.m_coefs[1] - v1.m_coefs[2] * v2.m_coefs[2] - v1.m_coefs[3] * v2.m_coefs[3];
	float b = v1.m_coefs[0] * v2.m_coefs[1] + v1.m_coefs[1] * v2.m_coefs[0] + v1.m_coefs[2] * v2.m_coefs[3] - v1.m_coefs[3] * v2.m_coefs[2];
	float c = v1.m_coefs[0] * v2.m_coefs[2] - v1.m_coefs[1] * v2.m_coefs[3] + v1.m_coefs[2] * v2.m_coefs[0] + v1.m_coefs[3] * v2.m_coefs[1];
	float d = v1.m_coefs[0] * v2.m_coefs[3] + v1.m_coefs[1] * v2.m_coefs[2] - v1.m_coefs[2] * v2.m_coefs[1] + v1.m_coefs[3] * v2.m_coefs[0];

	return Vector4{ a, b, c, d };
}

math::Vector4 math::Vector4::Conjugate() const
{
	return Vector4{ m_coefs[0], -m_coefs[1], -m_coefs[2], -m_coefs[3] };
}

math::Vector3 math::Vector4::Rotate(const Vector3& v) const
{
	Vector4 tmp{ 0, v.m_coefs[0], v.m_coefs[1] , v.m_coefs[2] };
	tmp = Conjugate() ^ tmp ^ (*this);

	return Vector3{ tmp.m_coefs[1], tmp.m_coefs[2], tmp.m_coefs[3] };
}

math::Transform math::Lerp(const Transform& t1, const Transform& t2, float alpha)
{
	return Transform{
		(1 - alpha) * t1.m_position + alpha * t2.m_position,
		SLerp(t1.m_rotation, t2.m_rotation, alpha),
		(1 - alpha) * t1.m_scale + alpha * t2.m_scale,
	};
}

math::Vector4 math::SLerp(const Vector4& q1, const Vector4& q2, float alpha)
{
	Vector4 mid = q1.Conjugate() ^ q2;
	Vector3 pole{ mid.m_coefs[1], mid.m_coefs[2], mid.m_coefs[3] };
	pole = pole.Normalize();
	pole = -1 * pole;

	float c = mid.m_coefs[0];
	if (c < -1)
	{
		c = -1;
	}
	if (c > 1)
	{
		c = 1;
	}
	float angle1 = 2 * acos(c);
	float angle2 = 2 * M_PI - angle1;

	float angle = angle1 > angle2 ? -angle2 : angle1;
	angle /= 2;

	angle *= alpha;
	
	return q1 ^ Vector4 {
		cos(angle),
		-sin(angle) * pole.m_coefs[0],
		-sin(angle) * pole.m_coefs[1],
		-sin(angle) * pole.m_coefs[2]
	};
}

#define DOT_IMPL(Type) \
float math::Dot(const Type& v1, const Type& v2)\
{\
	float res = 0;\
	for (int i = 0; i < _countof(v1.m_coefs); ++i)\
	{\
		res += v1.m_coefs[i] * v2.m_coefs[i];\
	}\
	return res;\
}\

DOT_IMPL(Vector2)
DOT_IMPL(Vector3)
DOT_IMPL(Vector4)

#undef DOT_IMPL

#define MULT_IMPL(Type) \
math::Type math::operator*(float coef, const Type& v)\
{\
	Type res;\
	for (int i = 0; i < _countof(v.m_coefs); ++i)\
	{\
		res.m_coefs[i] = coef * v.m_coefs[i];\
	}\
	return res;\
}\

MULT_IMPL(Vector2)
MULT_IMPL(Vector3)
MULT_IMPL(Vector4)

#undef MULT_IMPL

#define SUM_IMPL(Type) \
math::Type math::operator+(const Type& v1, const Type& v2)\
{\
	Type res;\
	for (int i = 0; i < _countof(v1.m_coefs); ++i)\
	{\
		res.m_coefs[i] = v1.m_coefs[i] + v2.m_coefs[i];\
	}\
	return res;\
}\

SUM_IMPL(Vector2)
SUM_IMPL(Vector3)
SUM_IMPL(Vector4)

#undef SUM_IMPL