#include "MathStructs.h"

#include "PrimitiveTypes.h"

#include "MathUtils.h"

namespace
{
	BasicObjectContainer<common::Vector4TypeDef> m_vector4;
	BasicObjectContainer<common::Vector3TypeDef> m_vector3;
	BasicObjectContainer<common::TransformTypeDef> m_transform;
}

const common::Vector4TypeDef& common::Vector4TypeDef::GetTypeDef()
{
	if (!m_vector4.m_object)
	{
		m_vector4.m_object = new common::Vector4TypeDef();
	}

	return *m_vector4.m_object;
}

const common::Vector3TypeDef& common::Vector3TypeDef::GetTypeDef()
{
	if (!m_vector3.m_object)
	{
		m_vector3.m_object = new common::Vector3TypeDef();
	}

	return *m_vector3.m_object;
}

const common::TransformTypeDef& common::TransformTypeDef::GetTypeDef()
{
	if (!m_transform.m_object)
	{
		m_transform.m_object = new common::TransformTypeDef();
	}

	return *m_transform.m_object;
}

common::Vector4TypeDef::Vector4TypeDef() :
	ValueTypeDef(&ValueTypeDef::GetTypeDef(), "8FDB92E9-F844-4042-B411-ECEE7AC19AA4"),
	m_x("A6C8E1A3-EE94-4E0D-A3C2-540240EA1B54", FloatTypeDef::GetTypeDef()),
	m_y("49A0464B-0F5E-4A99-B256-BC3AC827B963", FloatTypeDef::GetTypeDef()),
	m_z("1EF6F184-66A2-4EA8-A7B3-C7E8F2E59C06", FloatTypeDef::GetTypeDef()),
	m_w("9E079E83-1D3D-45B3-AC3A-0713420ABF55", FloatTypeDef::GetTypeDef())
{
	{
		m_x.m_name = "X";
		m_x.m_category = "";
		m_x.m_getValue = [](CompositeValue* val) -> Value& {
			Vector4Value* value = static_cast<Vector4Value*>(val);
			return value->m_x;
		};
		m_properties[m_x.GetId()] = &m_x;
	}

	{
		m_y.m_name = "Y";
		m_y.m_category = "";
		m_y.m_getValue = [](CompositeValue* val) -> Value& {
			Vector4Value* value = static_cast<Vector4Value*>(val);
			return value->m_y;
		};
		m_properties[m_y.GetId()] = &m_y;
	}

	{
		m_z.m_name = "Z";
		m_z.m_category = "";
		m_z.m_getValue = [](CompositeValue* val) -> Value& {
			Vector4Value* value = static_cast<Vector4Value*>(val);
			return value->m_z;
		};
		m_properties[m_z.GetId()] = &m_z;
	}

	{
		m_w.m_name = "W";
		m_w.m_category = "";
		m_w.m_getValue = [](CompositeValue* val) -> Value& {
			Vector4Value* value = static_cast<Vector4Value*>(val);
			return value->m_w;
		};
		m_properties[m_w.GetId()] = &m_w;
	}

	m_name = "Vector 4";
	m_category = "Math";
}

common::Vector4TypeDef::~Vector4TypeDef()
{
}

void common::Vector4TypeDef::Construct(Value& container) const
{
	Vector4Value* tmp = new Vector4Value(container.m_outer);
	container.m_payload = tmp;
}



common::Vector3TypeDef::Vector3TypeDef() :
	ValueTypeDef(&ValueTypeDef::GetTypeDef(), "46A3F7A0-33EA-495B-9716-36F225CDDBDB"),
	m_x("827F53AD-67EF-4C69-B89B-7F595A2E6EEF", FloatTypeDef::GetTypeDef()),
	m_y("B6BFCE7E-3BCF-496A-9903-B6C86F8E372A", FloatTypeDef::GetTypeDef()),
	m_z("13B05A9B-EAD9-4D63-92D6-429A97C6C902", FloatTypeDef::GetTypeDef())
{
	{
		m_x.m_name = "X";
		m_x.m_category = "";
		m_x.m_getValue = [](CompositeValue* val) -> Value& {
			Vector3Value* value = static_cast<Vector3Value*>(val);
			return value->m_x;
		};
		m_properties[m_x.GetId()] = &m_x;
	}

	{
		m_y.m_name = "Y";
		m_y.m_category = "";
		m_y.m_getValue = [](CompositeValue* val) -> Value& {
			Vector3Value* value = static_cast<Vector3Value*>(val);
			return value->m_y;
		};
		m_properties[m_y.GetId()] = &m_y;
	}

	{
		m_z.m_name = "Z";
		m_z.m_category = "";
		m_z.m_getValue = [](CompositeValue* val) -> Value& {
			Vector3Value* value = static_cast<Vector3Value*>(val);
			return value->m_z;
		};
		m_properties[m_z.GetId()] = &m_z;
	}

	m_name = "Vector 3";
	m_category = "Math";
}

common::Vector3TypeDef::~Vector3TypeDef()
{
}

void common::Vector3TypeDef::Construct(Value& container) const
{
	Vector3Value* tmp = new Vector3Value(container.m_outer);
	container.m_payload = tmp;
}

common::Vector4Value::Vector4Value(size_t outer) :
	CopyValue(Vector4TypeDef::GetTypeDef(), outer),
	m_x(Vector4TypeDef::GetTypeDef().m_x.GetType(), this),
	m_y(Vector4TypeDef::GetTypeDef().m_y.GetType(), this),
	m_z(Vector4TypeDef::GetTypeDef().m_z.GetType(), this),
	m_w(Vector4TypeDef::GetTypeDef().m_w.GetType(), this)
{
}

common::Vector4Value::~Vector4Value()
{
}

void common::Vector4Value::Copy(const CopyValue& src)
{
	const Vector4Value& other = static_cast<const Vector4Value&>(src);
	*this = other;
}


common::Vector3Value::Vector3Value(size_t outer) :
	CopyValue(Vector3TypeDef::GetTypeDef(), outer),
	m_x(Vector3TypeDef::GetTypeDef().m_x.GetType(), this),
	m_y(Vector3TypeDef::GetTypeDef().m_y.GetType(), this),
	m_z(Vector3TypeDef::GetTypeDef().m_z.GetType(), this)
{
}

common::Vector3Value::~Vector3Value()
{
}

void common::Vector3Value::Copy(const CopyValue& src)
{
	const Vector3Value& other = static_cast<const Vector3Value&>(src);
	*this = other;
}


common::TransformTypeDef::TransformTypeDef() :
	ValueTypeDef(&ValueTypeDef::GetTypeDef(), "22DEF19B-EA55-4795-9106-56200D6A1C49"),
	m_position("3D07B260-00EC-4CA9-803D-5A043476C58B", Vector3TypeDef::GetTypeDef()),
	m_rotation("88AB23E4-636B-4E08-A5AB-0D92A5D18CB7", Vector3TypeDef::GetTypeDef()),
	m_scale("E855B049-4305-47FF-962B-0115463D9350", Vector3TypeDef::GetTypeDef())
{
	{
		m_position.m_name = "Position";
		m_position.m_category = "";
		m_position.m_getValue = [](CompositeValue* val) -> Value& {
			TransformValue* value = static_cast<TransformValue*>(val);
			return value->m_position;
		};
		m_properties[m_position.GetId()] = &m_position;
	}

	{
		m_rotation.m_name = "Rotation";
		m_rotation.m_category = "";
		m_rotation.m_getValue = [](CompositeValue* val) -> Value& {
			TransformValue* value = static_cast<TransformValue*>(val);
			return value->m_rotation;
		};
		m_properties[m_rotation.GetId()] = &m_rotation;
	}

	{
		m_scale.m_name = "Scale";
		m_scale.m_category = "";
		m_scale.m_getValue = [](CompositeValue* val) -> Value& {
			TransformValue* value = static_cast<TransformValue*>(val);
			return value->m_scale;
		};
		m_properties[m_scale.GetId()] = &m_scale;
	}

	m_name = "Transform";
	m_category = "Math";
}

common::TransformTypeDef::~TransformTypeDef()
{
}

void common::TransformTypeDef::Construct(Value& container) const
{
	TransformValue* tmp = new TransformValue(container.m_outer);
	container.m_payload = tmp;
}

common::TransformValue::TransformValue(size_t outer) :
	CopyValue(TransformTypeDef::GetTypeDef(), outer),
	m_position(TransformTypeDef::GetTypeDef().m_position.GetType(), this),
	m_rotation(TransformTypeDef::GetTypeDef().m_position.GetType(), this),
	m_scale(TransformTypeDef::GetTypeDef().m_position.GetType(), this)
{
}

common::TransformValue::~TransformValue()
{
}

void common::TransformValue::Copy(const CopyValue& src)
{
	const TransformValue& other = static_cast<const common::TransformValue&>(src);
	*this = other;
}
