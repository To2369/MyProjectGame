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

void Character::UpdateAnimation(float elapsedTime)
{
    //�Đ����łȂ��Ȃ珈�����Ȃ�
    if (!model->IsPlayAnimation())
        return;

    // �u�����h��Ԃɂ����鎞��
    float blendRate = 1.0f;
    if (model->animationBlendTime < model->animationBlendSeconds)
    {
        model->animationBlendTime += elapsedTime;
        if (model->animationBlendTime >= model->animationBlendSeconds)
        {
            model->animationBlendTime = model->animationBlendSeconds;
        }
        blendRate = model->animationBlendTime / model->animationBlendSeconds;
        blendRate *= blendRate;
    }

    // �A�j���[�V�����̔ԍ�
    int clip_index{ model->currentAnimationIndex };

    int frame_index{ 0 }, old_frame_index{ 0 };
    Animation& animation{ model->animationClips.at(clip_index) };
    frame_index = static_cast<int>(model->currentAnimationSeconds * animation.samplingRate);
    old_frame_index = static_cast<int>(model->oldAnimationSeconds * animation.samplingRate);
    if (frame_index > animation.sequence.size() - 1)
    {
        frame_index = animation.sequence.size() - 1;
    }
    if (old_frame_index > animation.sequence.size() - 1)
    {
        old_frame_index = animation.sequence.size() - 1;
    }

    Animation::Keyframe beginPose, oldPose, newPose;
    Animation::Keyframe endPose;
    if (blendRate < 1.0f)
    {
        // �u�����h�Đ�
        const Animation::Keyframe* begin_keyframes[2]{
            &model->keyframe,
            // ����A�j���[�V�����̍ŏ��̃t���[�����Ō�Ƃ��ĕ⊮
            &model->animationClips.at(clip_index).sequence.at(0),
        };
        const Animation::Keyframe* old_keyframes[2]{
            &model->keyframe,
            // ����A�j���[�V�����̍ŏ��̃t���[�����Ō�Ƃ��ĕ⊮
            &model->animationClips.at(clip_index).sequence.at(old_frame_index),
        };
        const Animation::Keyframe* current_keyframes[2]{
            &model->keyframe,
            // ����A�j���[�V�����̍ŏ��̃t���[�����Ō�Ƃ��ĕ⊮
            &model->animationClips.at(clip_index).sequence.at(frame_index),
        };
        const Animation::Keyframe* end_keyframes[2]{
            &model->keyframe,
            // ����A�j���[�V�����̍ŏ��̃t���[�����Ō�Ƃ��ĕ⊮
            &model->animationClips.at(clip_index).sequence.at(animation.sequence.size() - 1),
        };

        // �u�����h���
        model->BlendAnimations(begin_keyframes, blendRate, beginPose);
        model->BlendAnimations(old_keyframes, blendRate, oldPose);
        model->BlendAnimations(current_keyframes, blendRate, model->keyframe);
        model->BlendAnimations(end_keyframes, blendRate, endPose);
    }
    else
    {
        // �ʏ�Đ�
        model->keyframe = animation.sequence.at(frame_index);

        beginPose = animation.sequence.at(0);
        oldPose = animation.sequence.at(old_frame_index);
        newPose = animation.sequence.at(frame_index);
        endPose = animation.sequence.at(animation.sequence.size() - 1);
    }
#if 0
    {
        // �L�[�t���[���ɑ��݂��邷�ׂẴm�[�h���X�V����
        size_t node_count{ model->keyframe.nodes.size() };
        model->keyframe.nodes.resize(node_count);
        for (size_t node_index = 0; node_index < node_count; ++node_index)
        {
            // ���[�J���s���ݒ�
            animation::keyframe::node& node{ model->keyframe.nodes.at(node_index) };
            DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scaling.x,node.scaling.y,node.scaling.z) };
            DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation)) };
            DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node.translation.x,node.translation.y,node.translation.z) };

            // �e�̃O���[�o���s����擾
            int64_t parent_index{ model->scene_view.nodes.at(node_index).parent_index };
            DirectX::XMMATRIX P{ parent_index < 0 ? DirectX::XMMatrixIdentity() : DirectX::XMLoadFloat4x4(&model->keyframe.nodes.at(parent_index).global_transform) };

            // ���[�J���s�� * �e�̃O���[�o���s��
            DirectX::XMStoreFloat4x4(&node.global_transform, S * R * T * P);
        }
    }

#endif

    //���[�g���[�V����
    {
        const Model::Mesh* mesh = model->FindMesh("root");
        const Skeleton::Bone* bone = model->FindNode("root");
        if (bone && !model->keyframe.nodes.empty())
        {
            const int rootMotionIndex = bone->nodeIndex;
            DirectX::XMFLOAT3 localTranslation{};
            if (model->oldAnimationSeconds > model->currentAnimationSeconds)
            {
                //�I�[�ʒu
                DirectX::XMVECTOR EndPos =
                    DirectX::XMLoadFloat3(&endPose.nodes[rootMotionIndex].translation);
                //�O��ʒu
                DirectX::XMVECTOR oldPos =
                    DirectX::XMLoadFloat3(&oldPose.nodes[rootMotionIndex].translation);
                //����ʒu
                DirectX::XMVECTOR newPos =
                    DirectX::XMLoadFloat3(&newPose.nodes[rootMotionIndex].translation);
                //����ʒu
                DirectX::XMVECTOR beginPos =
                    DirectX::XMLoadFloat3(&beginPose.nodes[rootMotionIndex].translation);

                DirectX::XMVECTOR lerpedTranslation = DirectX::XMVectorLerp(oldPos, newPos, blendRate);
                DirectX::XMStoreFloat3(&localTranslation, lerpedTranslation);
            }
            else
            {
                //�O��ʒu
                DirectX::XMVECTOR oldPos = DirectX::XMLoadFloat3(&oldPose.nodes[rootMotionIndex].translation);

                DirectX::XMVECTOR newPos = DirectX::XMLoadFloat3(&newPose.nodes[rootMotionIndex].translation);
                DirectX::XMVECTOR newPosToOldPos = DirectX::XMVectorSubtract(newPos, oldPos);
                DirectX::XMStoreFloat3(&localTranslation, newPosToOldPos);
            }
            //�e�m�[�h�擾
            DirectX::XMMATRIX ParentGlobalTransform = DirectX::XMMatrixIdentity();
            if (bone->parentIndex >= 0)
            {
                //rootmotionNode�̐e�m�[�h(parent)����O���[�o���s����擾�ł���̂�
                //���[�J���ړ��ʂ��O���[�o����ԂɈړ�������
                const Skeleton::Bone& rootmotionNode = mesh->bindPose.bones[bone->parentIndex];
                ParentGlobalTransform = DirectX::XMLoadFloat4x4(&model->keyframe.nodes[rootmotionNode.nodeIndex].globalTransform);
            }

            DirectX::XMVECTOR GlobalTranslation;
            GlobalTranslation = DirectX::XMVector3TransformNormal(XMLoadFloat3(&localTranslation), ParentGlobalTransform);

            {
                GlobalTranslation = DirectX::XMVectorSetY(GlobalTranslation, 0);
                DirectX::XMVECTOR LocalPos, GlobalPos;
                LocalPos = DirectX::XMLoadFloat3(&newPose.nodes[rootMotionIndex].translation);
                GlobalPos = DirectX::XMVector3Transform(LocalPos, ParentGlobalTransform);
                GlobalPos = DirectX::XMVectorSetX(GlobalPos, 0);
                GlobalPos = DirectX::XMVectorSetZ(GlobalPos, 0);
                DirectX::XMMATRIX InverceParentGlobalTransform;
                InverceParentGlobalTransform = XMMatrixInverse(nullptr, ParentGlobalTransform);
                LocalPos = XMVector3Transform(GlobalPos, InverceParentGlobalTransform);
                DirectX::XMStoreFloat3(&model->keyframe.nodes[bone->nodeIndex].translation, LocalPos);

                DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&transform);
                DirectX::XMFLOAT3 worldTranslation;
                DirectX::XMVECTOR ConvertWordtransform;
                ConvertWordtransform = DirectX::XMVector3TransformNormal(GlobalTranslation, WorldTransform);
                DirectX::XMStoreFloat3(&worldTranslation, ConvertWordtransform);

                position.x += worldTranslation.x;
                position.y += worldTranslation.y;
                position.z += worldTranslation.z;
            }
        }
    }
    {
        // �L�[�t���[���ɑ��݂��邷�ׂẴm�[�h���X�V����
        size_t node_count{ model->keyframe.nodes.size() };
        model->keyframe.nodes.resize(node_count);
        for (size_t node_index = 0; node_index < node_count; ++node_index)
        {
            // ���[�J���s���ݒ�
            Animation::Keyframe::Node& node{ model->keyframe.nodes.at(node_index) };
            DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node.scaling.x,node.scaling.y,node.scaling.z) };
            DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotation)) };
            DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node.translation.x,node.translation.y,node.translation.z) };

            // �e�̃O���[�o���s����擾
            int64_t parent_index{ model->sceneView.nodes.at(node_index).parentIndex };
            DirectX::XMMATRIX P{ parent_index < 0 ? DirectX::XMMatrixIdentity() : DirectX::XMLoadFloat4x4(&model->keyframe.nodes.at(parent_index).globalTransform) };

            // ���[�J���s�� * �e�̃O���[�o���s��
            DirectX::XMStoreFloat4x4(&node.globalTransform, S * R * T * P);
        }
    }

    //�ŏI�t���[������
    if (model->animationEndFlag)
    {
        model->animationEndFlag = false;
        model->currentAnimationIndex = -1;
        return;
    }
    //���Ԍo��
    model->oldAnimationSeconds = model->currentAnimationSeconds;
    model->currentAnimationSeconds += elapsedTime;

    // �A�j���[�V�����S�̂̒������v�Z
    model->totalAnimationTime =
        model->animationClips.at(model->currentAnimationIndex).sequence.size() *
        (1.0f / model->animationClips.at(model->currentAnimationIndex).samplingRate);
    //�Đ����Ԃ��I�[���Ԃ𒴂�����
    if (model->currentAnimationSeconds >= model->totalAnimationTime)
    {
        //�Đ����Ԃ������߂�
        if (model->animationLoop)
        {
            model->currentAnimationSeconds -= model->totalAnimationTime;
        }
        else
        {
            model->currentAnimationSeconds = model->totalAnimationTime;
            model->animationEndFlag = true;
        }
    }
}


void Character::UpdateStatus(float elapsedTime)
{
    if (!useSpiritEnergyFlag)
    {
        //�C�͂��񕜂���Ԋu
        energyRecoveryTimer -= elapsedTime;
        if (energyRecoveryTimer <= 0)
        {
            spiritEnergy += spiritHealSpeed;
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
void Character::Fly(float elapsedTime)
{
    if (flyingFlag)
    {
        //velocity.y = 0.0f; // �d�̓��Z�b�g
        groundedFlag = false;
    }
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
    if (!flyingFlag)
    {
        velocity.y += gravity * elapsedTime * 60.0f;
    }
    // �����ł���ꍇ
    else
    {
        // 2�ɔ�s���x������
        velocity.y = flySpeed * elapsedTime * 60.0f;
    }
}

DirectX::XMFLOAT3 convert_quaternion_to_euler(DirectX::XMFLOAT4X4 rotation)
{
    //	ZXY��]
    DirectX::XMFLOAT3 euler;
    if (1.0f - fabs(rotation.m[2][1]) < 1.0e-6f)
    {
        euler.x = rotation.m[2][1] < 0 ? DirectX::XM_PIDIV2 : -DirectX::XM_PIDIV2;
        euler.y = atan2f(rotation.m[1][0], rotation.m[0][0]);
        euler.z = 0;
    }
    else
    {
        euler.x = asinf(-rotation.m[2][1]);
        euler.y = atan2f(rotation.m[2][0], rotation.m[2][2]);
        euler.z = atan2f(rotation.m[0][1], rotation.m[1][1]);
    }
    return euler;
}

DirectX::XMFLOAT3 convert_quaternion_to_euler(DirectX::XMFLOAT4 quaternion)
{
    DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quaternion));
    DirectX::XMFLOAT4X4 rotation;
    DirectX::XMStoreFloat4x4(&rotation, Rotation);
    return convert_quaternion_to_euler(rotation);
}
// �����ړ��X�V����
void Character::UpdateVerticalMove(float elapsedTime)
{
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
        position.y += moveY;
        groundedFlag = false;
    }

    // ���������̈ړ���
    if (flyingFlag)
    {
        // ��s���͎��R�ɏ㉺�ړ�
        position.y += moveY;
    }
#if 01
    DirectX::XMVECTOR Normal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&normal));
    DirectX::XMVECTOR Up = DirectX::XMVectorSet(0, 1, 0, 0);
    //  ��x�N�g���Ɩ@�������]�����N�H�[�^�j�I�����Z�o
    DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Normal, Up);
    float dot = DirectX::XMVectorGetX(Dot);
    DirectX::XMVECTOR NQ = DirectX::XMQuaternionIdentity();
    DirectX::XMVECTOR QuaternionAdditional = DirectX::XMLoadFloat4(&quaternion_additional);
    if (abs(dot) < 0.999f)
    {
        DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Up, Normal);
        NQ = DirectX::XMQuaternionRotationAxis(Axis, acosf(DirectX::XMVectorGetX(Dot)));
    }
    NQ = DirectX::XMQuaternionSlerp(QuaternionAdditional, NQ, 0.1f);
    //  ���̎p���ɍ�����K��
    DirectX::XMVECTOR Quaternion = DirectX::XMLoadFloat4(&quaternion);
    {
        //  �����K���O�ɑO��̉�]�ʂ�ł�����
        QuaternionAdditional = DirectX::XMQuaternionInverse(QuaternionAdditional);
        Quaternion = DirectX::XMQuaternionMultiply(Quaternion, QuaternionAdditional);
        //  ���߂ď�Z
        Quaternion = DirectX::XMQuaternionMultiply(Quaternion, NQ);
    }
    //�L�^
    DirectX::XMStoreFloat4(&quaternion, Quaternion);
    DirectX::XMStoreFloat4(&quaternion_additional, NQ);
#endif
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