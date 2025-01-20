#pragma once

#include "Graphics/Model.h"
#include "Character.h"
#include "ArtsManager.h"
#include "Graphics/GeometricPrimitive.h"
#include "StateMachine.h"
#include"StateDerived.h"
//#include "Effect/Effect.h"


//�A�j���[�V����
enum Animation
{
	AnimTpose,
	AnimIdle,
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
	AnimAvoidBack,
	AnimAvoidFront,
	AnimAvoidLeft,
	AnimAvoidRight,
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
	AnimConbo01,
	AnimConbo02,
};
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
	Model* GetModel() { return model.get(); }
	//�X�e�[�g�}�V���擾
	StateMachine* GetStateMachine() { return stateMachine.get(); }
public:
	static Player* Instance()
	{
		static Player ins;
		return &ins;
	}
	enum class State
	{
		Movement,
		WeakAttack,
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
	enum class WeakAttack
	{
		WeakAttack01,
		WeakAttack02,
		WeakAttack03,
		WeakAttack04,
		WeakAttack05,
		WeakAttack06,
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

	//����ړ�
	bool InputMove(float elapsedTime);

	// �W�����v���͏���
	bool InputJump();

	bool InputArts(float elapsedTime);

	bool InputDashTowardsEnemy(float elapsedTime);

	bool InputDash(float elapsedTime);
	bool InputAttack();

	bool artSkillReady = false;
	bool artUltSkillReady = false;
	void InputFlying(float elapsedTime);
private:
	DirectX::XMFLOAT3 GetMoveVec() const;

	void Lock();

	// �v���C���[�ƓG�Ƃ̏Փˏ���
	void CollisionPlayerAndEnemies();

	// �e�ƓG�̏Փˏ���
	void CollisionArtsAndEnemies();
	void CollisionPlayerAndArts();
	void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

	void TeleportBehindEnemy();

	void UpdateAnimation(float elapsedTime);
protected:
	// ���n�����Ƃ��ɌĂяo�����
	void OnLanding() override;
private:
	std::unique_ptr<StateMachine> stateMachine;
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
	int jumpLimit = 2;	// �W�����v�����i�ő�W�����v���A�ЂƂ܂��Q�i�W�����v�j
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


	float totalAnimationTime = 0;

	int hit = 0;
};