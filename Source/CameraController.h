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

private:
    DirectX::XMFLOAT3 target = { 0,0,0 };       //ターゲットの位置
    DirectX::XMFLOAT3 angle = { 0,0,0 };        //カメラの角度
    float range = 10.0f;                        //一定距離
};