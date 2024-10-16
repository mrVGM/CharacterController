﻿#include "App.h"
#include "Files.h"

#include <iostream>
#include <filesystem>

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
	app::Boot();

	std::cin.get();

	app::Shutdown();
	std::cout << "Shut Down!" << std::endl;

	std::cin.get();

	return 0;
}
