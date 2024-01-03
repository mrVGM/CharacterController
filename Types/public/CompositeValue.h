#pragma once

#include "GC.h"
#include "Value.h"

#include <variant>

class CompositeValue
{
	friend class AssetTypeDef;
private:
	const CompositeTypeDef* m_typeDef = nullptr;

public:
	CompositeValue(const CompositeTypeDef& typeDef);
	virtual ~CompositeValue();

	const CompositeTypeDef& GetTypeDef() const;
};

class ObjectValue : public CompositeValue, public gc::ManagedObject
{
public:
	ObjectValue(const ReferenceTypeDef& typeDef);
	virtual ~ObjectValue();
};

class CopyValue : public CompositeValue
{
private:
	size_t m_outer = 0;

public:
	CopyValue(const ValueTypeDef& typeDef, size_t outer);
	virtual ~CopyValue();

	virtual void Copy(const CopyValue& src) = 0;
	size_t GetOuter() const;
};
