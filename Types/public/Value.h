#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include <variant>

class Value
{
private:
	void Initialize(const TypeDef& type, const CompositeValue* outer);

public:
	typedef std::variant<bool, int, float, std::string, const TypeDef*, CompositeValue*> ValuePayload;

	ValuePayload m_payload;

	const TypeDef* m_type = nullptr;
	size_t m_outer = 0;

	Value(const TypeDef& type, const CompositeValue* outer);
	Value(const Value& other) = delete;

	Value& operator=(const Value& other);
	~Value();

	void AssignObject(ObjectValue* object);

	template<typename T>
	T Get() const
	{
		return std::get<T>(m_payload);
	}

	template<typename T>
	T GetValue() const
	{
		CompositeValue* compositeValue = std::get<CompositeValue*>(m_payload);
		return static_cast<T>(compositeValue);
	}

	template<typename T>
	T GetType() const
	{
		const TypeDef* typeValue = std::get<const TypeDef*>(m_payload);
		return static_cast<T>(typeValue);
	}
};
