#include "JSONParser.h"
#include "Files.h"

#include "JSONBuilder.h"

#include "AssetManager.h"

#include <iostream>
#include <filesystem>

#include "PrimitiveTypes.h"
#include "Types.h"
#include "ListDef.h"
#include "ValueList.h"
#include "CompositeValue.h"

#include "App.h"
#include "Reflection.h"
#include "Jobs.h"

int main(int args, const char** argv)
{
	std::string executableName = argv[0];
	int index = executableName.find_last_of('\\');

	if (index < 0) {
		std::cerr << "Can't find data directory!" << std::endl;
		return 1;
	}
	std::string executableDirectory = executableName.substr(0, index);
	std::filesystem::path executableDirPath = std::filesystem::path(executableDirectory);

#if DEBUG
	std::filesystem::path dataPath = "..\\..\\..\\..\\Data\\";
#else
	std::filesystem::path dataPath = executableDirPath.append("..\\data\\");
#endif

	files::Init(dataPath.string());
	json_parser::Boot();

	BootTypeSystem();

	Value jobSystems(ListDef::GetTypeDef(ReferenceTypeDef::GetReferenceTypeDef()), nullptr);
	ValueList* vl = static_cast<ValueList*>(std::get<CompositeValue*>(jobSystems.m_payload));

	Value& mainJS = vl->EmplaceBack();
	Value& asyncJS = vl->EmplaceBack();
	jobs::Boot(mainJS, asyncJS);

	reflection::Boot();
	app::Boot();
	assets::Boot(jobSystems);

	app::Run();

	assets::Shutdown();

	return 0;
}
