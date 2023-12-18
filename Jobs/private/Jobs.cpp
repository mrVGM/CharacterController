#include "Jobs.h"

#include "AssetManager.h"
#include "AssetTypeDef.h"
#include "ValueList.h"
#include "ObjectValueContainer.h"

#include "JobSystemDef.h"

#include "JSONValue.h"

namespace
{
	const char* m_mainJobSystemData = "{\"id\": {\"id\": \"eb769ae4-f478-4b56-bbd3-dd81f58301e9\"}, \"parent\": {\"id\": \"061DFEE1-B9C6-406D-BFEC-E89830C7FD3D\"}, \"name\": \"Main Job System\", \"category\": \"Jobs\", \"isGenerated\": 1, \"defaults\": {\"C519F2FA-A756-42E1-B362-15556A2758D8\": 1} }";
	const char* m_asyncJobSystemData = "{\"id\": {\"id\": \"eb1f39c7-1073-4091-985a-73120cc80d18\"}, \"parent\": {\"id\": \"061DFEE1-B9C6-406D-BFEC-E89830C7FD3D\"}, \"name\": \"Async Job System\", \"category\": \"Jobs\", \"isGenerated\" : 1, \"defaults\": {\"C519F2FA-A756-42E1-B362-15556A2758D8\": 5} }";

    AssetTypeDef* InitJobSystem(const char* data)
    {
        using namespace json_parser;

        Value& assetList = assets::GetAssetList();
        ValueList* valueList = static_cast<ValueList*>(std::get<CompositeValue*>(assetList.m_payload));

        JSONValue curAssetData;
        JSONValue::FromString(data, curAssetData);
        auto& map = curAssetData.GetAsObj();

        AssetTypeDef* assetTypeDef = new AssetTypeDef(curAssetData);
        Value& asset = valueList->EmplaceBack();
        
        {
            Value tmp(*assetTypeDef, nullptr);
            assetTypeDef->Construct(tmp);
            asset = tmp;
        }

        JSONValue defaultValues = map["defaults"];
        assetTypeDef->GetParent()->DeserializeFromJSON(asset, defaultValues);

        return assetTypeDef;
    }

    jobs::JobSystem* m_mainJobSystem = nullptr;
    jobs::JobSystem* m_asyncJobSystem = nullptr;
}

void jobs::Boot()
{
    JobSystemDef::GetJobSystemTypeDef();

    AssetTypeDef* mainJobSystem = InitJobSystem(m_mainJobSystemData);
    AssetTypeDef* asyncJobSystem = InitJobSystem(m_asyncJobSystemData);

    ObjectValueContainer& container = ObjectValueContainer::GetContainer();

    std::list<ObjectValue*> objs;
    container.GetObjectsOfType(*mainJobSystem, objs);
    m_mainJobSystem = static_cast<JobSystem*>(objs.front());

    objs.clear();
    container.GetObjectsOfType(*asyncJobSystem, objs);
    m_asyncJobSystem = static_cast<JobSystem*>(objs.front());


}
