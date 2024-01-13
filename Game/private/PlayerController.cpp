#include "PlayerController.h"

#include "Camera.h"
#include "Input.h"
#include "RenderWindow.h"
#include "SceneObject.h"
#include "Character.h"

#include "ValueList.h"
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
    m_camera(rendering::renderer::CameraTypeDef::GetTypeDef(), this),
    m_scene(scene::SceneObjectTypeDef::GetTypeDef(), this),
    m_character(CharacterTypeDef::GetTypeDef(), this)
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
        ObjectValueContainer::GetObjectOfType(scene::SceneObjectTypeDef::GetTypeDef(), m_scene);
        jobs::RunAsync(loadCam);
    });

    jobs::RunSync(init);
}

game::PlayerController::~PlayerController()
{
}

void game::PlayerController::Tick(double dt)
{
    runtime::Input& input = runtime::GetInput();
    
    Character* character = m_character.GetValue<Character*>();
    if (!character && input.m_keysDown.contains(32))
    {
        scene::SceneObject* scene = m_scene.GetValue<scene::SceneObject*>();
        
        ValueList* actors = scene->GetActors().GetValue<ValueList*>();
        for (auto it = actors->GetIterator(); it; ++it)
        {
            Value& cur = *it;
            runtime::Actor* actor = cur.GetValue<runtime::Actor*>();
            if (TypeDef::IsA(actor->GetTypeDef(), CharacterTypeDef::GetTypeDef()))
            {
                m_character = cur;
                character = m_character.GetValue<Character*>();
                break;
            }
        }
    }

    if (character)
    {
        ControllCharacter(dt);
    }
    else
    {
        FreeMove(dt);
    }

}

void game::PlayerController::FreeMove(double dt)
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

void game::PlayerController::ControllCharacter(double dt)
{
    using namespace math;

    const float aimSpeed = 100;
    const float moveSpeed = 6;

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

    azm += azmChange;
    alt += altChange;

    if (m_justPossesed)
    {
        m_justPossesed = false;

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
    move = moveSpeed * move.Normalize();

    azm *= M_PI / 180;
    alt *= M_PI / 180;

    Vector3 dir{ cos(azm), 0, sin(azm) };
    dir = sin(alt) * dir;
    dir.m_coefs[1] = cos(alt);

    cam->m_target = cam->m_position + dir;
    cam->GetCoordinateVectors(right, fwd, up);

    Vector3 charFwd = fwd;
    charFwd.m_coefs[1] = 0;
    charFwd = charFwd.Normalize();

    Vector3 charRight = Vector3{ 0,1,0 } ^ charFwd;

    Character* character = m_character.GetValue<Character*>();
    HandleCharMove(dt, move.m_coefs[1] * charRight + move.m_coefs[0] * charFwd);

    cam->m_target = character->m_curTransform.m_position + Vector3{ 0, 1, 0 };
    cam->m_position = cam->m_target + -3 * fwd;
}

void game::PlayerController::HandleCharMove(float dt, const math::Vector3& velocity)
{
    using namespace math;

    const float acc = 30;

    float newSpeed = Dot(velocity, velocity);
    newSpeed = sqrt(newSpeed);

    Character* character = m_character.GetValue<Character*>();

    float curSpeed = Dot(character->m_velocity, character->m_velocity);
    curSpeed = sqrt(curSpeed);

    float coef = 1;
    if (coef > newSpeed + GetFloatEPS())
    {
        coef = -1;
    }
    
    curSpeed += coef * acc * dt;

    if (curSpeed < 0)
    {
        curSpeed = 0;
    }
    if (curSpeed > newSpeed)
    {
        curSpeed = newSpeed;
    }

    Vector3 velNorm = velocity.Normalize();
    character->m_velocity = curSpeed * velNorm;
    character->m_curTransform.m_position = character->m_curTransform.m_position + dt * character->m_velocity;

    HandleCharRotation(dt, character->m_velocity);
}

void game::PlayerController::HandleCharRotation(float dt, const math::Vector3& velocity)
{
    using namespace math;
    const float angleSpeed = 2000;

    Character* character = m_character.GetValue<Character*>();

    Vector3 velNorm = velocity.Normalize();
    if (Dot(velNorm, velNorm) < GetFloatEPS())
    {
        return;
    }

    Vector3 fwd = velNorm;
    fwd.m_coefs[1] = 0;
    fwd = fwd.Normalize();
    Vector3 up{ 0,1,0 };
    Vector3 right = up ^ fwd;

    Matrix tmp
    {
        right.m_coefs[0],   up.m_coefs[0],  fwd.m_coefs[0], 0,
        right.m_coefs[1],   up.m_coefs[1],  fwd.m_coefs[1], 0,
        right.m_coefs[2],   up.m_coefs[2],  fwd.m_coefs[2], 0,
        0,                  0,              0,              1
    };

    Transform tr = tmp.ToTransform();
    Vector4 desiredRot = tr.m_rotation;

    Vector3 curFwd = character->m_curTransform.m_rotation.Rotate(Vector3{ 0, 0, 1 });
    float cosAngle = Dot(curFwd, fwd);
    if (cosAngle < -1)
    {
        cosAngle = -1;
    }
    if (cosAngle > 1)
    {
        cosAngle = 1;
    }

    float angleOffset = 2 * acos(cosAngle);
    angleOffset = 180 * angleOffset / M_PI;

    if (angleOffset < GetFloatEPS())
    {
        return;
    }

    float alpha = dt * angleSpeed / angleOffset;
    if (alpha > 1)
    {
        character->m_curTransform.m_rotation = desiredRot;
        return;
    }

    character->m_curTransform.m_rotation = SLerp(character->m_curTransform.m_rotation, desiredRot, alpha);
}

void game::PlayerController::PrepareForNextTick()
{
    rendering::renderer::Camera* cam = m_camera.GetValue<rendering::renderer::Camera*>();
    cam->Update();
}
