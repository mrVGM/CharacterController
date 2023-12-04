#pragma once

#include "ListTypeDef.h"

#include <list>

template <typename T>
class GenericList
{
private:
	const ListTypeDef& m_typeDef;
	std::list<T> m_list;

public:
	inline GenericList(const ListTypeDef& listTypeDef) :
		m_typeDef(listTypeDef)
	{
	}

	inline std::list<T> GetList()
	{
		return m_list;
	}
};

