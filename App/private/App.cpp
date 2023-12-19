#include "App.h"

#include "AppTypeDef.h"
#include "AppEntryTypeDef.h"
#include "AppObj.h"
#include "AppEntryObj.h"
#include "ObjectValueContainer.h"
#include "JSONParser.h"
#include "Types.h"
#include "ListDef.h"
#include "ValueList.h"
#include "Jobs.h"
#include "Reflection.h"
#include "AssetManager.h"

#include "RenderingCore.h"

namespace
{
	void Run()
	{
		ObjectValueContainer& container = ObjectValueContainer::GetContainer();

		std::list<ObjectValue*> objs;
		container.GetObjectsOfType(app::AppTypeDef::GetTypeDef(), objs);

		app::AppObj* app = static_cast<app::AppObj*>(objs.front());

		objs.clear();
		container.GetObjectsOfType(*app->m_appEntry.GetType<const app::AppEntryTypeDef*>(), objs);

		app::AppEntryObj* entry = static_cast<app::AppEntryObj*>(objs.front());
		entry->Boot();
	}
}

void app::Boot()
{
	json_parser::Boot();
	BootTypeSystem();

	reflection::Boot();
	rendering::core::Boot();

	AppTypeDef::GetTypeDef();
	AppEntryTypeDef::GetTypeDef();

	Value jobSystems(ListDef::GetTypeDef(ReferenceTypeDef::GetTypeDef()), nullptr);
	ValueList* vl = jobSystems.GetValue<ValueList*>();

	Value& mainJS = vl->EmplaceBack();
	Value& asyncJS = vl->EmplaceBack();
	jobs::Boot(mainJS, asyncJS);


	class StartAppJob : public jobs::Job
	{
	public:
		void Do() override
		{
			Run();
		}
	};

	assets::Boot(jobSystems, new StartAppJob());
}



void app::Shutdown()
{
	assets::Shutdown();
}
