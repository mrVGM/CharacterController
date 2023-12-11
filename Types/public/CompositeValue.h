#pragma once

#include "CompositeTypeDef.h"
#include "GC.h"

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

class Value
{
private:
	bool m_initialized = false;

public:
	Value();
	void Initialize(const TypeDef& type, const CompositeValue* outer);

	typedef std::variant<bool, int, float, std::string, const TypeDef*, CompositeValue*> ValuePayload;

	ValuePayload m_payload;

	const CompositeValue* m_outer = nullptr;
	const TypeDef* m_type = nullptr;

	Value(const TypeDef& type, const CompositeValue* outer);
	Value(const Value& other) = delete;

	Value& operator=(const Value& other);
	~Value();

	void AssignObject(ObjectValue* object);
};

class ListDef;

class ValueList : public CopyValue
{
public:
	std::list<Value> m_values;

	ValueList(const ListDef& typeDef, const CompositeValue* outer);

	virtual void Copy(const CopyValue& src) override;
};