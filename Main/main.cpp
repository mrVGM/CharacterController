#include "JSONParser.h"
#include "Files.h"

#include "JSONBuilder.h"

#include "AssetManager.h"

#include <iostream>
#include <filesystem>

#include "PrimitiveTypes.h"
#include "Types.h"

#include "TestTypeDef.h"

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

	TestTypeDef::GetTypeDef();

	BootTypeSystem();

	TypeDef::SaveReflectionData();

	std::string defaults;
	files::ReadTextFile("Assets\\test_object_defaults.json", defaults);

	json_parser::JSONValue tmp;
	json_parser::JSONValue::FromString(defaults, tmp);

	assets::Boot();


	return 0;
}
