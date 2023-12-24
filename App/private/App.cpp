#include "App.h"

#include "JSONParser.h"
#include "XMLReader.h"
#include "AppTypeDef.h"
#include "AppEntryTypeDef.h"
#include "AppObj.h"
#include "AppEntryObj.h"
#include "ObjectValueContainer.h"
#include "Types.h"
#include "ListDef.h"
#include "ValueList.h"
#include "Jobs.h"
#include "Reflection.h"
#include "AssetManager.h"

#include "CommonTypeDefs.h"
#include "RenderingCore.h"
#include "Renderer.h"
#include "Geometry.h"
#include "RenderPass.h"

#include "GCWorker.h"

#include "Scene.h"

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
	xml_reader::Boot();

	BootTypeSystem();

#pragma region Boot Libs

	reflection::Boot();
	common_defs::Boot();
	rendering::core::Boot();
	rendering::renderer::Boot();
	rendering::render_pass::Boot();
	geo::Boot();
	scene::Boot();

#pragma endregion

	AppTypeDef::GetTypeDef();
	AppEntryTypeDef::GetTypeDef();

	struct Context
	{
		Value m_jobSystems;

		Context() :
			m_jobSystems(ListDef::GetTypeDef(ReferenceTypeDef::GetTypeDef()), nullptr)
		{
		}
	};
	Context ctx;

	ValueList* vl = ctx.m_jobSystems.GetValue<ValueList*>();

	Value& mainJS = vl->EmplaceBack();
	Value& asyncJS = vl->EmplaceBack();
	jobs::Boot(mainJS, asyncJS);

	gc::Boot();

	class StartAppJob : public jobs::Job
	{
	public:
		void Do() override
		{
			Run();
		}
	};

	class StartExclusiveAccess : public jobs::Job
	{
		Context m_ctx;
	public:
		StartExclusiveAccess(const Context& ctx)
		{
			m_ctx = ctx;
		}

		void Do() override
		{
			ObjectValueContainer& container = ObjectValueContainer::GetContainer();
			container.StartExclusiveThreadAccess();

			assets::Boot(m_ctx.m_jobSystems, new StartAppJob());
		}
	};

	jobs::RunSync(new StartExclusiveAccess(ctx));
}



void app::Shutdown()
{
	gc::Shutdown();
	assets::Shutdown();
}

