#pragma once

#include<DirectXMath.h>

//�J��������
class CameraController
{
    // ���[�h
    enum class Mode
    {
        FreeCamera,		// �t���[�J����
        LockonCamera,	// ���b�N�I���J����
    };
public:
    CameraController() {}
    ~CameraController() {}

    //�X�V����
    void Update(float elapsedTime);

    void LockonCamera(float elapsedTime);
    void FreeCamera(float elapsedTime);
    //�^�[�Q�b�g�̈ʒu�ݒ�
    void SetTarget(const DirectX::XMFLOAT3& t) { this->target = t; }

    void SetRange(const float& r) { this->range = r; }

    const bool GetMouseMoveFlag()const { return mouseMoveFlag; }
    void SetMouseMoveFlag(const bool& flag) { this->mouseMoveFlag = flag; }

    void SetCutInFlag(const bool& flag) { this->cutInFlag = flag; }
    void DrawDebugGUI();
private:
    Mode mode = Mode::FreeCamera;
    DirectX::XMFLOAT3 target = { 0,0,0 };       //�^�[�Q�b�g�̈ʒu
    DirectX::XMFLOAT3 angle = { 0,0,0 };        //�J�����̊p�x
    float range = 10.0f;                        //��苗��
    DirectX::XMFLOAT3 cameraAngle = { 0,0,0 };
    float sensi = 0.00001f;
    bool mouseMoveFlag = true;
    float targetY = 0;
    bool cutInFlag = false;

    DirectX::XMFLOAT3	newPosition = { 0, 0, 0 };
    DirectX::XMFLOAT3	newTarget = { 0, 0, 0 };
    DirectX::XMFLOAT3	targetWork[2] = { { 0, 0, 0 }, { 0, 0, 0 } };	// 0 : ���W, 1 : �����_
    float				lengthLimit[2] = { 5, 7 };
    float				sideValue = 1;
};