#include "PlayerController.h"

#include "Camera.h"

#include "Jobs.h"

#include "ObjectValueContainer.h"

namespace
{
    BasicObjectContainer<game::PlayerControllerTypeDef> m_playerController;
}

const game::PlayerControllerTypeDef& game::PlayerControllerTypeDef::GetTypeDef()
{
    if (!m_playerController.m_object)
    {
        m_playerController.m_object = new PlayerControllerTypeDef();
    }

    return *m_playerController.m_object;
}

game::PlayerControllerTypeDef::PlayerControllerTypeDef() :
    ReferenceTypeDef(&runtime::ActorTypeDef::GetTypeDef(), "1883D2B8-49B9-494F-94EE-3742CAC5918B")
{
    m_name = "Player Controller";
    m_category = "Game";
}

game::PlayerControllerTypeDef::~PlayerControllerTypeDef()
{
}

void game::PlayerControllerTypeDef::Construct(Value& value) const
{
    PlayerController* pc = new PlayerController(*this);
    value.AssignObject(pc);
}

game::PlayerController::PlayerController(const ReferenceTypeDef& typeDef) :
    Actor(typeDef),
    m_camera(rendering::renderer::CameraTypeDef::GetTypeDef(), this)
{
}

void game::PlayerController::LoadData(jobs::Job* done)
{
    jobs::Job* loadCam = jobs::Job::CreateByLambda([=]() {
        rendering::renderer::Camera* cam = m_camera.GetValue<rendering::renderer::Camera*>();
        cam->Load(done);
    });

    jobs::Job* init = jobs::Job::CreateByLambda([=]() {
        ObjectValueContainer::GetObjectOfType(rendering::renderer::CameraTypeDef::GetTypeDef(), m_camera);
        jobs::RunAsync(loadCam);
    });

    jobs::RunSync(init);
}

game::PlayerController::~PlayerController()
{
}

void game::PlayerController::Tick(double dt)
{
}

void game::PlayerController::PrepareForNextTick()
{
    rendering::renderer::Camera* cam = m_camera.GetValue<rendering::renderer::Camera*>();
    cam->Update();
}
