#pragma once

#include "CompositeTypeDef.h"

#include <variant>

class Value
{
private:
	void Initialize(const TypeDef& type, const CompositeValue* outer);

public:
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
