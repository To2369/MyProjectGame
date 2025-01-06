#pragma once
#include "Player.h"

/*���C���X�e�[�g*/
class MovementState :public HierarchicalState
{
public:
	//�R���X�g���N�^
	MovementState(Player* enemy) :HierarchicalState(enemy) {}
	~MovementState();
	//�X�e�[�g�ɓ��������̃��\�b�h
	void Enter();
	//�X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime);
	//�X�e�[�g����łĂ����Ƃ��̃��\�b�h
	void Exit();
};

class BattleState :public HierarchicalState
{
public:
	//�R���X�g���N�^
	BattleState(Player* enemy) :HierarchicalState(enemy) {}
	~BattleState();
	//�X�e�[�g�ɓ��������̃��\�b�h
	void Enter();
	//�X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime);
	//�X�e�[�g����łĂ����Ƃ��̃��\�b�h
	void Exit();
};
//���^AI���烁�b�Z�[�W��M����M�����Ƃ��ɌĂ΂��
class HitDamegeState :public HierarchicalState
{
public:
	//�R���X�g���N�^
	HitDamegeState(Player* enemy) :HierarchicalState(enemy) {}
	~HitDamegeState();
	//�X�e�[�g�ɓ��������̃��\�b�h
	void Enter();
	//�X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime);
	//�X�e�[�g����łĂ����Ƃ��̃��\�b�h
	void Exit();
};
/***********************************************************************************************/

// �ҋ@�X�e�[�g�I�u�W�F�N�g
class IdleState : public State
{
public:
	// �R���X�g���N�^
	IdleState(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~IdleState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};

// �ǐՃX�e�[�g�I�u�W�F�N�g
class MoveState : public State
{
public:
	// �R���X�g���N�^
	MoveState(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~MoveState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};

// �ǐՃX�e�[�g�I�u�W�F�N�g
class DashState : public State
{
public:
	// �R���X�g���N�^
	DashState(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~DashState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};

// �ǐՃX�e�[�g�I�u�W�F�N�g
class JumpState: public State
{
public:
	// �R���X�g���N�^
	JumpState(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~JumpState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};

// �ǐՃX�e�[�g�I�u�W�F�N�g
class LandState : public State
{
public:
	// �R���X�g���N�^
	LandState(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~LandState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};

// �U���X�e�[�g�I�u�W�F�N�g
class AttackState : public State
{
public:
	// �R���X�g���N�^
	AttackState(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~AttackState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};

//���̃G�l�~�[����Ă΂ꂽ�Ƃ�
class RecoverySkillEnergyState : public State
{
public:
	// �R���X�g���N�^
	RecoverySkillEnergyState(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~RecoverySkillEnergyState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};
//�퓬�ҋ@
class DashToEnemyState : public State
{
public:
	// �R���X�g���N�^
	DashToEnemyState(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~DashToEnemyState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};
//�퓬�ҋ@
class DamegeState : public State
{
public:
	// �R���X�g���N�^
	DamegeState(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~DamegeState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};

class DeathState : public State
{
public:
	// �R���X�g���N�^
	DeathState(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~DeathState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};
