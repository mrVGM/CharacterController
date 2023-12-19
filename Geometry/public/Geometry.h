#pragma once

#include "MathUtils.h"

namespace geo
{
	struct MeshVertex
	{
		math::Vector3 m_position;
		math::Vector3 m_normal;
		math::Vector2 m_uv;
	};

	void Boot();
}