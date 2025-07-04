#pragma once
#include "Enemy.h"
#include "Player.h"

class EnemyHuman :public Enemy
{
public:
	EnemyHuman();
	~EnemyHuman() {};

	void Update(float elapsedTime) override;

	void Render(ID3D11DeviceContext* dc) override;

	void DrawDebugGUI() override;

	bool SearchPlayer();
	void SetRandomTargetPosition();
	//�i���o���ݒ�
	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);
	// ���S�����Ƃ��ɌĂ΂��
	void OnDead() override;

private:
	
	Player* lockonPlayer = nullptr;
	float moveSpeed = 3.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	DirectX::XMFLOAT3 ppos;
	DirectX::XMFLOAT3 targetPosition = { 0,0,0 };
	DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
	float territoryRange = 10.0f;
	float reset = 3;
	float dist = 0.0f;
	float searchRange = 5.0f;
	float stateTimer = 0.0f;

	//�A�j���[�V����
	enum AnimationNum
	{
		AnimTpose,
		AnimIdleNormal,
		AnimConbatIdle,
		AnimConbo01_1,
		AnimConbo01_2,
		AnimConbo01_3,
		AnimConbo01_4,
		AnimConbo01_5,
		AnimConbo01_6,
		AnimConbo02_1,
		AnimConbo02_2,
		AnimConbo02_3,
		AnimAttack01,
		AnimAttack02,
		AnimAttack03,
		AnimAttack04,
		AnimAttack05,
		AnimAttack06,
		AnimAttack07,
		AnimAttack08,
		AnimDefense,
		AnimDead,
		AnimDownEnd,
		AnimDownStart,
		AnimHitBack,
		AnimHitFront,
		AnimHitLeft,
		AnimHitRight,
		AnimJumpFalling,
		AnimJumpLanding,
		AnimJumpStart,
		AnimRun,
		AnimWalk,
		AnimRise,
		AnimUnequip,
	};
private:

	//�ڕW�n�_�ֈړ�
	void MoveToTarget(float elapsedTime, float speedRate);


	//�m�[�h�ƃv���C���[�̏Փˏ���
	void CollisionNodeVsPlayer(const char* nodeName, float boneRadius);

	//�o�~�X�e�[�g�֑J��
	void TransitionWanderState();

	//�o�~�X�e�[�g�X�V����
	void UpdateWanderState(float elapsedTime);

	//�ҋ@�X�e�[�g�֑J��
	void TransitionIdleState();

	//�ҋ@�X�e�[�g�X�V����
	void UpdateIdleState(float elapsedTime);

	//�ǐՃX�e�[�g�֑J��
	void TransitionPursuitState();

	//�ǐՃX�e�[�g�X�V����
	void UpdatePursuitState(float elapsedTime);

	//�U���X�e�[�g�֑J��
	void TransitionAttackState();

	//�U���X�e�[�g�X�V����
	void UpdateAttackState(float elapsedTime);

	//�퓬�ҋ@�X�e�[�g�֑J��
	void TransitionIdleBattleState();

	//�퓬�ҋ@�X�e�[�g�X�V����
	void UpdateIdleBattleState(float elapsedTime);

	//�_���[�W�X�e�[�g�֑J��
	void TransitionDamageState();

	//�_���[�W�X�e�[�g�X�V����
	void UpdateDamageState(float elapsedTime);

	//���S�X�e�[�g�֑J��
	void TransitionDeathState();

	//���S�X�e�[�g�X�V����
	void UpdateDeathState(float elapsedTime);
	//�X�e�[�g
	enum class State
	{
		Wander,
		Idle,
		Pursuit,
		Attack,
		IdleBattle,
		Damage,
		Death,
	};
	State state = State::Wander;
	float attackRange = 1.5f;
};