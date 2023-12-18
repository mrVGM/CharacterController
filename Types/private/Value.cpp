#pragma once

#include "Value.h"
#include "CompositeValue.h"

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

void Value::Initialize(const TypeDef& type, const CompositeValue* outer)
{
	m_type = &type;
	m_outer = outer;

	if (m_type->IsA(ValueTypeDef::GetTypeDef()))
	{
		const ValueTypeDef& type = static_cast<const ValueTypeDef&>(*m_type);
		type.Construct(*this);
	}

	if (m_type->IsA(ReferenceTypeDef::GetReferenceTypeDef()))
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
		CopyValue* self = GetValue<CopyValue*>();
		CopyValue* oth = other.GetValue<CopyValue*>();

		self->Copy(*oth);
		return *this;
	}

	if (m_type->IsA(ReferenceTypeDef::GetReferenceTypeDef()))
	{
		ObjectValue* obj = other.GetValue<ObjectValue*>();
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
		CopyValue* copyValue = GetValue<CopyValue*>();
		delete copyValue;
		return;
	}

	if (m_type->IsA(ReferenceTypeDef::GetReferenceTypeDef()))
	{
		ObjectValue* objectValue = GetValue<ObjectValue*>();
		AssignObject(nullptr);
		return;
	}
}

void Value::AssignObject(ObjectValue* object)
{
	if (!m_type->IsA(ReferenceTypeDef::GetReferenceTypeDef()))
	{
		throw "Can't Assign Object!";
	}

	const ObjectValue* outer = GetOuterObject(m_outer);
	const ObjectValue* cur = GetValue<ObjectValue*>();

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

