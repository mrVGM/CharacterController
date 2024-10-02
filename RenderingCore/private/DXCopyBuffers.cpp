#include "DXCopyBuffers.h"

#include "ObjectValueContainer.h"
#include "PrimitiveTypes.h"

#include "Jobs.h"
#include "JobSystem.h"
#include "WaitFence.h"

#include "DXDevice.h"
#include "DXCommandQueue.h"
#include "DXFence.h"

#include "ObjectValueContainer.h"

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
            rendering::DXCommandQueue* commandQueue,
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
            commandQueue->GetCopyCommandQueue()->ExecuteCommandLists(_countof(copyCommandList), copyCommandList);

            commandQueue->GetCopyCommandQueue()->Signal(fence, signal);
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
    m_commandQueue(DXCommandQueueTypeDef::GetTypeDef(), this),
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
    jobs::Job done)
{
    DXBuffer* dstPtr = &dst;
    const DXBuffer* srcPtr = &src;

    jobs::Job copyJob = [=]() {

		DXFence* fence = m_copyFence.GetValue<DXFence*>();
		DXCommandQueue* commandQueue = m_commandQueue.GetValue<DXCommandQueue*>();
		UINT64 signal = m_copyCounter++;

        jobs::RunAsync([=]() {
            WaitFence waitFence(*m_copyFence.GetValue<DXFence*>());
            waitFence.Wait(signal);

            jobs::RunSync(done);
        });

        CopyCommandList cl(m_device.GetValue<DXDevice*>());
        cl.Execute(*dstPtr, *srcPtr, commandQueue, fence->GetFence(), signal);
    };

    m_copyJobSytem.GetValue<jobs::JobSystem*>()->ScheduleJob(copyJob);
}

void rendering::DXCopyBuffers::Execute(ID3D12CommandList* const* lists, UINT64 numLists, jobs::Job done)
{
    jobs::Job copyJob = [=]() {
		DXFence* fence = m_copyFence.GetValue<DXFence*>();
		DXCommandQueue* commandQueue = m_commandQueue.GetValue<DXCommandQueue*>();
		UINT64 signal = m_copyCounter++;

        jobs::RunAsync([=]() {
            WaitFence waitFence(*fence);
            waitFence.Wait(signal);

            jobs::RunSync(done);
        });

		commandQueue->GetCopyCommandQueue()->ExecuteCommandLists(numLists, lists);
		commandQueue->GetCopyCommandQueue()->Signal(fence->GetFence(), signal);
	};

    m_copyJobSytem.GetValue<jobs::JobSystem*>()->ScheduleJob(copyJob);
}


void rendering::DXCopyBuffers::LoadData(jobs::Job done)
{
    jobs::Job load = [=]() {
        DXFence* copyFence = m_copyFence.GetValue<DXFence*>();
        copyFence->Load(done);
    };

    jobs::Job init = [=]() {
        ObjectValueContainer::GetObjectOfType(DXDeviceTypeDef::GetTypeDef(), m_device);
        ObjectValueContainer::GetObjectOfType(DXCommandQueueTypeDef::GetTypeDef(), m_commandQueue);
        ObjectValueContainer::GetObjectOfType(*m_copyJobSystemDef.GetType<const TypeDef*>(), m_copyJobSytem);
        ObjectValueContainer::GetObjectOfType(*m_copyFenceDef.GetType<const TypeDef*>(), m_copyFence);

        jobs::JobSystem* copyJS = m_copyJobSytem.GetValue<jobs::JobSystem*>();
        copyJS->Start();

        jobs::RunAsync(load);
    };

    jobs::RunSync(init);
}


#undef THROW_ERROR
