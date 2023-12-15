#pragma once

#include "CompositeValue.h"
#include "GenericListDef.h"

#include "ObjectValueContainer.h"

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

const CompositeTypeDef& CompositeValue::GetTypeDef() const
{
	return m_typeDef;
}

const CompositeValue* CompositeValue::GetOuter() const
{
	return m_outer;
}

CompositeValue::CompositeValue(const CompositeTypeDef& typeDef, const CompositeValue* outer) :
	m_typeDef(typeDef),
	m_outer(outer)
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

	if (m_type->IsA(ValueTypeDef::GetTypeDef()))
	{
		const ValueTypeDef& type = static_cast<const ValueTypeDef&>(*m_type);
		type.Construct(*this);
	}
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

ValueList::ValueList(const ListDef& typeDef, const CompositeValue* outer) :
	CopyValue(typeDef, outer)
{
}

void ValueList::Copy(const CopyValue& src)
{
	m_values.clear();
	const ValueList& srcList = static_cast<const ValueList&>(src);

	const ListDef& type = GetTypeDef();
	for (int i = 0; i < srcList.m_values.size(); ++i)
	{
		Value& cur = m_values.emplace_back();
		cur.Initialize(type.m_templateDef, this);
	}

	auto srcIt = srcList.m_values.begin();
	auto it = m_values.begin();

	for (; it != m_values.end(); ++it)
	{
		*it = *srcIt;
		++srcIt;
	}
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