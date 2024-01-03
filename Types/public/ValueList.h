#pragma once

#include "CompositeValue.h"

class ListDef;

class ValueList : public CopyValue
{
	struct ListElem
	{
		Value m_value;
		ListElem* m_next;
	};

	ListElem* m_first = nullptr;
	ListElem* m_last = nullptr;

public:
	class Iterator
	{
		friend class ValueList;
	private:
		ListElem* m_cur = nullptr;

	public:
		operator bool() const;
		Value& operator*() const;
		void operator++();
	};

	ValueList(const ListDef& typeDef, size_t outer);
	virtual ~ValueList();

	virtual void Copy(const CopyValue& src) override;

	Value& EmplaceBack();
	Iterator GetIterator() const;
	void Clear();
};