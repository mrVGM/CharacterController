#include "AssetManager.h"

#include "Files.h"
#include "JSONValue.h"

#include "GenericListDef.h"
#include "CompositeValue.h"

#include "TypeDef.h"
#include "AssetTypeDef.h"

#include <filesystem>

namespace
{
    Value m_assetList;
}

void assets::Boot()
{
    using namespace json_parser;

    m_assetList.Initialize(ListDef::GetTypeDef(ReferenceTypeDef::GetTypeDef()), nullptr);

    const TypeDef::TypeDefsMap& defsMap = TypeDef::GetDefsMap();
    ValueList* valueList = static_cast<ValueList*>(std::get<CompositeValue*>(m_assetList.m_payload));

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
            auto& map = tmp.GetAsObj();

            AssetTypeDef* assetTypeDef = new AssetTypeDef(tmp);
            Value& asset = valueList->m_values.emplace_back();
            asset.Initialize(static_cast<const ListDef&>(valueList->GetTypeDef()).m_templateDef, valueList);

            assetTypeDef->Construct(asset);

            JSONValue defaults = map["defaults"];
            const ReferenceTypeDef* parent = static_cast<const ReferenceTypeDef*>(assetTypeDef->GetParent());
            parent->DeserializeFromJSON(asset, defaults);
        }
    }
}
