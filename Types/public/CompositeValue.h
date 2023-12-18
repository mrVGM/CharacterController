#pragma once

#include "GC.h"
#include "Value.h"

#include <variant>

class CompositeValue
{
	friend class AssetTypeDef;
private:
	const CompositeTypeDef* m_typeDef = nullptr;
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
	virtual ~ObjectValue();
};

class CopyValue : public CompositeValue
{
public:
	CopyValue(const CompositeTypeDef& typeDef, const CompositeValue* outer);

	virtual void Copy(const CopyValue& src) = 0;
};
