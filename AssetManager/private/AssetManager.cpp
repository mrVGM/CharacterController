#include "AssetManager.h"

#include "Files.h"
#include "JSONValue.h"

#include "ListDef.h"
#include "ValueList.h"

#include "TypeDef.h"
#include "AssetTypeDef.h"

#include <filesystem>

namespace
{
    BasicObjectContainer<Value> m_assetList;

    Value& GetAssetList()
    {
        if (!m_assetList.m_object)
        {
            m_assetList.m_object = new Value(ListDef::GetTypeDef(ReferenceTypeDef::GetReferenceTypeDef()), nullptr);
        }
        return *m_assetList.m_object;
    }
}

void assets::Boot()
{
    using namespace json_parser;

    Value& assetList = GetAssetList();

    const TypeDef::TypeDefsMap& defsMap = TypeDef::GetDefsMap();
    ValueList* valueList = static_cast<ValueList*>(std::get<CompositeValue*>(assetList.m_payload));

    for (const auto& entry : std::filesystem::directory_iterator(files::GetDataDir() + files::GetAssetsDir()))
    {
        if (entry.is_regular_file())
        {
            std::filesystem::path path = entry.path();
            std::string file = path.filename().string();
            std::string contents;
            files::ReadTextFile(files::GetAssetsDir() + file, contents);

            JSONValue curAssetData;
            JSONValue::FromString(contents, curAssetData);
            auto& map = curAssetData.GetAsObj();

            AssetTypeDef* assetTypeDef = new AssetTypeDef(curAssetData);
            Value& asset = valueList->EmplaceBack();

            {
                Value tmp(*assetTypeDef, nullptr);
                assetTypeDef->Construct(tmp);
                asset = tmp;
            }


            JSONValue defaults = map["defaults"];
            const ReferenceTypeDef* parent = static_cast<const ReferenceTypeDef*>(assetTypeDef->GetParent());
            parent->DeserializeFromJSON(asset, defaults);
        }
    }
}

void assets::Shutdown()
{
    m_assetList.Dispose();
}