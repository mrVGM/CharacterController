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

    std::list<JSONValue> defaultValues;

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
            defaultValues.push_back(defaults);
        }
    }

    auto defaultValuesIt = defaultValues.begin();
    for (auto it = valueList->GetIterator(); it; ++it)
    {
        Value& cur = *it;
        CompositeValue* curVal = std::get<CompositeValue*>(cur.m_payload);
        const AssetTypeDef& assetTypeDef = static_cast<const AssetTypeDef&>(curVal->GetTypeDef());
        assetTypeDef.GetParent()->DeserializeFromJSON(cur, *defaultValuesIt);

        ++defaultValuesIt;
    }
}

void assets::Shutdown()
{
    m_assetList.Dispose();
}