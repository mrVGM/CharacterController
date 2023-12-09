#pragma once

#include "CompositeTypeDef.h"

#include <variant>

class CompositeValue
{
private:
	const CompositeTypeDef& m_typeDef;
	const CompositeValue* m_outer = nullptr;

public:
	CompositeValue(const CompositeTypeDef& typeDef, const CompositeValue* outer);

	const CompositeTypeDef& GetTypeDef() const;
	virtual void Copy(const CompositeValue& src);
};

class ObjectValue : public CompositeValue
{
};


template <typename T>
class StructValue : public CompositeValue
{
public:
	T m_payload;

	StructValue(const CompositeTypeDef& typeDef, const CompositeValue* outer) :
		CompositeValue(typeDef, outer)
	{
	}

	virtual void Copy(const CompositeValue& src) override
	{
		const StructValue& srcStruct = static_cast<StructValue&>(src);

		m_payload = srcStruct.m_payload;
	}
};

class Value
{
private:
	friend class ValueList;

	bool m_initialized = false;
	Value();

	void Initialize(const TypeDef& type, const CompositeValue* outer);

public:
	typedef std::variant<bool, int, float, std::string, CompositeValue*> ValuePayload;

	ValuePayload m_payload;

	const CompositeValue* m_outer = nullptr;
	const TypeDef* m_type = nullptr;

	Value(const TypeDef& type, const CompositeValue* outer);
	Value(const Value& other) = delete;

	Value& operator=(const Value& other);
	~Value();
};


class ValueList : public CompositeValue
{
public:
	std::list<Value> m_values;

	ValueList(const CompositeTypeDef& typeDef, const CompositeValue* outer);

	virtual void Copy(const CompositeValue& src) override;
};