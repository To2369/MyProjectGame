#pragma once

#include<DirectXMath.h>

//カメラ操作
class CameraController
{
public:
    CameraController() {}
    ~CameraController() {}

    //更新処理
    void Update(float elapsedTime);

    //ターゲットの位置設定
    void SetTarget(const DirectX::XMFLOAT3& t) { this->target = t; }

    void SetRange(const float& r) { this->range = r; }

    const bool GetMouseMoveFlag()const { return mouseMoveFlag; }
    void SetMouseMoveFlag(const bool& flag) { this->mouseMoveFlag = flag; }

    void SetCutInFlag(const bool& flag) { this->cutInFlag = flag; }
    void DrawDebugGUI();
private:
    DirectX::XMFLOAT3 target = { 0,0,0 };       //ターゲットの位置
    DirectX::XMFLOAT3 angle = { 0,0,0 };        //カメラの角度
    float range = 10.0f;                        //一定距離
    DirectX::XMFLOAT3 cameraAngle = { 0,0,0 };
    float sensi = 0.00001f;
    bool mouseMoveFlag = true;
    float targetY = 0;
    bool cutInFlag = false;
};