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

class WeakAttackState :public HierarchicalState
{
public:
	//�R���X�g���N�^
	WeakAttackState(Player* enemy) :HierarchicalState(enemy) {}
	~WeakAttackState();
	//�X�e�[�g�ɓ��������̃��\�b�h
	void Enter();
	//�X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime);
	//�X�e�[�g����łĂ����Ƃ��̃��\�b�h
	void Exit();
};

class UseSkillState :public HierarchicalState
{
public:
	//�R���X�g���N�^
	UseSkillState(Player* enemy) :HierarchicalState(enemy) {}
	~UseSkillState();
	//�X�e�[�g�ɓ��������̃��\�b�h
	void Enter();
	//�X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime);
	//�X�e�[�g����łĂ����Ƃ��̃��\�b�h
	void Exit();
};

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
class WeakAttackState01 : public State
{
public:
	// �R���X�g���N�^
	WeakAttackState01(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~WeakAttackState01() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};

// �U���X�e�[�g�I�u�W�F�N�g
class WeakAttackState02 : public State
{
public:
	// �R���X�g���N�^
	WeakAttackState02(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~WeakAttackState02() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};
// �U���X�e�[�g�I�u�W�F�N�g
class WeakAttackState03 : public State
{
public:
	// �R���X�g���N�^
	WeakAttackState03(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~WeakAttackState03() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};
// �U���X�e�[�g�I�u�W�F�N�g
class WeakAttackState04 : public State
{
public:
	// �R���X�g���N�^
	WeakAttackState04(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~WeakAttackState04() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};
// �U���X�e�[�g�I�u�W�F�N�g
class WeakAttackState05 : public State
{
public:
	// �R���X�g���N�^
	WeakAttackState05(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~WeakAttackState05() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	void Enter()override;
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	void Execute(float elapsedTime)override;
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	void Exit()override;
};
// �U���X�e�[�g�I�u�W�F�N�g
class WeakAttackState06 : public State
{
public:
	// �R���X�g���N�^
	WeakAttackState06(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~WeakAttackState06() {}
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

class SkillSelectState :public State
{
public:
	// �R���X�g���N�^
	SkillSelectState(Player* enemy) :State(enemy) {};
	// �f�X�g���N�^
	~SkillSelectState() {}
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
