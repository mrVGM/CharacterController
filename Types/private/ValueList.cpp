#pragma once

#include "ValueList.h"
#include "ListDef.h"

#include "ObjectValueContainer.h"

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