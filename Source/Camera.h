#pragma once

#include<DirectXMath.h>

//カメラ
class Camera
{
private:
    Camera() {}
    ~Camera() {}

public:
    static Camera& Instance()
    {
        static Camera ins;
        return ins;
    }

    //カメラの位置と姿勢の設定
    void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

    //カメラのレンズの設定
    void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

    //ビュー行列取得
    const DirectX::XMFLOAT4X4& GetView()const { return view; }

    //プロジェクション行列
    const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

    //視点取得
    const DirectX::XMFLOAT3& GetEye() const { return eye; }

    //注視点取得
    const DirectX::XMFLOAT3& GetFocus() const { return focus; }

    //右方向ベクトル取得
    const DirectX::XMFLOAT3& GetRight() const { return right; }

    //前方向ベクトル取得
    const DirectX::XMFLOAT3& GetFront()const { return front; }

    //上方向ベクトル取得
    const DirectX::XMFLOAT3& GetUp()const { return up; }
private:
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;

    DirectX::XMFLOAT3 eye;      //カメラの視点(カメラ位置)
    DirectX::XMFLOAT3 focus;    //注視点

    DirectX::XMFLOAT3 right;    //右方向ベクトル
    DirectX::XMFLOAT3 up;       //上方向ベクトル
    DirectX::XMFLOAT3 front;    //前方向ベクトル
};