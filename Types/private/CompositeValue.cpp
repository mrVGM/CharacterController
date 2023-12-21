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
			if (TypeDef::IsA(outer->GetTypeDef(), ReferenceTypeDef::GetTypeDef()))
			{
				return static_cast<const ObjectValue*>(outer);
			}

			const CopyValue* copyValue = static_cast<const CopyValue*>(outer);
			outer = copyValue->GetOuter();
		}

		return nullptr;
	}
}

const CompositeTypeDef& CompositeValue::GetTypeDef() const
{
	return *m_typeDef;
}

CompositeValue::CompositeValue(const CompositeTypeDef& typeDef) :
	m_typeDef(&typeDef)
{
}

CopyValue::CopyValue(const ValueTypeDef& typeDef, const CompositeValue* outer) :
	CompositeValue(typeDef),
	m_outer(outer)
{
}

const CompositeValue* CopyValue::GetOuter() const
{
	return m_outer;
}

ObjectValue::ObjectValue(const ReferenceTypeDef& typeDef) :
	CompositeValue(typeDef)
{
	ObjectValueContainer::GetContainer().Register(this);
}

ObjectValue::~ObjectValue()
{
	ObjectValueContainer::GetContainer().Unregister(this);
}
