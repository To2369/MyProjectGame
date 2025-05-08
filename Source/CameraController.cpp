#include "CameraController.h"
#include "Camera.h"
#include "Input/InputManager.h"
#include "Messenger.h"
#include "MessageData.h"
#include "StageManager.h"
#ifdef USE_IMGUI
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_internal.h"
#include "../Imgui/imgui_impl_dx11.h"
#include "../Imgui/imgui_impl_win32.h"
#endif

CameraController::CameraController()
{
    position = Camera::Instance().GetEye();
    newPosition = Camera::Instance().GetEye();
    CAMERACHANGEFREEMODEKEY = Messenger::Instance().AddReceiver(MessageData::CAMERACHANGEFREEMODE, [&](void* data) { OnFreeMode(data); });
    CAMERACHANGELOCKONMODEKEY = Messenger::Instance().AddReceiver(MessageData::CAMERACHANGELOCKONMODE, [&](void* data) { OnLockonMode(data); });
   /* CAMERACHANGEMOTIONMODEKEY = Messenger::Instance().AddReceiver(MessageData::CAMERACHANGEMOTIONMODE, [&](void* data) { OnMotionMode(data); });
    CAMERASHAKEKEY = Messenger::Instance().AddReceiver(MessageData::CAMERASHAKE, [&](void* data) { OnShake(data); });*/
}

//�X�V����
void CameraController::Update(float elapsedTime)
{
    switch (mode)
    {
    case	Mode::FreeCamera:	FreeCamera(elapsedTime);	break;
    case	Mode::LockonCamera:	LockonCamera(elapsedTime);	break;
    }
    //LockOnMode(elapsedTime);

    // ���X�ɖڕW�ɋ߂Â���
    static	constexpr	float	Speed = 1.0f / 8.0f;
    position.x += (newPosition.x - position.x) * Speed;
    position.y += (newPosition.y - position.y) * Speed;
    position.z += (newPosition.z - position.z) * Speed;
    target.x += (newTarget.x - target.x) * Speed;
    target.y += (newTarget.y - target.y) * Speed;
    target.z += (newTarget.z - target.z) * Speed;

    //�J�����̎��_�ƒ����_��ݒ�
    Camera::Instance().SetLookAt(position, target, { 0,1,0 });
}

void CameraController::DrawDebugGUI()
{

#ifdef USE_IMGUI

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_None))
    {
        ImGui::SliderFloat3("cameraAngle", &cameraAngle.x, -3, 3);
        ImGui::SliderFloat("range", &range,1,10);
        ImGui::SliderFloat("targetY", &targetY, 0, 10);// �J�������L�����̒��S�ł͂Ȃ����R�ɐݒ�ł���悤��
    }
    ImGui::End();
#endif // USE_IMGUI
}

void CameraController::FreeCamera(float elapsedTime)
{
    //���͏����擾
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    Mouse* mouse = InputManager::Instance()->getMouse();

    POINT mousePosition;
    GetCursorPos(&mousePosition);

    // ��ʒ���
    const float centerX = 1920 / 2;
    const float centerY = 1080 / 2;

    //�@�}�E�X�ړ���
    float deltaX = static_cast<float>(mousePosition.x - centerX);
    float deltaY = static_cast<float>(mousePosition.y - centerY);
    if (mouseMoveFlag)
    {
        // �}�E�X�J�[�\���𒆉��ɖ߂�
        SetCursorPos(centerX, centerY);
    }
    //�J�����̉�]���x
    constexpr float rollSpeed = DirectX::XMConvertToRadians(90);
    float speed = rollSpeed * elapsedTime;
    float ax = gamePad->GetAxisRX();
    float ay = gamePad->GetAxisRY();

    // �R���g���[���[�ŃJ�������삷��ꍇ
    if (ax != 0 || ay != 0)
    {
        cameraAngle.x += ay * speed;
        cameraAngle.y += ax * speed;
    }
    // �}�E�X�ŃJ��������
    else
    {
        cameraAngle.y += deltaX * sensi * 0.5f;
        cameraAngle.x += deltaY * sensi * 0.5f;
    }
    //X���̃J������]���㉺45�x�ɐ���
    constexpr float maxAngle = DirectX::XMConvertToRadians(45);
    constexpr float minAngle = DirectX::XMConvertToRadians(-45);

    //X���̃J������]�𐧌�
    if (cameraAngle.x < minAngle)
    {
        cameraAngle.x = minAngle;
    }

    if (cameraAngle.x > maxAngle)
    {
        cameraAngle.x = maxAngle;
    }

    //Y���̉�]�l��-3.14~3.14�Ɏ��܂�悤�ɂ���
    if (cameraAngle.y < -DirectX::XM_PI)
    {
        cameraAngle.y += DirectX::XM_2PI;
    }
    if (cameraAngle.y > DirectX::XM_PI)
    {
        cameraAngle.y -= DirectX::XM_2PI;
    }

    if (cutInFlag)
    {
        cameraAngle.x = 0.5f;
        cameraAngle.y = 3.0f;
        range = 2;
        targetY = 1;
        sensi = 0;
    }
    else
    {
        range = 10.0f;
        targetY = 1;
        sensi = 0.005f;
    }

    //�J�����̉�]�l����]�s��ɕϊ�
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(cameraAngle.x, cameraAngle.y, cameraAngle.z);

    //�J�����̉�]�s�񂩂�O�����x�N�g�������o��
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Transform.r[2]);

    //�����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�(eye = target - front * range)
    newPosition.x = target.x - front.x * range;
    newPosition.y = target.y - front.y * range;
    newPosition.z = target.z - front.z * range;

    // �n�`�Ƃ̓����蔻����s��
    HitResult hitResult;
    if (StageManager::Instance().RayCast(newTarget, newPosition, hitResult))
    {
        DirectX::XMVECTOR	p = DirectX::XMLoadFloat3(&hitResult.position);
        DirectX::XMVECTOR	cuv = DirectX::XMVectorSet(0, 1, 0, 0);
        p = DirectX::XMVectorMultiplyAdd(DirectX::XMVectorReplicate(4), cuv, p);
        DirectX::XMStoreFloat3(&newPosition, p);
    }
}
void CameraController::LockonCamera(float elapsedTime)
{
    //	����΂Ɉړ�������
    DirectX::XMVECTOR	t0 = DirectX::XMVectorSet(targetWork[0].x, 0.5f, targetWork[0].z, 0);
    DirectX::XMVECTOR	t1 = DirectX::XMVectorSet(targetWork[1].x, 0.5f, targetWork[1].z, 0);
    DirectX::XMVECTOR	crv = DirectX::XMLoadFloat3(&Camera::Instance().GetRight());
    DirectX::XMVECTOR	cuv = DirectX::XMVectorSet(0, 1, 0, 0);
    DirectX::XMVECTOR	v = DirectX::XMVectorSubtract(t1, t0);
    DirectX::XMVECTOR	l = DirectX::XMVector3Length(v);

    t0 = DirectX::XMLoadFloat3(&targetWork[0]);
    t1 = DirectX::XMLoadFloat3(&targetWork[1]);

    //	�V���������_���Z�o
    DirectX::XMStoreFloat3(&newTarget, DirectX::XMVectorMultiplyAdd(v, DirectX::XMVectorReplicate(0.5f), t0));

    //	�V�������W���Z�o
    l = DirectX::XMVectorClamp(l
        , DirectX::XMVectorReplicate(lengthLimit[0])
        , DirectX::XMVectorReplicate(lengthLimit[1]));
    t0 = DirectX::XMVectorMultiplyAdd(l, DirectX::XMVector3Normalize(DirectX::XMVectorNegate(v)), t0);
    t0 = DirectX::XMVectorMultiplyAdd(crv, DirectX::XMVectorReplicate(sideValue * 3.0f), t0);
    t0 = DirectX::XMVectorMultiplyAdd(cuv, DirectX::XMVectorReplicate(3.0f), t0);
    DirectX::XMStoreFloat3(&newPosition, t0);
}

void CameraController::OnFreeMode(void* data)
{
    MessageData::CAMERACHANGEFREEMODEDATA* p = static_cast<MessageData::CAMERACHANGEFREEMODEDATA*>(data);
    if (this->mode != Mode::FreeCamera)
    {
        // �p�x�Z�o
        DirectX::XMFLOAT3	v;
        v.x = newPosition.x - newTarget.x;
        v.y = newPosition.y - newTarget.y;
        v.z = newPosition.z - newTarget.z;
        angle.y = atan2f(v.x, v.z) + DirectX::XM_PI;
        angle.x = atan2f(v.y, v.z);
        //	�p�x�̐��K��
        angle.y = atan2f(sinf(angle.y), cosf(angle.y));
        angle.x = atan2f(sinf(angle.x), cosf(angle.x));
    }
    this->mode = Mode::FreeCamera;
    this->newTarget = p->target;
    this->newTarget.y += 0.01f;
}

void CameraController::OnLockonMode(void* data)
{
    MessageData::CAMERACHANGELOCKONMODEDATA* p = static_cast<MessageData::CAMERACHANGELOCKONMODEDATA*>(data);
    if (this->mode != Mode::LockonCamera)
        sideValue = CalcSide(p->start, p->target);

    this->mode = Mode::LockonCamera;
    targetWork[0] = p->start;
    targetWork[1] = p->target;
    targetWork[0].y += 0.01f;
    targetWork[1].y += 0.01f;
}

float CameraController::CalcSide(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2)
{
    // �O�ς�p���ĉ����̃Y�������Z�o
    DirectX::XMFLOAT2	v;
    v.x = position.x - target.x;
    v.y = position.z - target.z;
    float	l = sqrtf(v.x * v.x + v.y * v.y);
    v.y /= l;
    v.y /= l;
    DirectX::XMFLOAT2	n;
    n.x = p1.x - p2.x;
    n.y = p1.z - p2.z;
    l = sqrtf(n.x * n.x + n.y * n.y);
    n.x /= l;
    n.y /= l;
    return	((v.x * n.y) - (v.y * n.x) < 0) ? +1.0f : -1.0f;
}