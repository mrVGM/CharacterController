#include "App.h"

#include "AppTypeDef.h"
#include "AppEntryTypeDef.h"

#include "AppObj.h"
#include "AppEntryObj.h"

#include "ObjectValueContainer.h"

void app::Boot()
{
	AppTypeDef::GetTypeDef();
	AppEntryTypeDef::GetTypeDef();
}

void app::Run()
{
	ObjectValueContainer& container = ObjectValueContainer::GetContainer();

	std::list<ObjectValue*> objs;
	container.GetObjectsOfType(AppTypeDef::GetTypeDef(), objs);

	AppObj* app = static_cast<AppObj*>(objs.front());

	objs.clear();
	container.GetObjectsOfType(AppEntryTypeDef::GetTypeDef(), objs);

	AppEntryObj* entry = static_cast<AppEntryObj*>(objs.front());
	entry->Boot();
}
