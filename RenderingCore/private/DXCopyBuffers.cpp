#include "DXCopyBuffers.h"

#include "ObjectValueContainer.h"
#include "PrimitiveTypes.h"

#include "Jobs.h"
#include "WaitFence.h"

#include "CoreUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}


namespace
{
    BasicObjectContainer<rendering::DXCopyBuffersTypeDef> m_dxCopyBuffers;


    struct CopyCommandList
    {
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

        CopyCommandList(rendering::DXDevice* device)
        {
            using Microsoft::WRL::ComPtr;
            using namespace rendering;

            THROW_ERROR(
                device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_commandAllocator)),
                "Can't create Command Allocator!")

            THROW_ERROR(
                device->GetDevice().CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)),
                "Can't create Command List!")

            THROW_ERROR(
                m_commandList->Close(),
                "Can't close command List!")
        }

        void Execute(
            rendering::DXBuffer& dst,
            const rendering::DXBuffer& src,
            rendering::DXCopyCommandQueue* commandQueue,
            ID3D12Fence* fence,
            UINT64 signal)
        {
            using namespace rendering;

            THROW_ERROR(
                m_commandAllocator->Reset(),
                "Can't reset Command Allocator!")

            THROW_ERROR(
                m_commandList->Reset(m_commandAllocator.Get(), nullptr),
                "Can't reset Command List!")

            m_commandList->CopyResource(dst.GetBuffer(), src.GetBuffer());

            THROW_ERROR(
                m_commandList->Close(),
                "Can't close Command List!")


            ID3D12CommandList* copyCommandList[] = { m_commandList.Get() };
            commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(copyCommandList), copyCommandList);

            commandQueue->GetCommandQueue()->Signal(fence, signal);
        }
    };
}


const rendering::DXCopyBuffersTypeDef& rendering::DXCopyBuffersTypeDef::GetTypeDef()
{
    if (!m_dxCopyBuffers.m_object)
    {
        m_dxCopyBuffers.m_object = new DXCopyBuffersTypeDef();
    }

    return *m_dxCopyBuffers.m_object;
}

rendering::DXCopyBuffersTypeDef::DXCopyBuffersTypeDef() :
    ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "945C7AFF-17AA-4A0F-9892-1CEF4DF8E668"),
    m_copyJobSystem("F7922C5F-0A2B-4E0D-8B11-9D39508D0D1A", TypeTypeDef::GetTypeDef(jobs::JobSystemDef::GetTypeDef())),
    m_copyFence("14CD8F99-7658-4A81-9049-C61CCC527659", TypeTypeDef::GetTypeDef(DXFenceTypeDef::GetTypeDef()))
{
    {
        m_copyJobSystem.m_name = "Job System";
        m_copyJobSystem.m_category = "Setup";
        m_copyJobSystem.m_getValue = [](CompositeValue* obj) -> Value& {
            DXCopyBuffers* copyBuffers = static_cast<DXCopyBuffers*>(obj);
            return copyBuffers->m_copyJobSystemDef;
        };

        m_properties[m_copyJobSystem.GetId()] = &m_copyJobSystem;
    }

    {
        m_copyFence.m_name = "Fence";
        m_copyFence.m_category = "Setup";
        m_copyFence.m_getValue = [](CompositeValue* obj) -> Value& {
            DXCopyBuffers* copyBuffers = static_cast<DXCopyBuffers*>(obj);
            return copyBuffers->m_copyFenceDef;
        };

        m_properties[m_copyFence.GetId()] = &m_copyFence;
    }

    m_name = "Copy Buffers";
    m_category = "Rendering";

    json_parser::JSONValue tmp;
    GetReflectionData(tmp);
}

rendering::DXCopyBuffersTypeDef::~DXCopyBuffersTypeDef()
{
}

void rendering::DXCopyBuffersTypeDef::Construct(Value& container) const
{
    DXCopyBuffers* copyBuffers = new DXCopyBuffers(*this);
    container.AssignObject(copyBuffers);
}



rendering::DXCopyBuffers::DXCopyBuffers(const ReferenceTypeDef& typeDef) :
    ObjectValue(typeDef),
    m_device(DXDeviceTypeDef::GetTypeDef(), this),
    m_copyCommandQueue(DXCopyCommandQueueTypeDef::GetTypeDef(), this),
    m_copyFence(DXFenceTypeDef::GetTypeDef(), this),
    m_copyJobSytem(jobs::JobSystemDef::GetTypeDef(), this),

    m_copyJobSystemDef(DXCopyBuffersTypeDef::GetTypeDef().m_copyJobSystem.GetType(), this),
    m_copyFenceDef(DXCopyBuffersTypeDef::GetTypeDef().m_copyFence.GetType(), this)
{
}

rendering::DXCopyBuffers::~DXCopyBuffers()
{
}

void rendering::DXCopyBuffers::Execute(
    DXBuffer& dst,
    const DXBuffer& src,
    jobs::Job* done)
{
    struct JobContext
    {
        DXCopyBuffers* m_dxCopyBuffers = nullptr;
        DXBuffer* m_dst = nullptr;
        const DXBuffer* m_src = nullptr;
        CopyCommandList* m_copyCommandList = nullptr;
        UINT64 m_signal = -1;
        jobs::Job* m_done = nullptr;
    };

    JobContext ctx{ this, &dst, &src, new CopyCommandList(m_device.GetValue<DXDevice*>()), -1, done };

    class WaitJob : public jobs::Job
    {
    private:
        JobContext m_jobContext;
    public:
        WaitJob(const JobContext& jobContext) :
            m_jobContext(jobContext)
        {
        }

        void Do() override
        {
            WaitFence waitFence(*m_jobContext.m_dxCopyBuffers->m_copyFence.GetValue<DXFence*>());
            waitFence.Wait(m_jobContext.m_signal);

            jobs::RunSync(m_jobContext.m_done);
            delete m_jobContext.m_copyCommandList;
        }
    };

    class CopyJob : public jobs::Job
    {
    private:
        JobContext m_jobContext;
    public:
        CopyJob(const JobContext& jobContext) :
            m_jobContext(jobContext)
        {
        }

        void Do() override
        {
            DXFence* fence = m_jobContext.m_dxCopyBuffers->m_copyFence.GetValue<DXFence*>();
            DXCopyCommandQueue* commandQueue = m_jobContext.m_dxCopyBuffers->m_copyCommandQueue.GetValue<DXCopyCommandQueue*>();
            UINT64 signal = m_jobContext.m_dxCopyBuffers->m_copyCounter++;
            m_jobContext.m_signal = signal;

            m_jobContext.m_copyCommandList->Execute(*m_jobContext.m_dst, *m_jobContext.m_src, commandQueue, fence->GetFence(), signal);
            jobs::RunAsync(new WaitJob(m_jobContext));
        }
    };

    m_copyJobSytem.GetValue<jobs::JobSystem*>()->ScheduleJob(new CopyJob(ctx));
}

void rendering::DXCopyBuffers::Execute(ID3D12CommandList* const* lists, UINT64 numLists, jobs::Job* done)
{
    struct JobContext
    {
        DXCopyBuffers* m_dxCopyBuffers = nullptr;
        ID3D12CommandList* const* m_lists = nullptr;
        UINT64 m_numLists = 0;
        UINT64 m_signal = -1;
        jobs::Job* m_done = nullptr;
    };

    JobContext ctx{ this, lists, numLists, -1, done };

    class WaitJob : public jobs::Job
    {
    private:
        JobContext m_jobContext;
    public:
        WaitJob(const JobContext& jobContext) :
            m_jobContext(jobContext)
        {
        }

        void Do() override
        {
            WaitFence waitFence(*m_jobContext.m_dxCopyBuffers->m_copyFence.GetValue<DXFence*>());
            waitFence.Wait(m_jobContext.m_signal);

            jobs::RunSync(m_jobContext.m_done);
        }
    };

    class CopyJob : public jobs::Job
    {
    private:
        JobContext m_jobContext;
    public:
        CopyJob(const JobContext& jobContext) :
            m_jobContext(jobContext)
        {
        }

        void Do() override
        {
            DXFence* fence = m_jobContext.m_dxCopyBuffers->m_copyFence.GetValue<DXFence*>();
            DXCopyCommandQueue* commandQueue = m_jobContext.m_dxCopyBuffers->m_copyCommandQueue.GetValue<DXCopyCommandQueue*>();
            UINT64 signal = m_jobContext.m_dxCopyBuffers->m_copyCounter++;

            m_jobContext.m_signal = signal;
            jobs::RunAsync(new WaitJob(m_jobContext));

            commandQueue->GetCommandQueue()->ExecuteCommandLists(m_jobContext.m_numLists, m_jobContext.m_lists);
            commandQueue->GetCommandQueue()->Signal(fence->GetFence(), signal);
        }
    };

    m_copyJobSytem.GetValue<jobs::JobSystem*>()->ScheduleJob(new CopyJob(ctx));
}


void rendering::DXCopyBuffers::Load(jobs::Job* done)
{
    m_device.AssignObject(core::utils::GetDevice());
    m_copyCommandQueue.AssignObject(core::utils::GetCopyCommandQueue());
    
    jobs::JobSystem* copyJS =
        static_cast<jobs::JobSystem*>(ObjectValueContainer::GetObjectOfType(*m_copyJobSystemDef.GetType<const TypeDef*>()));
    copyJS->Start();
    m_copyJobSytem.AssignObject(copyJS);

    DXFence* copyFence =
        static_cast<DXFence*>(ObjectValueContainer::GetObjectOfType(*m_copyFenceDef.GetType<const TypeDef*>()));

    m_copyFence.AssignObject(copyFence);

    copyFence->Load(done);
}


#undef THROW_ERROR
