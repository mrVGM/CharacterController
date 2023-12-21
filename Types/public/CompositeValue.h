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
	const CompositeValue* m_outer = nullptr;

public:
	CopyValue(const ValueTypeDef& typeDef, const CompositeValue* outer);

	virtual void Copy(const CopyValue& src) = 0;
	const CompositeValue* GetOuter() const;
};
