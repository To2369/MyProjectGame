#pragma once

#include <DirectXMath.h>
#include <vector>

// カメラモーション情報
struct CameraMotionData
{
	float time;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 target;
};

namespace MessageData
{
    // フリーカメラ
    static constexpr const char* CAMERACHANGEFREEMODE = "CAMERA CHANGE FREEMODE";
    struct CAMERACHANGEFREEMODEDATA
    {
        DirectX::XMFLOAT3 target;
    };

    // ロックオンカメラ
    static constexpr const char* CAMERACHANGELOCKONMODE = "CAMERA CHANGE LOCKONMODE";
    struct CAMERACHANGELOCKONMODEDATA
    {
        DirectX::XMFLOAT3 start;
        DirectX::XMFLOAT3 target;
    };

    // エリアルカメラ
    static constexpr const char* CAMERACHANGEARIELMODE = "CAMERA CHANGE ARIELMODE";
    struct CAMERACHANGEARIELMODEDATA
    {
        DirectX::XMFLOAT3 start;
        DirectX::XMFLOAT3 target;
        DirectX::XMFLOAT3 lockonTarget;
    };

    // モーションカメラ
    static constexpr const char* CAMERACHANGEMOTIONMODE = "CAMERA CHANGE MOTIONMODE";
    struct CAMERACHANGEMOTIONMODEDATA
    {
        std::vector<CameraMotionData> data;
    };

    // カメラ揺れエフェクト
    static constexpr const char* CAMERASHAKE = "CAMERA SHAKE";
    struct CAMERASHAKEDATA
    {
        float shakeTimer;
        float shakePower;
    };
};
