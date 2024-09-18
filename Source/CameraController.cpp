#include "CameraController.h"
#include "Camera.h"
#include "Input/InputManager.h"
//�X�V����
void CameraController::Update(float elapsedTime)
{

    //���͏����擾
    GamePad* gamePad = InputManager::Instance()->getGamePad();
    float ax = gamePad->GetAxisRX();
    float ay = gamePad->GetAxisRY();
    //�J�����̉�]���x
    constexpr float rollSpeed = DirectX::XMConvertToRadians(90);
    float speed = rollSpeed * elapsedTime;

    //�X�e�B�b�N�̓��͒l�ɍ��킹�ĉ�]
    angle.x += ay * speed;      //X����]
    angle.y += ax * speed;      //Y����]

    //X���̃J������]���㉺45�x�ɐ���
    constexpr float maxAngle = DirectX::XMConvertToRadians(45);
    constexpr float minAngle = DirectX::XMConvertToRadians(-45);

    //X���̃J������]�𐧌�
    if (angle.x < minAngle)
    {
        angle.x = minAngle;
    }

    if (angle.x > maxAngle)
    {
        angle.x = maxAngle;
    }

    //Y���̉�]�l��-3.14~3.14�Ɏ��܂�悤�ɂ���
    if (angle.y < -DirectX::XM_PI)
    {
        angle.y += DirectX::XM_2PI;
    }
    if (angle.y > DirectX::XM_PI)
    {
        angle.y -= DirectX::XM_2PI;
    }

    //�J�����̉�]�l����]�s��ɕϊ�
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

    //�J�����̉�]�s�񂩂�O�����x�N�g�������o��
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Transform.r[2]);

    //�����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�(eye = target - front * range)
    DirectX::XMFLOAT3 eye;
    eye.x = target.x - front.x * range;
    eye.y = target.y - front.y * range;
    eye.z = target.z - front.z * range;

    //�J�����̎��_�ƒ����_��ݒ�
    Camera::Instance().SetLookAt(eye, target, { 0,1,0 });
}