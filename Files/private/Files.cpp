#include "Files.h"

#include <stdio.h>

namespace
{
	std::string m_dataDir;
	std::string m_assetsDir;
	std::string m_assetsBinDir;
	std::string m_reflectionDataDir;
}

void files::Init(const std::string& dataDir)
{
	m_dataDir = dataDir;
	m_assetsDir = "Assets\\";
	m_assetsBinDir = "AssetsBin\\";
	m_reflectionDataDir = "ReflectionData\\";
}

void files::ReadTextFile(const std::string& filePath, std::string& outContents)
{
	std::string path = m_dataDir + filePath;

	FILE* f;
	fopen_s(&f, path.c_str(), "r");
	if (f == nullptr) {
		return;
	}

	const size_t buffSize = 501;
	char buf[buffSize] = {};
	
	while (true) {
		memset(buf, 0, buffSize);
		size_t read = fread(buf, sizeof(char), buffSize - 1, f);
		outContents += buf;
		if (read < buffSize - 1) {
			break;
		}
	}
	fclose(f);
}

void files::WriteTextFile(const std::string& filePath, const std::string& contents)
{
	std::string path = m_dataDir + filePath;

	FILE* f;
	fopen_s(&f, path.c_str(), "w");
	if (f == nullptr) {
		return;
	}

	const size_t buffSize = 500;
	char buf[buffSize] = {};

	size_t curPos = 0;

	while (curPos < contents.size()) {
		size_t toWrite = std::min(buffSize, contents.size() - curPos);
		memcpy(buf, contents.c_str() + curPos, toWrite);

		size_t written = fwrite(buf, sizeof(char), toWrite, f);
		curPos += written;
	}
	fclose(f);
}

const std::string& files::GetDataDir()
{
	return m_dataDir;
}

const std::string& files::GetAssetsDir()
{
	return m_assetsDir;
}

const std::string& files::GetAssetsBinDir()
{
	return m_assetsBinDir;
}

const std::string& files::GetReflectionDataDir()
{
	return m_reflectionDataDir;
}
