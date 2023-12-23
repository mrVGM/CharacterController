#include "DXCopyCommandQueue.h"

#include "Jobs.h"

#include "CoreUtils.h"

namespace
{
    BasicObjectContainer<rendering::DXCopyCommandQueueTypeDef> m_copyCommandQueueTypeDef;
}

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

const rendering::DXCopyCommandQueueTypeDef& rendering::DXCopyCommandQueueTypeDef::GetTypeDef()
{
    if (!m_copyCommandQueueTypeDef.m_object)
    {
        m_copyCommandQueueTypeDef.m_object = new DXCopyCommandQueueTypeDef();
    }

    return *m_copyCommandQueueTypeDef.m_object;
}

rendering::DXCopyCommandQueueTypeDef::DXCopyCommandQueueTypeDef() :
    ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "2AD66DC7-0968-47B7-8F07-F914BD148B4A")
{
    m_name = "Copy Command Queue";
    m_category = "Rendering";
}

rendering::DXCopyCommandQueueTypeDef::~DXCopyCommandQueueTypeDef()
{
}

void rendering::DXCopyCommandQueueTypeDef::Construct(Value& container) const
{
    DXCopyCommandQueue* commandQueue = new DXCopyCommandQueue(*this);
    container.AssignObject(commandQueue);
}

void rendering::DXCopyCommandQueue::Create()
{
    DXDevice* dxDevice = core::utils::GetDevice();
    if (!dxDevice)
    {
        throw "No device found!";
    }

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

    THROW_ERROR(
        dxDevice->GetDevice().CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)),
        "Can't create Command QUEUE!")
}

rendering::DXCopyCommandQueue::DXCopyCommandQueue(const ReferenceTypeDef& typeDef) :
    ObjectValue(typeDef)
{
}

rendering::DXCopyCommandQueue::~DXCopyCommandQueue()
{
}

void rendering::DXCopyCommandQueue::Load(jobs::Job* done)
{
    Create();
    jobs::RunSync(done);
}

ID3D12CommandQueue* rendering::DXCopyCommandQueue::GetCommandQueue()
{
    return m_commandQueue.Get();
}

#undef THROW_ERROR

