#include "AssetManager.h"

#include "Files.h"
#include "JSONValue.h"

#include <filesystem>

void assets::Boot()
{
    using namespace json_parser;
    for (const auto& entry : std::filesystem::directory_iterator(files::GetDataDir() + files::GetAssetsDir()))
    {
        if (entry.is_regular_file())
        {
            std::filesystem::path path = entry.path();
            std::string file = path.filename().string();
            std::string contents;
            files::ReadTextFile(files::GetAssetsDir() + file, contents);

            JSONValue tmp;
            JSONValue::FromString(contents, tmp);
        }
    }
}
