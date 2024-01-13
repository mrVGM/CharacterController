#include "PlayerController.h"

#include "Camera.h"
#include "Input.h"

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
    using namespace math;

    runtime::Input& input = runtime::GetInput();

    rendering::renderer::Camera* cam = m_camera.GetValue<rendering::renderer::Camera*>();

    Vector3 right, fwd, up;
    cam->GetCoordinateVectors(right, fwd, up);

    Vector3 hDir = fwd;
    hDir.m_coefs[1] = 0;
    hDir = hDir.Normalize();

    float azm = 180 * acos(Dot(hDir, Vector3{ 1,0,0 })) / M_PI;
    if (hDir.m_coefs[2] < 0)
    {
        azm *= -1;
    }
    float alt = 180 * acos(Dot(fwd, Vector3{ 0,1,0 })) / M_PI;

    float azmChange = dt * -input.m_mouseAxis[0] * 50;
    float altChange = dt * input.m_mouseAxis[1] * 50;

    azm += azmChange;
    alt += altChange;

    if (alt < 10)
    {
        alt = 10;
    }

    if (alt > 170)
    {
        alt = 170;
    }

    azm *= M_PI / 180;
    alt *= M_PI / 180;

    Vector3 dir{ cos(azm), 0, sin(azm) };
    dir = sin(alt) * dir;
    dir.m_coefs[1] = cos(alt);

    cam->m_target = cam->m_position + dir;
}

void game::PlayerController::PrepareForNextTick()
{
    rendering::renderer::Camera* cam = m_camera.GetValue<rendering::renderer::Camera*>();
    cam->Update();
}
