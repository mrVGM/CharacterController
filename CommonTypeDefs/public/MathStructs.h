#pragma once

#include "CompositeTypeDef.h"
#include "CompositeValue.h"

#include "Value.h"

namespace common
{
	class Vector4TypeDef : public ValueTypeDef
	{
		TYPE_DEF_BODY(Vector4TypeDef)

	public:
		TypeProperty m_x;
		TypeProperty m_y;
		TypeProperty m_z;
		TypeProperty m_w;

		Vector4TypeDef();
		virtual ~Vector4TypeDef();

		void Construct(Value& container) const override;
	};

	class Vector3TypeDef : public ValueTypeDef
	{
		TYPE_DEF_BODY(Vector3TypeDef)

	public:
		TypeProperty m_x;
		TypeProperty m_y;
		TypeProperty m_z;

		Vector3TypeDef();
		virtual ~Vector3TypeDef();

		void Construct(Value& container) const override;
	};

	class TransformTypeDef : public ValueTypeDef
	{
		TYPE_DEF_BODY(TransformTypeDef)

	public:
		TypeProperty m_position;
		TypeProperty m_rotation;
		TypeProperty m_scale;

		TransformTypeDef();
		virtual ~TransformTypeDef();

		void Construct(Value& container) const override;
	};

	class Vector4Value : public CopyValue
	{
	public:
		Value m_x;
		Value m_y;
		Value m_z;
		Value m_w;

		Vector4Value(size_t outer);
		~Vector4Value();

		virtual void Copy(const CopyValue& src) override;
	};

	class Vector3Value : public CopyValue
	{
	public:
		Value m_x;
		Value m_y;
		Value m_z;

		Vector3Value(size_t outer);
		~Vector3Value();

		virtual void Copy(const CopyValue& src) override;
	};

	class TransformValue : public CopyValue
	{
	public:
		Value m_position;
		Value m_rotation;
		Value m_scale;

		TransformValue(size_t outer);
		~TransformValue();

		virtual void Copy(const CopyValue& src) override;
	};
}