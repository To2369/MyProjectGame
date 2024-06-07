#pragma once

#include<DirectXMath.h>

//�J����
class Camera
{
private:
    Camera() {}
    ~Camera() {}

public:
    static Camera* Instance()
    {
        static Camera ins;
        return &ins;
    }

    //�J�����̈ʒu�Ǝp���̐ݒ�
    void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

    //�J�����̃����Y�̐ݒ�
    void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

    //�r���[�s��擾
    const DirectX::XMFLOAT4X4* GetView()const { return &view; }

    //�v���W�F�N�V�����s��
    const DirectX::XMFLOAT4X4* GetProjection() const { return &projection; }

    //���_�擾
    const DirectX::XMFLOAT3* GetEye() const { return &eye; }

    //�����_�擾
    const DirectX::XMFLOAT3* GetFocus() const { return &focus; }

    //�E�����x�N�g���擾
    const DirectX::XMFLOAT3* GetRight() const { return &up; }

    //�O�����x�N�g���擾
    const DirectX::XMFLOAT3* GetFront()const { return &front; }

    //������x�N�g���擾
    const DirectX::XMFLOAT3* GetUp()const { return &up; }
private:
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 projection;

    DirectX::XMFLOAT3 eye;      //�J�����̎��_(�J�����ʒu)
    DirectX::XMFLOAT3 focus;    //�����_

    DirectX::XMFLOAT3 right;    //�E�����x�N�g��
    DirectX::XMFLOAT3 up;       //������x�N�g��
    DirectX::XMFLOAT3 front;    //�O�����x�N�g��
};