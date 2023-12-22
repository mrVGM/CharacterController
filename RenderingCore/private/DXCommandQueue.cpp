#include "DXCommandQueue.h"

#include "Jobs.h"

#include "CoreUtils.h"

namespace
{
    BasicObjectContainer<rendering::DXCommandQueueTypeDef> m_commandQueueTypeDef;
}

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

const rendering::DXCommandQueueTypeDef& rendering::DXCommandQueueTypeDef::GetTypeDef()
{
    if (!m_commandQueueTypeDef.m_object)
    {
        m_commandQueueTypeDef.m_object = new DXCommandQueueTypeDef();
    }

    return *m_commandQueueTypeDef.m_object;
}

rendering::DXCommandQueueTypeDef::DXCommandQueueTypeDef() :
    ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "B7F9EC5F-C26F-4DE9-9A04-0B3E0570812E")
{
    m_name = "Command Queue";
    m_category = "Rendering";
}

rendering::DXCommandQueueTypeDef::~DXCommandQueueTypeDef()
{
}

void rendering::DXCommandQueueTypeDef::Construct(Value& value) const
{
    DXCommandQueue* commandQueue = new DXCommandQueue(*this);
    value.AssignObject(commandQueue);
}

void rendering::DXCommandQueue::Create()
{
    using Microsoft::WRL::ComPtr;

    DXDevice* dxDevice = core::utils::GetDevice();
    if (!dxDevice)
    {
        throw "No device found!";
    }


    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    THROW_ERROR(
        dxDevice->GetDevice().CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)),
        "Can't create Command QUEUE!")
}

ID3D12CommandQueue* rendering::DXCommandQueue::GetCommandQueue()
{
    return m_commandQueue.Get();
}

rendering::DXCommandQueue::DXCommandQueue(const ReferenceTypeDef& typeDef) :
    ObjectValue(typeDef)
{
}

rendering::DXCommandQueue::~DXCommandQueue()
{
}

void rendering::DXCommandQueue::Load(jobs::Job* done)
{
    struct Context
    {
        DXCommandQueue* m_self = nullptr;
        jobs::Job* m_done = nullptr;
    };

    Context ctx{ this, done };

    class LoadJob : public jobs::Job
    {
    private:
        Context m_ctx;
    public:
        LoadJob(const Context& ctx) :
            m_ctx(ctx)
        {
        }

        void Do() override
        {
            m_ctx.m_self->Create();

            jobs::RunSync(m_ctx.m_done);
        }
    };


    jobs::RunSync(new LoadJob(ctx));
}

#undef THROW_ERROR