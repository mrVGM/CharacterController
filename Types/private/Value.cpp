#pragma once

#include "Value.h"
#include "CompositeValue.h"

namespace
{
	const ObjectValue* GetOuterObject(const CompositeValue* outer)
	{
		while (outer)
		{
			if (outer->GetTypeDef().IsA(ReferenceTypeDef::GetTypeDef()))
			{
				return static_cast<const ObjectValue*>(outer);
			}

			outer = outer->GetOuter();
		}

		return nullptr;
	}
}

void Value::Initialize(const TypeDef& type, const CompositeValue* outer)
{
	m_type = &type;
	m_outer = outer;

	if (m_type->IsA(ValueTypeDef::GetTypeDef()))
	{
		const ValueTypeDef& type = static_cast<const ValueTypeDef&>(*m_type);
		type.Construct(*this);
	}

	if (m_type->IsA(ReferenceTypeDef::GetTypeDef()))
	{
		m_payload = static_cast<CompositeValue*>(nullptr);
	}
}

Value::Value(const TypeDef& type, const CompositeValue* outer)
{
	Initialize(type, outer);
}

Value& Value::operator=(const Value& other)
{
	if (m_type->IsA(ValueTypeDef::GetTypeDef()))
	{
		CopyValue* self = static_cast<CopyValue*>(std::get<CompositeValue*>(m_payload));
		CopyValue* oth = static_cast<CopyValue*>(std::get<CompositeValue*>(other.m_payload));

		self->Copy(*oth);
		return *this;
	}

	if (m_type->IsA(ReferenceTypeDef::GetTypeDef()))
	{
		ObjectValue* obj = static_cast<ObjectValue*>(std::get<CompositeValue*>(other.m_payload));
		AssignObject(obj);
		return *this;
	}

	m_payload = other.m_payload;

	return *this;
}

Value::~Value()
{
	if (m_type->IsA(ValueTypeDef::GetTypeDef()))
	{
		CopyValue* copyValue = static_cast<CopyValue*>(std::get<CompositeValue*>(m_payload));
		delete copyValue;
		return;
	}

	if (m_type->IsA(ReferenceTypeDef::GetTypeDef()))
	{
		ObjectValue* objectValue = static_cast<ObjectValue*>(std::get<CompositeValue*>(m_payload));
		AssignObject(nullptr);
		return;
	}
}

void Value::AssignObject(ObjectValue* object)
{
	if (!m_type->IsA(ReferenceTypeDef::GetTypeDef()))
	{
		throw "Can't Assign Object!";
	}

	const ObjectValue* outer = GetOuterObject(m_outer);
	const ObjectValue* cur = static_cast<ObjectValue*>(std::get<CompositeValue*>(m_payload));

	if (outer)
	{
		if (object)
		{
			gc::AddLink(object, outer);
		}
		if (cur)
		{
			gc::RemoveLink(cur, outer);
		}
	}
	else
	{
		if (object)
		{
			gc::IncrementRefs(object);
		}
		if (cur)
		{
			gc::DecrementRefs(cur);
		}
	}
	m_payload = object;
}

