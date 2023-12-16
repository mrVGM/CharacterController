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

ValueList::ValueList(const ListDef& typeDef, const CompositeValue* outer) :
	CopyValue(typeDef, outer)
{
}

ValueList::~ValueList()
{
	Clear();
}

void ValueList::Copy(const CopyValue& src)
{
	if (&src.GetTypeDef() != &GetTypeDef())
	{
		throw "Incompatible list types!";
	}

	const ValueList& srcList = static_cast<const ValueList&>(src);
	Clear();

	for (Iterator it = srcList.GetIterator(); it; ++it)
	{
		Value& tmp = EmplaceBack();
		tmp = *it;
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

Value& ValueList::EmplaceBack()
{
	const ListDef& listDef = static_cast<const ListDef&>(GetTypeDef());

	ListElem* newElem = new ListElem{ Value(listDef.m_templateDef, this), nullptr };
	if (!m_first)
	{
		m_first = newElem;
		m_last = newElem;
		return newElem->m_value;
	}

	m_last->m_next = newElem;
	m_last = newElem;
	return newElem->m_value;
}

ValueList::Iterator ValueList::GetIterator() const
{
	Iterator res;
	res.m_cur = m_first;

	return res;
}

ValueList::Iterator::operator bool() const
{
	return m_cur;
}

Value& ValueList::Iterator::operator*() const
{
	return m_cur->m_value;
}

void ValueList::Iterator::operator++()
{
	m_cur = m_cur->m_next;
}

void ValueList::Clear()
{
	std::list<ListElem*> elements;

	ListElem* cur = m_first;
	while (cur)
	{
		elements.push_back(cur);
		cur = cur->m_next;
	}

	for (auto it = elements.begin(); it != elements.end(); ++it)
	{
		delete *it;
	}

	m_first = nullptr;
	m_last = nullptr;
}