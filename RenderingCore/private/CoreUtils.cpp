#include "CoreUtils.h"

#include "ObjectValueContainer.h"

rendering::WindowObj* rendering::core::utils::GetWindow()
{
	std::list<ObjectValue*> tmp;
	ObjectValueContainer& container = ObjectValueContainer::GetContainer();

	container.GetObjectsOfType(WindowTypeDef::GetTypeDef(), tmp);
	WindowObj* wnd = static_cast<WindowObj*>(tmp.front());
	return wnd;
}

rendering::DXDevice* rendering::core::utils::GetDevice()
{
	std::list<ObjectValue*> tmp;
	ObjectValueContainer& container = ObjectValueContainer::GetContainer();

	container.GetObjectsOfType(DXDeviceTypeDef::GetTypeDef(), tmp);
	DXDevice* device = static_cast<DXDevice*>(tmp.front());
	return device;
}
