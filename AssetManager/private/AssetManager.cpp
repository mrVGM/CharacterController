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
}

void assets::Boot(const Value& preloadedAssets)
{
    using namespace json_parser;
    
    Value& assetList = GetAssetList();
    ValueList* valueList = assetList.GetValue<ValueList*>();
    ValueList* preloadedList = preloadedAssets.GetValue<ValueList*>();
    for (auto it = preloadedList->GetIterator(); it; ++it)
    {
        Value& tmp = valueList->EmplaceBack();
        tmp = *it;
    }

    const TypeDef::TypeDefsMap& defsMap = TypeDef::GetDefsMap();

    typedef std::pair<Value*, JSONValue> DefaultValuePair;
    std::list<DefaultValuePair> defaultValues;

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
            defaultValues.push_back(DefaultValuePair(&asset, defaults));
        }
    }
    for (auto it = defaultValues.begin(); it != defaultValues.end(); ++it)
    {
        Value& cur = *it->first;
        CompositeValue* curVal = cur.GetValue<CompositeValue*>();
        const AssetTypeDef& assetTypeDef = static_cast<const AssetTypeDef&>(curVal->GetTypeDef());
        assetTypeDef.DeserializeFromJSON(cur, it->second);
    }
}

void assets::Shutdown()
{
    m_assetList.Dispose();
}

Value& assets::GetAssetList()
{
    if (!m_assetList.m_object)
    {
        m_assetList.m_object = new Value(ListDef::GetTypeDef(ReferenceTypeDef::GetTypeDef()), nullptr);
    }
    return *m_assetList.m_object;
}