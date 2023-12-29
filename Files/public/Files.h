#pragma once

#include <string>

namespace files
{
	void Init(const std::string& dataDir);

	void ReadTextFile(const std::string& filePath, std::string& outContents);
	void WriteTextFile(const std::string& filePath, const std::string& contents);

	const std::string& GetDataDir();
	const std::string& GetAssetsDir();
	const std::string& GetAssetsBinDir();
	const std::string& GetReflectionDataDir();
}