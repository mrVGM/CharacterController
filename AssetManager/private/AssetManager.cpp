#include "AssetManager.h"

#include "Files.h"
#include "JSONValue.h"

#include "ListDef.h"
#include "ValueList.h"

#include "TypeDef.h"
#include "AssetTypeDef.h"

#include "Jobs.h"
#include "Job.h"

#include <filesystem>


void assets::Boot(jobs::Job* done)
{
    using namespace json_parser;

    std::list<std::string> assetPaths;

    for (const auto& entry : std::filesystem::directory_iterator(files::GetDataDir() + files::GetAssetsDir()))
    {
        if (entry.is_regular_file())
        {
            std::filesystem::path path = entry.path();
            std::string file = path.filename().string();

            assetPaths.push_back(files::GetAssetsDir() + file);
        }
    }

    int* toLoad = new int;
    *toLoad = 0;

    auto assetLoaded = [=]() {
        --(*toLoad);
        if (*toLoad > 0)
        {
            return;
        }

        delete toLoad;

        jobs::RunSync(done);
    };

    auto createLoadAssetJob = [=](const std::string& path) {
        return jobs::Job::CreateByLambda([=]() {
            std::string contents;
            files::ReadTextFile(path, contents);

            JSONValue assetData;
            JSONValue::FromString(contents, assetData);
            AssetTypeDef* assetTypeDef = new AssetTypeDef(assetData);

            jobs::RunSync(jobs::Job::CreateByLambda(assetLoaded));
        });
    };

    for (auto it = assetPaths.begin(); it != assetPaths.end(); ++it)
    {
        ++(*toLoad);
        jobs::RunAsync(createLoadAssetJob(*it));
    }
}

void assets::Shutdown()
{
}
