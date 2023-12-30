#pragma once

#include "Value.h"
#include "PrimitiveTypes.h"
#include "CompositeValue.h"

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

void Value::Initialize(const TypeDef& type, const CompositeValue* outer)
{
	m_type = &type;
	m_outer = outer;

	if (TypeDef::IsA(*m_type, BoolTypeDef::GetTypeDef()))
	{
		m_payload = false;
		return;
	}

	if (TypeDef::IsA(*m_type, IntTypeDef::GetTypeDef()))
	{
		m_payload = 0;
		return;
	}

	if (TypeDef::IsA(*m_type, FloatTypeDef::GetTypeDef()))
	{
		m_payload = 0.0f;
		return;
	}

	if (TypeDef::IsA(*m_type, StringTypeDef::GetTypeDef()))
	{
		m_payload = "";
		return;
	}

	if (TypeDef::IsA(*m_type, GenericTypeDef::GetTypeDef()))
	{
		m_payload = static_cast<const TypeDef*>(nullptr);
		return;
	}

	if (TypeDef::IsA(*m_type, ValueTypeDef::GetTypeDef()))
	{
		const ValueTypeDef& type = static_cast<const ValueTypeDef&>(*m_type);
		type.Construct(*this);
		return;
	}

	if (TypeDef::IsA(*m_type, ReferenceTypeDef::GetTypeDef()))
	{
		m_payload = static_cast<CompositeValue*>(nullptr);
		return;
	}
}

Value::Value(const TypeDef& type, const CompositeValue* outer)
{
	Initialize(type, outer);
}

Value& Value::operator=(const Value& other)
{
	if (TypeDef::IsA(*m_type, ValueTypeDef::GetTypeDef()))
	{
		CopyValue* self = GetValue<CopyValue*>();
		CopyValue* oth = other.GetValue<CopyValue*>();

		self->Copy(*oth);
		return *this;
	}

	if (TypeDef::IsA(*m_type, ReferenceTypeDef::GetTypeDef()))
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
	if (TypeDef::IsA(*m_type, ValueTypeDef::GetTypeDef()))
	{
		CopyValue* copyValue = GetValue<CopyValue*>();
		delete copyValue;
		return;
	}

	if (TypeDef::IsA(*m_type, ReferenceTypeDef::GetTypeDef()))
	{
		ObjectValue* objectValue = GetValue<ObjectValue*>();
		AssignObject(nullptr);
		return;
	}
}

void Value::AssignObject(ObjectValue* object)
{
	if (!TypeDef::IsA(*m_type, ReferenceTypeDef::GetTypeDef()))
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

