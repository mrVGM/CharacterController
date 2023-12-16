#pragma once

#include "CompositeTypeDef.h"
#include "GC.h"
#include "Value.h"

#include <variant>

class CompositeValue
{
private:
	const CompositeTypeDef& m_typeDef;
	const CompositeValue* m_outer = nullptr;

public:
	CompositeValue(const CompositeTypeDef& typeDef, const CompositeValue* outer);

	const CompositeTypeDef& GetTypeDef() const;
	const CompositeValue* GetOuter() const;
};

class ObjectValue : public CompositeValue, public gc::ManagedObject
{
public:
	ObjectValue(const CompositeTypeDef& typeDef, const CompositeValue* outer);
	~ObjectValue();
};

class CopyValue : public CompositeValue
{
public:
	CopyValue(const CompositeTypeDef& typeDef, const CompositeValue* outer);

	virtual void Copy(const CopyValue& src) = 0;
};


template <typename T>
class StructValue : public CopyValue
{
public:
	T m_payload;

	StructValue(const CompositeTypeDef& typeDef, const CompositeValue* outer) :
		CopyValue(typeDef, outer)
	{
	}

	virtual void Copy(const CopyValue& src) override
	{
		const StructValue& srcStruct = static_cast<StructValue&>(src);

		m_payload = srcStruct.m_payload;
	}
};

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

	ValueList(const ListDef& typeDef, const CompositeValue* outer);
	~ValueList();

	virtual void Copy(const CopyValue& src) override;

	Value& EmplaceBack();
	Iterator GetIterator() const;
	void Clear();
};