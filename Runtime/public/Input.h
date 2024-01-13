#pragma once

#include "TickUpdater.h"
#include "Actor.h"

#include "Animation.h"

#include "MultiLoader.h"

#include <set>

namespace runtime
{
	struct Input
	{
		double m_mouseAxis[2] = {};
		std::set<WPARAM> m_keysDown;

		bool m_lbmDown = false;
		bool m_rbmDown = false;
	};

	Input& GetInput();
}