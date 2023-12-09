#pragma once

#include "CompositeValue.h"

const CompositeTypeDef& CompositeValue::GetTypeDef() const
{
	return m_typeDef;
}

CompositeValue::CompositeValue(const CompositeTypeDef& typeDef, const CompositeValue* outer) :
	m_typeDef(typeDef),
	m_outer(outer)
{
}

void CompositeValue::Copy(const CompositeValue& src)
{
}


Value::Value()
{
}

void Value::Initialize(const TypeDef& type, const CompositeValue* outer)
{
	m_type = &type;
	m_outer = outer;
	m_initialized = true;
}

Value::Value(const TypeDef& type, const CompositeValue* outer)
{
	Initialize(type, outer);
}

Value& Value::operator=(const Value& other)
{
	if (!m_initialized)
	{
		throw "Not Initialized!";
	}

	if (m_type->IsA(ValueTypeDef::GetTypeDef()))
	{
		CompositeValue* self = std::get<CompositeValue*>(m_payload);
		CompositeValue* oth = std::get<CompositeValue*>(other.m_payload);

		self->Copy(*oth);
		return *this;
	}

	if (m_type->IsA(ReferenceTypeDef::GetTypeDef()))
	{
		throw "Not Implemented!";
	}

	m_payload = other.m_payload;

	return *this;
}

Value::~Value()
{
	throw "Not Implamented!";
}

ValueList::ValueList(const CompositeTypeDef& typeDef, const CompositeValue* outer) :
	CompositeValue(typeDef, outer)
{
}

void ValueList::Copy(const CompositeValue& src)
{
	throw "Not Implemented!";
}