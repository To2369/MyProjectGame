#pragma once

#include "Graphics/Model.h"
#include "Character.h"
#include "BulletManager.h"
#include "ArtsManager.h"
#include "Graphics/GeometricPrimitive.h"
//#include "Effect/Effect.h"

// �v���C���[
class Player :public Character
{
public:
	Player();
	~Player()override;

	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* dc);

	//�f�o�b�O�pGUI�`��
	void DrawDebugGUI();

	// �f�o�b�O�v���~�e�B�u�\��
	void DrawDebugPrimitive();
public:
	static Player* Instance()
	{
		static Player ins;
		return &ins;
	}
private:
	DirectX::XMFLOAT3 GetMoveVec() const;

	void Lock();
	//����ړ�
	bool InputMove(float elapsedTime);

	// �W�����v���͏���
	void InputJump();

	// ���͂ɂ��e���ˏ���
	void InputLaunchBullet();

	void InputArts();

	// �v���C���[�ƓG�Ƃ̏Փˏ���
	void CollisionPlayerAndEnemies();

	// �e�ƓG�̏Փˏ���
	void CollisionBulletsAndEnemies();

	void CollisionArtsAndEnemies();
	void CollisionPlayerAndArts();
	void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

	void TeleportBehindEnemy();

	bool InputDashTowardsEnemy(float elapsedTime);

	bool InputRecoverySkillEnergy(float elapsedTime);

	bool InputDash(float elapsedTime);
	bool InputAttack();
private:
	//�ҋ@�X�e�[�g
	void TransitionIdleState();

	//�ҋ@�X�e�[�g�X�V����
	void UpdateIdleState(float elapsedTime);

	//�ړ��X�e�[�g
	void TransitionMoveState();

	//�ړ��X�e�[�g�X�V����
	void UpdateMoveState(float elapsedTime);

	//�_�b�V���X�e�[�g
	void TransitionDashState();

	//�_�b�V���X�e�[�g�X�V����
	void UpdateDashState(float elapsedTime);

	//�G�փ_�b�V���X�e�[�g
	void TransitionDashToEnemyState();

	//�G�փ_�b�V���X�e�[�g�X�V����
	void UpdateDashToEnemyState(float elapsedTime);

	// �Z�͉�
	void TransitionRecoverySkillEnergy();
	// �Z�͉񕜍X�V����
	void UpdateRecoverySkillEnergyState(float elapsedTime);

	void TransitionAttackState();
	void UpdateAttackState(float elapsedTime);
protected:
	// ���n�����Ƃ��ɌĂяo�����
	void OnLanding() override;
private:
	std::unique_ptr<Model> model;
	std::unique_ptr<GeometricPrimitive> geo;
	// �ړ��X�s�[�h
	float moveSpeed = 5.0f;
	// ����X�s�[�h
	float turnSpeed = DirectX::XMConvertToRadians(720);

	// �W�����v��
	float jumpSpeed = 20.0f;
	// �d��
	float gravity = -1.0f;
	// ���x
	DirectX::XMFLOAT3 velocity = { 0,0,0 };

	int jumpCount = 0;	// �W�����v��
	int jumpLimit = 1;	// �W�����v�����i�ő�W�����v���A�ЂƂ܂��Q�i�W�����v�j
	//std::unique_ptr<BulletManager> bulletMgr;
	BulletManager bulletMgr;
	ArtsManager artsMgr;
	float elapsedTime_ = 0;
	//�A�j���[�V����
	enum Animation
	{
		AnimIdle,
		AnimConbatIdle,
		AnimConbatToIdle,
		AnimIdleToConbat,
		AnimConbo01_1,
		AnimConbo01_2,
		AnimConbo01_3,
		AnimConbo01_4,
		AnimConbo02_1,
		AnimConbo02_2,
		AnimConbo02_3,
		AnimConbo02_4,
		AnimConbo03_1,
		AnimConbo03_2,
		AnimConbo03_3,
		AnimConbo03_4,
		AnimConbo04_1,
		AnimConbo04_2,
		AnimConbo04_3,
		AnimConbo04_4,
		AnimBuff,
		AnimChargeAttackStart,
		AnimChargeAttackEnd,
		AnimWalk,
		AnimRun,
		AnimJumpStart,
		AnimJumpEnd,
		AnimStep,
		AnimHit,
		AnimDeathHit,
		AnimHitHeavyStart,
		AnimHitHeavyEnd,
		AnimGetUp,
		BlockHit,
	};
	enum class State
	{
		Idle,
		Move,
		Jump,
		Dash,
		DashToEnemy,
		RecoverySkillEnergy,
		Land,
		Attack,
		Damage,
		Death,
		Revive,
	};
	enum class Skill
	{
		SpiritBlast,
		SkillMax,
	};
	State state = State::Idle;
	Skill skill = Skill::SkillMax;
	bool dashTowardEnemyFlag = false;
	// �q�b�g�G�t�F�N�g
	//std::unique_ptr<Effect> hitEffect;
	float PIDIV180 = 0.017452f;


	DirectX::XMFLOAT3 p = {0,0,0};
	DirectX::XMFLOAT3 s = { 1,1,1 };
	DirectX::XMFLOAT4X4 t = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	float L = {};

	int attackCount = 0;
	bool attackFlag = false;
};