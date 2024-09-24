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

// �ړ�����������
void Character::Move(float vx, float vz, float speed)
{
    // �ړ������x�N�g��������
    direction.x = vx;
    direction.z = vz;

    // �ő呬�x�ݒ�
    maxMoveSpeed = speed;
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
    // �������x�X�V����
    UpdateVerticalVelocity(elapsedTime);

    // �����ړ��X�V����
    UpdateVerticalMove(elapsedTime);

    // �������x�X�V����
    UpdateHorizontalVelocity(elapsedTime);

    // �����ړ��X�V����
    UpdateHorizontalMove(elapsedTime);
}

// �_���[�W����
bool Character::ApplyDamage(float invincibleTime,int damage)
{
    // �_���[�W���O�ȉ��̏ꍇ�� HP �ɕω��Ȃ�
    if (damage <= 0)
        return false;

    // ���� HP ���O�ȉ��i���S�j�̏ꍇ�� HP �ɕω��Ȃ�
    if (health <= 0)
        return false;

    // ���G���Ԓ��� HP �ɕω��Ȃ�
    if (invincibleTimer > 0.0f)
        return false;

    // �_���[�W���󂯂��ۂɖ��G���Ԃ��Đݒ�
    invincibleTimer = invincibleTime;

    // �_���[�W�v�Z
    health -= damage;

    // �_���[�W�v�Z��̎c�� HP �ŏ�����ω�
    if (health <= 0)
    {
        // ���S
        OnDead();
    }
    else
    {
        // �_���[�W���󂯂�
        OnDamaged();
    }

    return true;
}

// �Ռ���^����
void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
    // ���x�ɗ͂�������
    velocity.x += impulse.x;
    velocity.y += impulse.y;
    velocity.z += impulse.z;
}


// ���G���Ԃ̍X�V
void Character::UpdateInvincibleTimer(float elapsedTime)
{
    if (invincibleTimer > 0.0f)
    {
        invincibleTimer -= elapsedTime;
    }
}

// �������x�X�V����
void Character::UpdateVerticalVelocity(float elapsedTime)
{
    // �d�͏����i�t���[���P�ʂŌv�Z�j
    velocity.y += gravity * elapsedTime * 60.0f;
}

// �����ړ��X�V����
void Character::UpdateVerticalMove(float elapsedTime)
{
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
            OnLanding();
        }
        groundedFlag = true;
    }
    else
    {
        groundedFlag = false;
    }
}

// �������x�X�V����
void Character::UpdateHorizontalVelocity(float elapsedTime)
{
    // ���x�ɗ͂�������Ă�����i0 ����Ȃ�������j�����������s��
    float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    if (length > 0.0f)
    {
        // ���C�́i�t���[���P�ʂŌv�Z�j
        float friction = this->friction * elapsedTime * 60.0f;

        // �󒆂ɂ���Ƃ��͖��C�͂�����
        if (!groundedFlag)
        {
            friction *= airControl;
        }

        if (length > friction)
        {
            // ���C�ɂ�鉡�����̌�������
            float vx = velocity.x / length;
            float vz = velocity.z / length;

            velocity.x -= vx * friction;
            velocity.z -= vz * friction;
        }
        else
        {
            // �������̑��x�����C�͈ȉ��ɂȂ����̂ő��x�𖳌���
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
    }

    // �ő呬�x�ȉ��Ȃ�����������s��
    if (length <= maxMoveSpeed)
    {
        // �����x�N�g�����[���łȂ��Ȃ�����������s��
        // �[���̏ꍇ�͓��͂���Ă��Ȃ�
        float directionLength = sqrtf(direction.x * direction.x + direction.z * direction.z);
        if (directionLength > 0.0f)
        {
            // �����x�i�t���[���P�ʂŌv�Z�j
            float acceleration = this->acceleration * elapsedTime * 60.0f;

            // �󒆂ɂ���Ƃ��͉����x������
            if (!groundedFlag)
            {
                acceleration *= airControl;
            }

            // ��������
            // �����x�N�g���ɉ����x���X�P�[�����O�����l�𑬓x�x�N�g���ɉ��Z
            velocity.x += direction.x * acceleration;
            velocity.z += direction.z * acceleration;

            // �ő呬�x����
            float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
            if (length > maxMoveSpeed)
            {
                // �����x�N�g���ɍő�ړ����x���X�P�[�����O�����l�𑬓x�x�N�g���ɑ��
                velocity.x = direction.x * maxMoveSpeed;
                velocity.z = direction.z * maxMoveSpeed;

            }
        }
    }
}

// �����ړ��X�V����
void Character::UpdateHorizontalMove(float elapsedTime)
{
    // �ړ�����
    position.x += velocity.x * elapsedTime;
    position.z += velocity.z * elapsedTime;
}