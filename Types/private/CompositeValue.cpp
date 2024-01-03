#pragma once

#include "CompositeValue.h"
#include "ListDef.h"

#include "ObjectValueContainer.h"

namespace
{
	size_t GetOuterObjectId(const CompositeValue* outer)
	{
		if (!outer)
		{
			return 0;
		}

		if (TypeDef::IsA(outer->GetTypeDef(), ReferenceTypeDef::GetTypeDef()))
		{
			const ObjectValue* tmp = static_cast<const ObjectValue*>(outer);
			return tmp->GetId();
		}

		const CopyValue* copyValue = static_cast<const CopyValue*>(outer);
		return copyValue->GetOuter();
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

CompositeValue::~CompositeValue()
{
}

CopyValue::CopyValue(const ValueTypeDef& typeDef, size_t outer) :
	CompositeValue(typeDef),
	m_outer(outer)
{
}

CopyValue::~CopyValue()
{
}

size_t CopyValue::GetOuter() const
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
