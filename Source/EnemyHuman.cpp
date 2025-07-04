#include "EnemyHuman.h"
#include "Graphics/Graphics.h"
#include "Mathf.h"
#include "Player.h"
EnemyHuman::EnemyHuman()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".\\Data\\Model\\Enemy\\EnemyBoxer.fbx");

	const float scaleFactor = 0.01f;
	scale = { scaleFactor,scaleFactor,scaleFactor };

	radius = 0.5f;  // ��
	height = 1.0f;  // ����

    maxHealth = 1000;
    health = maxHealth;
    position.y = 5.0f;

    //�p�j�X�e�[�g�֑J��
    TransitionWanderState();
}

void EnemyHuman::Update(float elapsedTime)
{
    //�X�e�[�g���X�V����
    switch (state)
    {
    case State::Wander:
        UpdateWanderState(elapsedTime);
        break;
    case State::Idle:
        UpdateIdleState(elapsedTime);
        break;
    case State::Pursuit:
        UpdatePursuitState(elapsedTime);
        break;
    case State::Attack:
        UpdateAttackState(elapsedTime);
        break;
    case State::IdleBattle:
        UpdateIdleBattleState(elapsedTime);
        break;
    case State::Damage:
        UpdateDamageState(elapsedTime);
        break;
    case State::Death:
        UpdateDeathState(elapsedTime);
        break;
    }
	// ���x�����X�V
	UpdateVelocity(elapsedTime);

	// ���G���Ԃ̍X�V
	UpdateInvincibleTimer(elapsedTime);
    //Lockon();
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        //Test(elapsedTime);
    }
    
    UpdateAnimation(elapsedTime);
    UpdateStatus(elapsedTime);
	UpdateTransform();
}

void EnemyHuman::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, transform, color);
}

void EnemyHuman::DrawDebugGUI()
{
#ifdef USE_IMGUI
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Slime", nullptr, ImGuiWindowFlags_None))
    {
        //�g�����X�t�H�[��
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            //�ʒu
            ImGui::InputFloat3("Position", &position.x);
            //�X�P�[��
            ImGui::InputFloat3("Scale", &scale.x);
            ImGui::InputInt("helth", &health);
            ImGui::InputInt("spirit", &spiritEnergy);
            ImGui::InputInt("skill", &skillEnergy);
            //ImGui::InputFloat("b", &b);
        }
    }
    ImGui::End();
#endif

}

// ���S�����Ƃ��ɌĂ΂��
void EnemyHuman::OnDead()
{
	Destroy();
}

//�^�[�Q�b�g�ʒu�������_���ݒ�
void EnemyHuman::SetRandomTargetPosition()
{
    float theta = Mathf::RandomRange(-DirectX::XM_PI, DirectX::XM_PI);
    float range = Mathf::RandomRange(0.0f, territoryRange);
    targetPosition.x = territoryOrigin.x + sinf(theta) + range;
    targetPosition.y = territoryOrigin.y;
    targetPosition.z = territoryOrigin.z + cosf(theta) * range;
}

//�v���C���[���G
bool EnemyHuman::SearchPlayer()
{
    //�v���C���[�Ƃ̍��፷���l������3D�ł̋������������
    const DirectX::XMFLOAT3& playerPosition = Player::Instance()->GetPosition();
    float vx = playerPosition.x - position.x;
    float vy = playerPosition.y - position.y;
    float vz = playerPosition.z - position.z;
    dist = sqrtf(vx * vx + vy * vy + vz * vz);

    if (dist < searchRange)
    {
        float distXZ = sqrtf(vx * vx + vz * vz);
        //�P�ʃx�N�g����
        vx /= distXZ;
        vz /= distXZ;
        //�O���x�N�g��
        DirectX::XMFLOAT3 f;
        DirectX::XMStoreFloat3(&f, front);
        float frontX = sinf(f.y);
        float frontZ = cosf(f.y);
        //2�̃x�N�g���̓��ϒl�őO�㔻��
        float dot = (frontX * vx) + (frontZ * vz);
        if (dot > 0.0f)
        {
            return true;
        }
    }
    return false;
}

//�꒣��ݒ�
void EnemyHuman::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
    territoryOrigin = origin;
    territoryRange = range;
}

//�ڕW�n�_�ֈړ�
void EnemyHuman::MoveToTarget(float elapsedTime, float speedRate)
{
    //�^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
    float vx = targetPosition.x - position.x;
    float vz = targetPosition.z - position.z;
    float dist = sqrtf(vx * vx + vz * vz);
    vx = vx / dist;
    vz = vz / dist;

    //�ړ�����
    Move(vx, vz, moveSpeed * speedRate);
    Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}


//�p�j�X�e�[�g�ւ̑J��
void EnemyHuman::TransitionWanderState()
{
    state = State::Wander;

    //�ڕW�n�_�ݒ�
    SetRandomTargetPosition();

    //�����A�j���[�V�����Đ�
    //model->PlayAnimation(Anim_WalkFWD, true);
}

//�p�j�X�e�[�g�X�V����
void EnemyHuman::UpdateWanderState(float elapsedTime)
{
    //�ڕW�n�_�܂�XZ���ʂł̋�������
    float vx = targetPosition.x - position.x;
    float vz = targetPosition.z - position.z;
    float distSq = vx * vx + vz * vz;
    if (distSq < radius * radius)
    {
        //�ҋ@�X�e�[�g�֑J��
        TransitionIdleState();
    }

    //�ڕW�n�_�ֈړ�
    MoveToTarget(elapsedTime, 0.5f);

    //�v���C���[���G
    if (SearchPlayer())
    {
        //����������ǐՃX�e�[�g�֑J��
        TransitionPursuitState();
    }
}

//�ҋ@�X�e�[�g�֑J��
void EnemyHuman::TransitionIdleState()
{
    state = State::Idle;

    //�^�C�}�[�������_���ݒ�
    stateTimer = Mathf::RandomRange(3.0f, 5.0f);

    //�ҋ@�A�j���[�V�����Đ�
    //model->PlayAnimation(Anim_IdleNormal, true);
}

//�ҋ@�X�e�[�g�X�V����
void EnemyHuman::UpdateIdleState(float elapsedTime)
{
    //�^�C�}�[����
    stateTimer -= elapsedTime;
    //�v���C���[���G
    if (SearchPlayer())
    {
        //����������ǐՃX�e�[�g�֑J��
        TransitionPursuitState();
        return;
    }
    if (stateTimer < 0.0f)
    {
        //�p�j�X�e�[�g�֑J��
        TransitionWanderState();
    }
}

//�ǐՃX�e�[�g�֑J��
void EnemyHuman::TransitionPursuitState()
{
    state = State::Pursuit;

    //���b�ԒǐՂ���^�C�}�[�������_���ݒ�
    stateTimer = Mathf::RandomRange(3.0f, 5.0f);

    //�����A�j���[�V�����Đ�
    //model->PlayAnimation(Anim_RunFWD, true);
}

//�ǐՃX�e�[�g�X�V����
void EnemyHuman::UpdatePursuitState(float elapsedTime)
{
    //�ڕW�n�_���v���C���[�ʒu�ɐݒ�
    targetPosition = Player::Instance()->GetPosition();

    //�ڕW�n�_�ֈړ�
    MoveToTarget(elapsedTime, 1.0f);

    //�^�C�}�[����
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        //�ҋ@�X�e�[�g�֑J��
        TransitionIdleState();
    }

    //�v���C���[�̋߂����ƍU���X�e�[�g�֑J��
    float vx = targetPosition.x - position.x;
    float vy = targetPosition.y - position.y;
    float vz = targetPosition.z - position.z;
    float dist = sqrtf(vx * vx + vy * vy + vz * vz);
    if (dist < attackRange)
    {
        //�U���X�e�[�g�֑J��
        TransitionAttackState();
    }
}

//�U���X�e�[�g�֑J��
void EnemyHuman::TransitionAttackState()
{
    state = State::Attack;

    //�U���A�j���[�V�����Đ�
    //model->PlayAnimation(Anim_Attack1, false);
}

//�U���X�e�[�g�X�V����
void EnemyHuman::UpdateAttackState(float elapsedTime)
{
    //�C�ӂ̃A�j���[�V�����Đ���Ԃł̂ݏՓ˔��菈��������
    //float animationTime = model->GetCurrentAnimationSeconds();
    //if (animationTime >= 0.1f && animationTime <= 0.35f)
    //{
    //    //�ڋʃm�[�h�ƃv���C���[�̏Փˏ���
    //    CollisionNodeVsPlayer("EyeBall", 0.3f);
    //}

    //�U���A�j���[�V�������I�������퓬�ҋ@�X�e�[�g�֑J��
    if (!model->IsPlayAnimation())
    {
        TransitionIdleBattleState();
    }
    MoveToTarget(elapsedTime, 0.0f);
}

//�퓬�ҋ@�X�e�[�g�֑J��
void EnemyHuman::TransitionIdleBattleState()
{
    state = State::IdleBattle;

    //���b�ԑҋ@����^�C�}�[�������_���ݒ�
    stateTimer = Mathf::RandomRange(2.0f, 3.0f);

    //�퓬�ҋ@�A�j���[�V�����Đ�
    //model->PlayAnimation(Anim_IdleBattle, true);
}

//�퓬�ҋ@�X�e�[�g�X�V����
void EnemyHuman::UpdateIdleBattleState(float elapsedTime)
{
    //�ڕW�n�_���v���C���[�ʒu�ɐݒ�
    targetPosition = Player::Instance()->GetPosition();

    //�^�C�}�[����
    stateTimer -= elapsedTime;
    if (stateTimer < 0.0f)
    {
        //�v���C���[���U���͈͂ɂ����ꍇ�͍U���X�e�[�g�֑J��
        float vx = targetPosition.x - position.x;
        float vy = targetPosition.y - position.y;
        float vz = targetPosition.z - position.z;
        float dist = sqrtf(vx * vx + vy * vy + vz * vz);
        if (dist < attackRange)
        {
            //�U���X�e�[�g�֑J��
            TransitionAttackState();
        }
        else
        {
            //�p�j�X�e�[�g�֑J��
            TransitionWanderState();
        }
    }
    MoveToTarget(elapsedTime, 0.0f);
}

//�_���[�W�X�e�[�g�֑J��
void EnemyHuman::TransitionDamageState()
{
    state = State::Damage;

    //�_���[�W�A�j���[�V�����Đ�
    //model->PlayAnimation(Anim_GetHit, false);
}

//�_���[�W�X�e�[�g�X�V����
void EnemyHuman::UpdateDamageState(float elapsedTime)
{
    //�_���[�W�A�j���[�V�������I�������퓬�ҋ@�X�e�[�g�֑J��
    if (!model->IsPlayAnimation())
    {
        TransitionIdleBattleState();
    }
}

//���S�X�e�[�g�֑J��
void EnemyHuman::TransitionDeathState()
{
    state = State::Death;

    //�_���[�W�A�j���[�V�����Đ�
    //model->PlayAnimation(Anim_Die, false);
}

//���S�X�e�[�g�X�V����
void EnemyHuman::UpdateDeathState(float elapsedTime)
{
    //�_���[�W�A�j���[�V�������I������玩����j��
    if (!model->IsPlayAnimation())
    {
        Destroy();
    }
}