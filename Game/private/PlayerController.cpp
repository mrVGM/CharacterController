#include "PlayerController.h"

#include "Camera.h"
#include "Input.h"
#include "RenderWindow.h"

#include "Jobs.h"

#include "ObjectValueContainer.h"

#include <Windows.h>

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

    const float aimSpeed = 100;
    const float moveSpeed = 10;

    runtime::Input& input = runtime::GetInput();

    rendering::renderer::Camera* cam = m_camera.GetValue<rendering::renderer::Camera*>();
    rendering::WindowObj* wnd = cam->m_window.GetValue<rendering::WindowObj*>();

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


    float azmChange = dt * -input.m_mouseAxis[0] * aimSpeed;
    float altChange = dt * input.m_mouseAxis[1] * aimSpeed;

    azm += input.m_rbmDown * azmChange;
    alt += input.m_rbmDown * altChange;

    if (input.m_rbmDown)
    {
        RECT rect;
        GetWindowRect(wnd->m_hwnd, &rect);
        int offsetH = wnd->m_width.Get<int>() / 3;
        int offsetV = wnd->m_height.Get<int>() / 3;

        rect.left += offsetH;
        rect.right -= offsetH;
        rect.top += offsetV;
        rect.bottom -= offsetV;

        ClipCursor(&rect);
    }
    else
    {
        ClipCursor(nullptr);
    }

    if (alt < 10)
    {
        alt = 10;
    }

    if (alt > 170)
    {
        alt = 170;
    }

    Vector2 move{ 0, 0 };
    if (input.m_keysDown.contains(87)) // W
    {
        move.m_coefs[0] += 1;
    }
    if (input.m_keysDown.contains(65)) // A
    {
        move.m_coefs[1] += -1;
    }
    if (input.m_keysDown.contains(83)) // S
    {
        move.m_coefs[0] += -1;
    }
    if (input.m_keysDown.contains(68)) // D
    {
        move.m_coefs[1] += 1;
    }

    if (input.m_keysDown.size() > 0)
    {
        std::wstringstream ss;
        ss << *input.m_keysDown.begin() << std::endl;
        OutputDebugString(ss.str().c_str());
    }
    move = dt * moveSpeed * move;

    azm *= M_PI / 180;
    alt *= M_PI / 180;

    Vector3 dir{ cos(azm), 0, sin(azm) };
    dir = sin(alt) * dir;
    dir.m_coefs[1] = cos(alt);

    cam->m_target = cam->m_position + dir;
    cam->GetCoordinateVectors(right, fwd, up);

    cam->m_position = cam->m_position + move.m_coefs[0] * fwd + move.m_coefs[1] * right;
    cam->m_target = cam->m_position + fwd;
}

void game::PlayerController::PrepareForNextTick()
{
    rendering::renderer::Camera* cam = m_camera.GetValue<rendering::renderer::Camera*>();
    cam->Update();
}
