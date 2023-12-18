#include "WindowTypeDef.h"

#include "PrimitiveTypes.h"
#include "JobSystemDef.h"

#include "WindowObj.h"

namespace
{
	BasicObjectContainer<rendering::WindowTypeDef> m_wnd;
}

rendering::WindowTypeDef::WindowTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "99EC3C91-412A-450B-AA61-721FFC77BF8B"),
	m_width("7E7854FF-F38C-47F1-8690-6FC16DB93882", IntTypeDef::GetTypeDef()),
	m_height("10A08A76-DB55-4F6C-B74B-8FA4DD263005", IntTypeDef::GetTypeDef()),
	m_windowUpdateJobSystem("BE6AE599-53DC-4932-9536-74D398FD1003", TypeTypeDef::GetTypeDef(jobs::JobSystemDef::GetTypeDef()))
{
	{
		m_width.m_name = "Width";
		m_width.m_category= "Setup";
		m_width.m_getValue = [](CompositeValue* obj) -> Value& {
			WindowObj* wnd = static_cast<WindowObj*>(obj);
			return wnd->m_width;
		};
		m_properties[m_width.GetId()] = &m_width;
	}

	{
		m_height.m_name = "Height";
		m_height.m_category = "Setup";
		m_height.m_getValue = [](CompositeValue* obj) -> Value& {
			WindowObj* wnd = static_cast<WindowObj*>(obj);
			return wnd->m_height;
		};
		m_properties[m_height.GetId()] = &m_height;
	}

	{
		m_windowUpdateJobSystem.m_name = "Window Update Job System";
		m_windowUpdateJobSystem.m_category = "Setup";
		m_windowUpdateJobSystem.m_getValue = [](CompositeValue* obj) -> Value& {
			WindowObj* wnd = static_cast<WindowObj*>(obj);
			return wnd->m_windowUpdateJobSystem;
		};
		m_properties[m_windowUpdateJobSystem.GetId()] = &m_windowUpdateJobSystem;
	}

	m_name = "Window";
	m_category = "Rendering";
}

void rendering::WindowTypeDef::Construct(Value& container) const
{
	WindowObj* obj = new WindowObj(WindowTypeDef::GetTypeDef(), nullptr);
	container.AssignObject(obj);
}

const rendering::WindowTypeDef& rendering::WindowTypeDef::GetTypeDef()
{
	if (!m_wnd.m_object)
	{
		m_wnd.m_object = new WindowTypeDef();
	}

	return *m_wnd.m_object;
}
