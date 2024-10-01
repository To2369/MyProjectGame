#pragma once

#include "Graphics/Model.h"
#include "Character.h"
#include "BulletManager.h"
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

	//����ړ�
	void InputMove(float elapsedTime);

	// �W�����v���͏���
	void InputJump();

	// ���͂ɂ��e���ˏ���
	void InputLaunchBullet();

	// �v���C���[�ƓG�Ƃ̏Փˏ���
	void CollisionPlayerAndEnemies();

	// �e�ƓG�̏Փˏ���
	void CollisionBulletsAndEnemies();

	void InputTeleportBehindEnemy();

	void InputDashTowardsEnemy(float elapsedTime);
private:
	//�ҋ@�X�e�[�g�֑J��
	void TransitionIdleState();

	//�ҋ@�X�e�[�g�X�V����
	void UpdateIdleState(float elapsedTime);
protected:
	// ���n�����Ƃ��ɌĂяo�����
	void OnLanding() override;
private:
	std::unique_ptr<Model> model;
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

	bool dash = false;
	//std::unique_ptr<BulletManager> bulletMgr;
	BulletManager bulletMgr;
	float elapsedTime_ = 0;
	//�A�j���[�V����
	enum Animation
	{
		Anim_Attack,
		Anim_Death,
		Anim_Falling,
		Anim_GetHit1,
		Anim_GetHit2,
		Anim_Idle,
		Anim_jump,
		Anim_jump_Flip,
		Anim_Landing,
		Anim_Revive,
		Anim_Running,
		Anim_Walking
	};
	enum class State
	{
		Idle,
		Move,
		Jump,
		Land,
		Attack,
		Damage,
		Death,
		Revive,
	};
	State state = State::Idle;
	// �q�b�g�G�t�F�N�g
	//std::unique_ptr<Effect> hitEffect;
};