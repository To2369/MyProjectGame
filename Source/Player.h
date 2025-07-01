#pragma once

#include "Character.h"
#include "ArtsManager.h"
#include "Graphics/GeometricPrimitive.h"
#include "StateMachine.h"
#include "Enemy.h"
#include "AttackData.h"
#include "Effect/Effect.h"
#include <string>



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

	//�X�e�[�g�}�V���擾
	StateMachine<Player>* GetStateMachine() { return stateMachine.get(); }
public:
	static Player* Instance()
	{
		static Player ins;
		return &ins;
	}
	enum class State
	{
		Movement,
		Attack,
		UseSkill,
		HitDamege,
	};
	enum class Movement
	{
		Idle,
		Move,
		Dash,
		Jump,
		Land,
	};
	enum class Attack
	{
		WeakAttack01_1,
		WeakAttack01_2,
		WeakAttack01_3,
		WeakAttack01_4,
		WeakAttack01_5,
		WeakAttack01_6,
		WeakAttack01_7,
		WeakAttack01_8,
		WeakAttack02_1,
		WeakAttack02_2,
		StrongAttack01,
		StrongAttack02,
		StrongAttack03,
		StrongAttack04,
		StrongAttack05,
		StrongAttack06,
		StrongAttack07,
		StrongAttack08,
	};

	enum class UseSkill
	{
		SkillSelect,
	};
	enum class HitDamege
	{
		Damege,
		Death,
	};

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
	//����ړ�
	bool InputMove(float elapsedTime);

	// �W�����v���͏���
	bool InputJump();

	bool InputArts(float elapsedTime);

	bool InputDashTowardsEnemy(float elapsedTime);

	bool InputDash(float elapsedTime);
	bool InputWeekAttack();
	bool InputStrongAttack();
	bool artSkillReady = false;
	bool artUltSkillReady = false;
	void InputFlying(float elapsedTime);
	DirectX::XMFLOAT3 GetMoveVec(const DirectX::XMFLOAT3& cameraRight, const DirectX::XMFLOAT3& cameraFront) const;

	void Lockon();

	// �v���C���[�ƓG�Ƃ̏Փˏ���
	void CollisionPlayerAndEnemies();

	// �e�ƓG�̏Փˏ���
	void CollisionArtsAndEnemies();
	void CollisionNodeVsEnemies(const std::string nodeName, float nodeRadius, float invicibleTimer, int damage);

	void InputAttackNext(float currentAnimSeconds, AttackData attackData);
	void ActiveAttackCollider(AttackData attackData);
	void AttackMove(float startMoveTimer, float endMoveTime, float speed);
	void TeleportBehindEnemy();
protected:
	// ���n�����Ƃ��ɌĂяo�����
	void OnLanding() override;
private:
	std::unique_ptr<StateMachine<Player>> stateMachine;

	enum class LockonState
	{
		NotLocked,
		Locked,
	};
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
	ArtsManager artsMgr;
	float elapsedTime_ = 0;

	enum class Skill
	{
		SpiritBlast,
		SkillMax,
	};
	//State state = State::Idle;
	Skill skill = Skill::SkillMax;
	bool dashTowardEnemyFlag = false;
	// �q�b�g�G�t�F�N�g
	float PIDIV180 = 0.017452f;

	int hit = 0;

	bool lockon = false;

	LockonState lockonState = LockonState::NotLocked;
	Enemy* lockonEnemy = nullptr;
	std::unique_ptr<Effect> hitEffect;
	float flyTimer = 1;
};