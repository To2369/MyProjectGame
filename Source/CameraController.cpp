#include "CameraController.h"
#include "Camera.h"
#include "Input/InputManager.h"

#ifdef USE_IMGUI
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_internal.h"
#include "../Imgui/imgui_impl_dx11.h"
#include "../Imgui/imgui_impl_win32.h"
//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//extern ImWchar glyphRangesJapanese[];
#endif
//�X�V����
void CameraController::Update(float elapsedTime)
{

    //���͏����擾
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    Mouse* mouse = InputManager::Instance()->getMouse();

    DirectX::XMFLOAT2 mousePosition = { (float)mouse->GetPositionX(),(float)mouse->GetPositionY() };
    DirectX::XMFLOAT2 mouseOldPosition = { (float)mouse->GetOldPositionX(),(float)mouse->GetOldPositionY() };
    float ax = gamePad->GetAxisRX();
    float ay = gamePad->GetAxisRY();
    if (mouseMoveFlag)
    {
        //�J�����̉�]���x
        constexpr float rollSpeed = DirectX::XMConvertToRadians(90);
        float speed = rollSpeed * elapsedTime;

        // �R���g���[���[�ŃJ�������삷��ꍇ
        if (ax!=0||ay!=0)
        {
            cameraAngle.x += ay * speed;
            cameraAngle.y += ax * speed;
        }
        // �}�E�X�ŃJ��������
        else
        {
            cameraAngle.y += (mousePosition.x - mouseOldPosition.x) * sensi;
            cameraAngle.x += (mousePosition.y - mouseOldPosition.y) * sensi;
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
    }
    //�J�����̉�]�l����]�s��ɕϊ�
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(cameraAngle.x, cameraAngle.y, cameraAngle.z);

    //�J�����̉�]�s�񂩂�O�����x�N�g�������o��
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Transform.r[2]);

    //�����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�(eye = target - front * range)
    DirectX::XMFLOAT3 eye;
    target.y += targetY;
    eye.x = target.x - front.x * range;
    eye.y = target.y - front.y * range;
    eye.z = target.z - front.z * range;

    //�J�����̎��_�ƒ����_��ݒ�
    Camera::Instance().SetLookAt(eye, target, { 0,1,0 });
}

void CameraController::DrawDebugGUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_None))
    {
        ImGui::SliderFloat3("cameraAngle", &cameraAngle.x, -3, 3);
        ImGui::SliderFloat("range", &range,1,10);
        ImGui::SliderFloat("targetY", &targetY, 0, 10);// �J�������L�����̒��S�ł͂Ȃ����R�ɐݒ�ł���悤��
    }
    ImGui::End();
}