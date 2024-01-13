#include "App.h"

#include "JSONParser.h"
#include "XMLReader.h"
#include "AppTypeDef.h"
#include "AppEntry.h"
#include "AppObj.h"
#include "ObjectValueContainer.h"
#include "Types.h"
#include "ListDef.h"
#include "ValueList.h"
#include "Jobs.h"
#include "Reflection.h"
#include "AssetManager.h"
#include "Runtime.h"

#include "CommonTypeDefs.h"
#include "RenderingCore.h"
#include "Renderer.h"
#include "Geometry.h"
#include "RenderPass.h"
#include "Materials.h"

#include "UnlitRP.h"

#include "GCWorker.h"

#include "Scene.h"

namespace
{
	Value m_app(app::AppTypeDef::GetTypeDef(), nullptr);
	Value m_appEntry(app::AppEntryTypeDef::GetTypeDef(), nullptr);

	void Run()
	{
		ObjectValueContainer::GetObjectOfType(app::AppTypeDef::GetTypeDef(), m_app);
		app::AppObj* app = m_app.GetValue<app::AppObj*>();

		const TypeDef* appEntry = app->m_appEntry.GetType<const TypeDef*>();
		ObjectValueContainer::GetObjectOfType(*appEntry, m_appEntry);

		app::AppEntryObj* entry = m_appEntry.GetValue<app::AppEntryObj*>();
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
	rendering::materials::Boot();
	rendering::render_pass::Boot();
	geo::Boot();
	scene::Boot();
	rendering::unlit_rp::Boot();
	runtime::Boot();

#pragma endregion

	AppTypeDef::GetTypeDef();
	AppEntryTypeDef::GetTypeDef();

	jobs::Boot();
	gc::Boot();

	jobs::RunSync(jobs::Job::CreateByLambda([=]() {
		ObjectValueContainer& container = ObjectValueContainer::GetContainer();
		container.StartExclusiveThreadAccess();

		assets::Boot(jobs::Job::CreateByLambda([=]() {
			Run();
		}));
	}));
}



void app::Shutdown()
{
	gc::Shutdown();
	assets::Shutdown();
}

