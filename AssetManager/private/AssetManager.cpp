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

namespace
{
    BasicObjectContainer<Value> m_assetList;
}

void assets::Boot(const Value& preloadedAssets, jobs::Job* done)
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

    struct Context
    {
        int m_inProgress = 0;
        std::list<Value*> m_assets;
        jobs::Job* m_done = nullptr;
    };

    class InitDefaults : public jobs::Job
    {
        Context& m_ctx;
        AssetTypeDef* m_assetDef = nullptr;
    public:
        InitDefaults(Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            for (auto it = m_ctx.m_assets.begin(); it != m_ctx.m_assets.end(); ++it)
            {
                ObjectValue* cur = (*it)->GetValue<ObjectValue*>();
                const AssetTypeDef& curAssetDef = static_cast<const AssetTypeDef&>(cur->GetTypeDef());

                JSONValue& jsonData = const_cast<JSONValue&>(curAssetDef.GetJSONData());
                auto& map = jsonData.GetAsObj();
                JSONValue& defaults = map["defaults"];
                curAssetDef.DeserializeFromJSON(*(*it), defaults);
            }

            jobs::RunSync(m_ctx.m_done);
            delete &m_ctx;
        }
    };

    class LoadComplete : public jobs::Job
    {
        Context& m_ctx;
        AssetTypeDef* m_assetDef = nullptr;
    public:
        LoadComplete(Context& ctx, AssetTypeDef* assetDef) :
            m_ctx(ctx),
            m_assetDef(assetDef)
        {
        }

        void Do() override
        {
            Value& assetList = GetAssetList();
            ValueList* l = assetList.GetValue<ValueList*>();
            Value& val = l->EmplaceBack();
            m_assetDef->Construct(val);
            m_ctx.m_assets.push_back(&val);
            --m_ctx.m_inProgress;

            if (m_ctx.m_inProgress > 0)
            {
                return;
            }

            jobs::RunSync(new InitDefaults(m_ctx));
        }
    };

    struct LoadAssetContext
    {
        std::string m_path;
        Context* m_ctx = nullptr;
    };

    class LoadAsset : public jobs::Job
    {
        LoadAssetContext m_ctx;
    public:
        LoadAsset(const LoadAssetContext& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            std::string contents;
            files::ReadTextFile(m_ctx.m_path, contents);

            JSONValue assetData;
            JSONValue::FromString(contents, assetData);
            AssetTypeDef* assetTypeDef = new AssetTypeDef(assetData);

            jobs::RunSync(new LoadComplete(*m_ctx.m_ctx, assetTypeDef));
        }
    };

    const TypeDef::TypeDefsMap& defsMap = TypeDef::GetDefsMap();

    typedef std::pair<Value*, JSONValue> DefaultValuePair;
    std::list<DefaultValuePair> defaultValues;

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

    Context* ctx = new Context();
    ctx->m_inProgress = assetPaths.size();
    ctx->m_done = done;

    for (auto it = assetPaths.begin(); it != assetPaths.end(); ++it)
    {
        LoadAssetContext loadCtx{ *it, ctx };
        jobs::RunAsync(new LoadAsset(loadCtx));
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