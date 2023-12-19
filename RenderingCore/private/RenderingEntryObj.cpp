#include "RenderingEntryObj.h"

#include "ObjectValueContainer.h"

#include "WindowTypeDef.h"
#include "WindowObj.h"

rendering::core::RenderingEntryObj::RenderingEntryObj(const ReferenceTypeDef& typeDef, const CompositeValue* outer) :
	app::AppEntryObj(typeDef, outer)
{
}

void rendering::core::RenderingEntryObj::Boot()
{
	ObjectValueContainer& container = ObjectValueContainer::GetContainer();

	std::list<ObjectValue*> tmp;
	container.GetObjectsOfType(WindowTypeDef::GetTypeDef(), tmp);

	WindowObj* wnd = static_cast<WindowObj*>(tmp.front());
	wnd->Start();
}
