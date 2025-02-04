#pragma once

#include<DirectXMath.h>

//カメラ操作
class CameraController
{
    // モード
    enum class Mode
    {
        FreeCamera,		// フリーカメラ
        LockonCamera,	// ロックオンカメラ
    };
public:
    CameraController() {}
    ~CameraController() {}

    //更新処理
    void Update(float elapsedTime);

    void LockonCamera(float elapsedTime);
    void FreeCamera(float elapsedTime);
    //ターゲットの位置設定
    void SetTarget(const DirectX::XMFLOAT3& t) { this->target = t; }

    void SetRange(const float& r) { this->range = r; }

    const bool GetMouseMoveFlag()const { return mouseMoveFlag; }
    void SetMouseMoveFlag(const bool& flag) { this->mouseMoveFlag = flag; }

    void SetCutInFlag(const bool& flag) { this->cutInFlag = flag; }
    void DrawDebugGUI();
private:
    Mode mode = Mode::FreeCamera;
    DirectX::XMFLOAT3 target = { 0,0,0 };       //ターゲットの位置
    DirectX::XMFLOAT3 angle = { 0,0,0 };        //カメラの角度
    float range = 10.0f;                        //一定距離
    DirectX::XMFLOAT3 cameraAngle = { 0,0,0 };
    float sensi = 0.00001f;
    bool mouseMoveFlag = true;
    float targetY = 0;
    bool cutInFlag = false;

    DirectX::XMFLOAT3	newPosition = { 0, 0, 0 };
    DirectX::XMFLOAT3	newTarget = { 0, 0, 0 };
    DirectX::XMFLOAT3	targetWork[2] = { { 0, 0, 0 }, { 0, 0, 0 } };	// 0 : 座標, 1 : 注視点
    float				lengthLimit[2] = { 5, 7 };
    float				sideValue = 1;
};