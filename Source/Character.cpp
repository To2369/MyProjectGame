#include "Character.h"
#include "StageManager.h"
#include "Mathf.h"
//�s��X�V
void Character::UpdateTransform()
{
    // ���W�n�ϊ��p�̍s��
    const DirectX::XMFLOAT4X4 coordinate_system_transform[] =
    {
        { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	//0:�E��n Y-UP
        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	    //1:����n Y-UP
        { -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	//2:�E��n Z-UP
        { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	    //3:����n Z-UP
    };
    // �f�t�H���g�̃X�P�[���t�@�N�^��ݒ肵�čs��ɔ��f
    const float scale_factor = 1.0f;
    DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transform[0]) * DirectX::XMMatrixScaling(scale_factor, scale_factor, scale_factor) };

    // �X�P�[���s��쐬
    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) };


    DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quaternion));
    // �ʒu�s��쐬
    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(position.x, position.y, position.z) };

    // �s���g�ݍ��킹�A���[���h�s����쐬
    DirectX::XMMATRIX W = C * S * R * T;

    // �v�Z�������[���h�s���transform�Ɏ��o��
    DirectX::XMStoreFloat4x4(&transform, W);

    right = { transform._11,transform._12,transform._13 };
    up = { transform._21,transform._22,transform._23 };
    front = { transform._31,transform._32,transform._33 };
}

void Character::UpdateStatus(float elapsedTime)
{
    if (!useSpiritEnergyFlag)
    {
        //�C�͂��񕜂���Ԋu
        energyRecoveryTimer -= elapsedTime;
        if (energyRecoveryTimer <= 0)
        {
            spiritEnergy += 1;
            energyRecoveryTimer = 0.1f;
        }
    }
    else
    {
        //�C�͂��g�p�����ہA���ɉ񕜂��n�߂�܂ł̎���
        energyNoRecoveryTimer -= elapsedTime;
        if (energyNoRecoveryTimer <= 0)
        {
            energyNoRecoveryTimer = 1.0f;
            energyRecoveryTimer = 0.1f;
            useSpiritEnergyFlag = false;
        }
    }


    //�@�ő�l�ƍŏ��l�Œ�
    {
        if (health <= 0)
            health = 0;
        if (health >= maxHealth)
            health = maxHealth;

        if (spiritEnergy <= 0)
            spiritEnergy = 0;
        if (spiritEnergy >= maxSpritEnergy)
            spiritEnergy = maxSpritEnergy;

        if (skillEnergy <= 0)
            skillEnergy = 0;
        if (skillEnergy >= maxSkillEnergy)
            skillEnergy = maxSkillEnergy;
    }
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
    // Y�����̉�]�p���N�H�[�^�j�I���ɕϊ�
    DirectX::XMVECTOR rotationQuat = DirectX::XMQuaternionRotationAxis(
        up, // Y��
        cross < 0.0f ? -rot : rot                     // ��]�����ɉ����ĕ�����ς���
    );

    // ���݂̃N�H�[�^�j�I���ɐV������]���|�����킹��
    DirectX::XMVECTOR currentQuat = DirectX::XMLoadFloat4(&quaternion);
    currentQuat = DirectX::XMQuaternionMultiply(currentQuat, rotationQuat);

    // ���ʂ�ۑ�
    DirectX::XMStoreFloat4(&quaternion, currentQuat);
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
    // ���������̈ړ���
    float moveY = velocity.y * elapsedTime;

    slopeRate = 0.0f;

    // �p������p�@���x�N�g���i�f�t�H���g�͏�����j
    DirectX::XMFLOAT3 normal = { 0,1,0 };
    if (moveY < 0.0f)
    {
        // ������

        // ���C�̊J�n�ʒu��ݒ�i�����Ƃ�菭����j
        DirectX::XMFLOAT3 start = { position.x, position.y + 1.0f, position.z };
        // ���C�̏I�_�ʒu��ݒ�i�ړ���̈ʒu�j
        DirectX::XMFLOAT3 end = { position.x, position.y + moveY, position.z };

        // ���C�L���X�g�ɂ��n�ʔ���
        HitResult hit;
        if (StageManager::Instance().GetStage(0)->RayCast(start, end, hit))
        {
            // �n�ʂ̌������p������p�@���x�N�g���ɐݒ�
            normal = hit.normal;

            // �n�ʂɐݒu���Ă���i�q�b�g���ʂ� y �ʒu�𔽉f�j
            position.y = hit.position.y;

            // ���n����
            if (!groundedFlag)
            {
                OnLanding();
            }
            groundedFlag = true;
            velocity.y = 0.0f;

            // �X�Η��̌v�Z
            float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z + 0.001f);
            slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));
        }
        else
        {
            // �󒆂ɕ����Ă���
            position.y += moveY;
            groundedFlag = false;
        }
    }
    else if (moveY > 0.0f)
    {
        // �㏸��
        position.y += moveY;
        groundedFlag = false;
    }

    //// �p������p�@���x�N�g������ x �� z �̊p�x���v�Z
    //// y �����p������p�@���x�N�g�������Ɍ����悤�Ɋp�x���v�Z
    float angleX = atan2f(normal.z, normal.y);
    float angleZ = -atan2f(normal.x, normal.y);

    //// ���`��ԂŊ��炩�ɉ�]
    //quaternion.x = Mathf::Lerp(quaternion.x, angleX, 0.1f);
    //quaternion.z = Mathf::Lerp(quaternion.z, angleZ, 0.1f);
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
            
            // �X�Η��������ꍇ�͗����Ă����悤�Ɍv�Z
            if (groundedFlag && slopeRate > 0.0f)
            {
                velocity.y -= length * slopeRate * elapsedTime * 60.0f;
            }
        }
    }
    direction.x = 0.0f;
    direction.z = 0.0f;
}

// �����ړ��X�V����
void Character::UpdateHorizontalMove(float elapsedTime)
{
    // �������x�ʌv�Z
    float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    if (velocityLengthXZ > 0.0f)
    {
        // �v�Z�p�̈ړ���̑��x
        float moveX = velocity.x * elapsedTime;
        float moveZ = velocity.z * elapsedTime;

        // ���C�̎n�_�ʒu�ƏI�_�ʒu
        DirectX::XMFLOAT3 start = { position.x, position.y + 1.0f, position.z };
        DirectX::XMFLOAT3 end = { position.x + moveX, position.y + 1.0f, position.z + moveZ };

        // ���C�L���X�g
        HitResult hit;
        if (StageManager::Instance().GetStage(0)->RayCast(start, end, hit))
        {
            DirectX::XMVECTOR startVec = DirectX::XMLoadFloat3(&hit.position);
            DirectX::XMVECTOR endVec = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(endVec, startVec);

            // �ǂ̖@���x�N�g��
            DirectX::XMVECTOR normalVec = DirectX::XMLoadFloat3(&hit.normal);

            // ���˃x�N�g�����t�x�N�g���ɕϊ�
            vec = DirectX::XMVectorNegate(vec);

            // ���˃x�N�g����@���Ŏˉe�i�ړ���̈ʒu����ǂ܂ł̋����j
            DirectX::XMVECTOR lengthVec = DirectX::XMVector3Dot(vec, normalVec);
            float length;
            DirectX::XMStoreFloat(&length, lengthVec);

            // �␳�ʒu�ւ̃x�N�g�����v�Z
            DirectX::XMVECTOR correctVec = DirectX::XMVectorScale(normalVec, length * 1.1f);

            // �ŏI�I�ȕ␳�ʒu�x�N�g�����v�Z
            DirectX::XMVECTOR correctPosVec = DirectX::XMVectorAdd(endVec, correctVec);
            DirectX::XMFLOAT3 correctPos;
            DirectX::XMStoreFloat3(&correctPos, correctPosVec);
            //�ǂ�������փ��C�L���X�g
            HitResult hit2;
            if (!StageManager::Instance().GetStage(0)->RayCast(hit.position, correctPos, hit2))
            {
                //�ǂ�������ŕǂɓ�����Ȃ�������␳�ʒu�Ɉړ�
                position.x = correctPos.x;
                position.z = correctPos.z;
            }
            else
            {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }
        }
        else
        {
            // �ǂɂ������Ă��Ȃ��ꍇ�͒ʏ�̈ړ�
            position.x += moveX;
            position.z += moveZ;
        }
    }
}