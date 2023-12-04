#pragma once

#include "ListTypeDef.h"
#include "CompositeTypeDef.h"
#include "Object.h"

#include <list>

template <typename T>
class GenericList
{
private:
	const ObjectValue* m_container = nullptr;
	const ListTypeDef& m_typeDef;
	std::list<T> m_list;

	bool IsListOfObjects() const
	{
		return m_typeDef.IsA(ReferenceTypeDef::GetTypeDef());
	}

public:
	inline GenericList(const ListTypeDef& listTypeDef, const ObjectValue* container) :
		m_typeDef(listTypeDef),
		m_container(container)
	{
	}

	T& EmplaceBack()
	{
		T& newElem = m_list.emplace_back();
		
		if (IsListOfObjects())
		{
			ObjectValue& obj = static_cast<ObjectValue&>(newElem);
			obj.SetContainer(m_container);
		}

		return newElem;
	}

	GenericList& operator=(const GenericList& other)
	{
		if (!IsListOfObjects())
		{
			m_list = other.m_list;
			return *this;
		}

		m_list.clear();
		for (auto it = other.m_list.begin(); it != other.m_list.end(); ++it)
		{
			ObjectValue& val = static_cast<ObjectValue&>(m_list.emplace_back());
			val.SetContainer(m_container);

			val = static_cast<ObjectValue&>(*it);
		}

		return *this;
	}
};

