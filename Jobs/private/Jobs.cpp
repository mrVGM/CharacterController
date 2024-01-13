#include "Jobs.h"

#include "AssetTypeDef.h"
#include "ValueList.h"
#include "ObjectValueContainer.h"

#include "JobSystemDef.h"

#include "JSONValue.h"

namespace
{
    Value m_mainJS(jobs::JobSystemDef::GetTypeDef(), nullptr);
    Value m_asyncJS(jobs::JobSystemDef::GetTypeDef(), nullptr);

    jobs::JobSystem* m_mainJobSystem = nullptr;
    jobs::JobSystem* m_asyncJobSystem = nullptr;
}

void jobs::Boot()
{
    JobSystemDef::GetTypeDef();

    JobSystemDef::GetTypeDef().Construct(m_mainJS);
    JobSystemDef::GetTypeDef().Construct(m_asyncJS);

    m_mainJobSystem = m_mainJS.GetValue<jobs::JobSystem*>();
    m_asyncJobSystem = m_asyncJS.GetValue<jobs::JobSystem*>();

    m_mainJobSystem->m_numThreads.m_payload = 1;
    m_asyncJobSystem->m_numThreads.m_payload = 5;

    m_mainJobSystem->Start();
    m_asyncJobSystem->Start();
}

void jobs::RunSync(Job* job)
{
    m_mainJobSystem->ScheduleJob(job);
}

void jobs::RunAsync(Job* job)
{
    m_asyncJobSystem->ScheduleJob(job);
}