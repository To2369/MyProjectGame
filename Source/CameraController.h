#pragma once

#include<DirectXMath.h>

//�J��������
class CameraController
{
public:
    CameraController() {}
    ~CameraController() {}

    //�X�V����
    void Update(float elapsedTime);

    //�^�[�Q�b�g�̈ʒu�ݒ�
    void SetTarget(const DirectX::XMFLOAT3& t) { this->target = t; }

    void SetRange(const float& r) { this->range = r; }

    const bool GetMouseMoveFlag()const { return mouseMoveFlag; }
    void SetMouseMoveFlag(const bool& flag) { this->mouseMoveFlag = flag; }

    void SetCutInFlag(const bool& flag) { this->cutInFlag = flag; }
    void DrawDebugGUI();
private:
    DirectX::XMFLOAT3 target = { 0,0,0 };       //�^�[�Q�b�g�̈ʒu
    DirectX::XMFLOAT3 angle = { 0,0,0 };        //�J�����̊p�x
    float range = 10.0f;                        //��苗��
    DirectX::XMFLOAT3 cameraAngle = { 0,0,0 };
    float sensi = 0.00001f;
    bool mouseMoveFlag = true;
    float targetY = 0;
    bool cutInFlag = false;
};