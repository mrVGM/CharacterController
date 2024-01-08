#include "PlayerController.h"

#include "Jobs.h"

namespace
{
    BasicObjectContainer<runtime::PlayerControllerTypeDef> m_playerController;
}

const runtime::PlayerControllerTypeDef& runtime::PlayerControllerTypeDef::GetTypeDef()
{
    if (!m_playerController.m_object)
    {
        m_playerController.m_object = new PlayerControllerTypeDef();
    }

    return *m_playerController.m_object;
}

runtime::PlayerControllerTypeDef::PlayerControllerTypeDef() :
    ReferenceTypeDef(&runtime::TickUpdaterTypeDef::GetTypeDef(), "1883D2B8-49B9-494F-94EE-3742CAC5918B")
{
    m_name = "Player Controller";
    m_category = "Game";
}

runtime::PlayerControllerTypeDef::~PlayerControllerTypeDef()
{
}

void runtime::PlayerControllerTypeDef::Construct(Value& value) const
{
    PlayerController* pc = new PlayerController(*this);
    value.AssignObject(pc);
}

runtime::PlayerController::PlayerController(const ReferenceTypeDef& typeDef) :
    TickUpdater(typeDef)
{
}

runtime::PlayerController::~PlayerController()
{
}

bool runtime::PlayerController::IsTicking()
{
    return false;
}

void runtime::PlayerController::Tick(double dt, jobs::Job* done)
{
    jobs::RunSync(done);
}
