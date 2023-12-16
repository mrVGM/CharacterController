#pragma once

#include "Value.h"
#include "CompositeValue.h"

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
