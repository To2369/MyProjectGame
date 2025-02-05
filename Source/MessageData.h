#pragma once

#include <DirectXMath.h>
#include <vector>

// �J�������[�V�������
struct CameraMotionData
{
	float time;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 target;
};

namespace MessageData
{
    // �t���[�J����
    static constexpr const char* CAMERACHANGEFREEMODE = "CAMERA CHANGE FREEMODE";
    struct CAMERACHANGEFREEMODEDATA
    {
        DirectX::XMFLOAT3 target;
    };

    // ���b�N�I���J����
    static constexpr const char* CAMERACHANGELOCKONMODE = "CAMERA CHANGE LOCKONMODE";
    struct CAMERACHANGELOCKONMODEDATA
    {
        DirectX::XMFLOAT3 start;
        DirectX::XMFLOAT3 target;
    };

    // �G���A���J����
    static constexpr const char* CAMERACHANGEARIELMODE = "CAMERA CHANGE ARIELMODE";
    struct CAMERACHANGEARIELMODEDATA
    {
        DirectX::XMFLOAT3 start;
        DirectX::XMFLOAT3 target;
        DirectX::XMFLOAT3 lockonTarget;
    };

    // ���[�V�����J����
    static constexpr const char* CAMERACHANGEMOTIONMODE = "CAMERA CHANGE MOTIONMODE";
    struct CAMERACHANGEMOTIONMODEDATA
    {
        std::vector<CameraMotionData> data;
    };

    // �J�����h��G�t�F�N�g
    static constexpr const char* CAMERASHAKE = "CAMERA SHAKE";
    struct CAMERASHAKEDATA
    {
        float shakeTimer;
        float shakePower;
    };
};
