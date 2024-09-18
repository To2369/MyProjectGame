#include "Character.h"

//�s��X�V
void Character::UpdateTransform()
{
    //�@���W�n�ϊ��p�̍s��
    const DirectX::XMFLOAT4X4 coordinate_system_transform[]
    {
        { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	//0:�E��n Y-UP
        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	    //1:����n Y-UP
        { -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	//2:�E��n Z-UP
        { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	    //3:����n Z-UP
    };
    //�@�f�t�H���g�̃X�P�[���t�@�N�^��ݒ肵�čs��ɔ��f
    const float scale_factor = 1.0f;
    DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transform[0]) * DirectX::XMMatrixScaling(scale_factor,scale_factor,scale_factor) };
    // �X�P�[���s��쐬
    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x,scale.y,scale.z) };
    // ��]�s��쐬
    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(angle.x,angle.y,angle.z) };
    // �ʒu�s��쐬
    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(position.x,position.y,position.z) };

    // �s���g�ݍ��킹�A���[���h�s����쐬
    DirectX::XMMATRIX W= C * S * R * T;
    //�v�Z�������[���h�s���trabsform�Ɏ��o��
    DirectX::XMStoreFloat4x4(&transform, W);

}

//�ړ�����
void Character::Move(float elapsedTime, float vx, float vz, float speed)
{
    //�ړ�����
    float moveSpeed = speed * elapsedTime;
    position.x += vx * moveSpeed;
    position.z += vz * moveSpeed;
}

// ���񏈗�
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
    speed *= elapsedTime;
    //�i�ׂ��i�s�x�N�g�����[���̏ꍇ�͐��񏈗��Ȃ�
    float length = sqrtf(vx * vx + vz * vz);
    if (length < 0.001f)
    {
        return;
    }

    // �i�s�x�N�g���ƒP�ʃx�N�g����
    vx /= length;
    vz /= length;

    // �O�����x�N�g����XZ�������擾���P�ʃx�N�g����
    float frontX = transform._31;
    float frontZ = transform._33;
    float frontLength = sqrtf(frontX * frontX + frontZ * frontZ);
    if (frontLength > 0.0f)
    {
        frontX /= frontLength;
        frontZ /= frontLength;
    }

    // ���E�`�F�b�N�ׂ̈̊O�όv�Z
    float cross = (frontZ * vx) - (frontX * vz);

    // ��]�p�����߂�ׁA2�̒P�ʃx�N�g���̓��ς��v�Z����
    // �Q�̃x�N�g���̓��ϒl��-1.0�`1.0�ŕ\������܂�
    float dot = (frontX * vx) + (frontZ * vz);

    // �Q�̃x�N�g�����d�Ȃ����Ƃ��A��]���x��0.0f�ɂȂ�
    float rot = (1.0f - dot);

    // ���܂藣�ꂷ����Ɖ�]���x�������Ȃ肷����̂� speed �ȏ�̉�]���x�ɂ͂Ȃ�Ȃ��悤����
    if (rot > speed)
    {
        rot = speed;
    }
    // �O�ς����̏ꍇ�͉E��],���̏ꍇ�͍���]
    if (cross < 0.0f)
    {
        angle.y -= rot;
    }
    else
    {
        angle.y += rot;
    }
}
// �W�����v����
void Character::Jump(float speed)
{
    // ������̗͂�ݒ�
    velocity.y = speed;
}
// ���x�����X�V
void Character::UpdateVelocity(float elapsedTime)
{
    // �d�͏����i�t���[���P�ʂŌv�Z�j
    velocity.y += gravity * elapsedTime * 60.0f;

    // �ړ�����
    position.y += velocity.y * elapsedTime;

    // �n�ʔ���
    if (position.y < 0.0f)
    {
        position.y = 0.0f;
        velocity.y = 0.0;

        // ���n����
        if (!groundedFlag)
        {
            onLanding();
        }
        groundedFlag = true;
    }
    else
    {
        groundedFlag = false;
    }
}