#include "CameraController.h"
#include "Camera.h"
#include "Input/InputManager.h"

#ifdef USE_IMGUI
#include "../Imgui/imgui.h"
#include "../Imgui/imgui_internal.h"
#include "../Imgui/imgui_impl_dx11.h"
#include "../Imgui/imgui_impl_win32.h"
#endif
//更新処理
void CameraController::Update(float elapsedTime)
{
    switch (mode)
    {
    case	Mode::FreeCamera:	FreeCamera(elapsedTime);	break;
    case	Mode::LockonCamera:	LockonCamera(elapsedTime);	break;
    }
    //LockOnMode(elapsedTime);
    //カメラの回転値を回転行列に変換
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(cameraAngle.x, cameraAngle.y, cameraAngle.z);

    //カメラの回転行列から前方向ベクトルを取り出す
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Transform.r[2]);

    //注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める(eye = target - front * range)
    DirectX::XMFLOAT3 eye;
    target.y += targetY;
    eye.x = target.x - front.x * range;
    eye.y = target.y - front.y * range;
    eye.z = target.z - front.z * range;

    //カメラの視点と注視点を設定
    Camera::Instance().SetLookAt(eye, target, { 0,1,0 });
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
        ImGui::SliderFloat("targetY", &targetY, 0, 10);// カメラをキャラの中心ではなく自由に設定できるように
    }
    ImGui::End();
#endif // USE_IMGUI
}

void CameraController::FreeCamera(float elapsedTime)
{
    //入力情報を取得
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    Mouse* mouse = InputManager::Instance()->getMouse();

    POINT mousePosition;
    GetCursorPos(&mousePosition);

    // 画面中央
    const float centerX = 1920 / 2;
    const float centerY = 1080 / 2;

    //　マウス移動量
    float deltaX = static_cast<float>(mousePosition.x - centerX);
    float deltaY = static_cast<float>(mousePosition.y - centerY);
    if (mouseMoveFlag)
    {
        // マウスカーソルを中央に戻す
        SetCursorPos(centerX, centerY);
    }
    //カメラの回転速度
    constexpr float rollSpeed = DirectX::XMConvertToRadians(90);
    float speed = rollSpeed * elapsedTime;
    float ax = gamePad->GetAxisRX();
    float ay = gamePad->GetAxisRY();

    // コントローラーでカメラ操作する場合
    if (ax != 0 || ay != 0)
    {
        cameraAngle.x += ay * speed;
        cameraAngle.y += ax * speed;
    }
    // マウスでカメラ操作
    else
    {
        cameraAngle.y += deltaX * sensi * 0.5f;
        cameraAngle.x += deltaY * sensi * 0.5f;
    }
    //X軸のカメラ回転を上下45度に制限
    constexpr float maxAngle = DirectX::XMConvertToRadians(45);
    constexpr float minAngle = DirectX::XMConvertToRadians(-45);

    //X軸のカメラ回転を制限
    if (cameraAngle.x < minAngle)
    {
        cameraAngle.x = minAngle;
    }

    if (cameraAngle.x > maxAngle)
    {
        cameraAngle.x = maxAngle;
    }

    //Y軸の回転値を-3.14~3.14に収まるようにする
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
void CameraController::LockonCamera(float elapsedTime)
{
    //	後方斜に移動させる
    DirectX::XMVECTOR	t0 = DirectX::XMVectorSet(targetWork[0].x, 0.5f, targetWork[0].z, 0);
    DirectX::XMVECTOR	t1 = DirectX::XMVectorSet(targetWork[1].x, 0.5f, targetWork[1].z, 0);
    DirectX::XMVECTOR	crv = DirectX::XMLoadFloat3(&Camera::Instance().GetRight());
    DirectX::XMVECTOR	cuv = DirectX::XMVectorSet(0, 1, 0, 0);
    DirectX::XMVECTOR	v = DirectX::XMVectorSubtract(t1, t0);
    DirectX::XMVECTOR	l = DirectX::XMVector3Length(v);

    t0 = DirectX::XMLoadFloat3(&targetWork[0]);
    t1 = DirectX::XMLoadFloat3(&targetWork[1]);

    //	新しい注視点を算出
    DirectX::XMStoreFloat3(&newTarget, DirectX::XMVectorMultiplyAdd(v, DirectX::XMVectorReplicate(0.5f), t0));

    //	新しい座標を算出
    l = DirectX::XMVectorClamp(l
        , DirectX::XMVectorReplicate(lengthLimit[0])
        , DirectX::XMVectorReplicate(lengthLimit[1]));
    t0 = DirectX::XMVectorMultiplyAdd(l, DirectX::XMVector3Normalize(DirectX::XMVectorNegate(v)), t0);
    t0 = DirectX::XMVectorMultiplyAdd(crv, DirectX::XMVectorReplicate(sideValue * 3.0f), t0);
    t0 = DirectX::XMVectorMultiplyAdd(cuv, DirectX::XMVectorReplicate(3.0f), t0);
    DirectX::XMStoreFloat3(&newPosition, t0);
}