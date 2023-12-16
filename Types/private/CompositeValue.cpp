#pragma once

#include "CompositeValue.h"
#include "ListDef.h"

#include "ObjectValueContainer.h"

namespace
{
	const ObjectValue* GetOuterObject(const CompositeValue* outer)
	{
		while (outer)
		{
			if (outer->GetTypeDef().IsA(ReferenceTypeDef::GetReferenceTypeDef()))
			{
				return static_cast<const ObjectValue*>(outer);
			}

			outer = outer->GetOuter();
		}

		return nullptr;
	}
}

const CompositeTypeDef& CompositeValue::GetTypeDef() const
{
	return *m_typeDef;
}

const CompositeValue* CompositeValue::GetOuter() const
{
	return m_outer;
}

CompositeValue::CompositeValue(const CompositeTypeDef& typeDef, const CompositeValue* outer) :
	m_typeDef(&typeDef),
	m_outer(outer)
{
}

CopyValue::CopyValue(const CompositeTypeDef& typeDef, const CompositeValue* outer) :
	CompositeValue(typeDef, outer)
{
}

ObjectValue::ObjectValue(const CompositeTypeDef& typeDef, const CompositeValue* outer) :
	CompositeValue(typeDef, outer)
{
	ObjectValueContainer::GetContainer().Register(this);
}

ObjectValue::~ObjectValue()
{
	ObjectValueContainer::GetContainer().Unregister(this);
}
