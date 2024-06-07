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

private:
    DirectX::XMFLOAT3 target = { 0,0,0 };       //�^�[�Q�b�g�̈ʒu
    DirectX::XMFLOAT3 angle = { 0,0,0 };        //�J�����̊p�x
    float range = 10.0f;                        //��苗��
};