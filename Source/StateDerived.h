#pragma once
#include "StateBase.h"

/*���C���X�e�[�g*/
template<typename TypeCharacter>
class MovementState :public HierarchicalState<TypeCharacter>
{
public:
	//�R���X�g���N�^
	MovementState(TypeCharacter* character) :HierarchicalState<TypeCharacter>(character) {}
	~MovementState()
	{
		for (State<TypeCharacter>* state : this->subStatePool)
		{
			delete state;
		}
		this->subStatePool.clear();
	};
	//�X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter() override { }
	//�X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime) override {}
	//�X�e�[�g����łĂ����Ƃ��̃��\�b�h
	virtual void Exit() override {}
};

template<typename TypeCharacter>
class WeakAttackState :public HierarchicalState<TypeCharacter>
{
public:
	//�R���X�g���N�^
	WeakAttackState(TypeCharacter* character) :HierarchicalState<TypeCharacter>(character) {}
	~WeakAttackState()
	{
		for (State<TypeCharacter>* state : this->subStatePool)
		{
			delete state;
		}
		this->subStatePool.clear();
	};
	//�X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter() override {}
	//�X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime) override {}
	//�X�e�[�g����łĂ����Ƃ��̃��\�b�h
	virtual void Exit() override {}
};

template<typename TypeCharacter>
class UseSkillState :public HierarchicalState<TypeCharacter>
{
public:
	//�R���X�g���N�^
	UseSkillState(TypeCharacter* character) :HierarchicalState<TypeCharacter>(character) {}
	~UseSkillState()
	{
		for (State<TypeCharacter>* state : this->subStatePool)
		{
			delete state;
		}
		this->subStatePool.clear();
	};
	//�X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter() override {}
	//�X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime) override {}
	//�X�e�[�g����łĂ����Ƃ��̃��\�b�h
	virtual void Exit() override {}
};

template<typename TypeCharacter>
class HitDamegeState :public HierarchicalState<TypeCharacter>
{
public:
	//�R���X�g���N�^
	HitDamegeState(TypeCharacter* character) :HierarchicalState<TypeCharacter>(character) {}
	~HitDamegeState()
	{
		for (State<TypeCharacter>* state : this->subStatePool)
		{
			delete state;
		}
		this->subStatePool.clear();
	};
	//�X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter() override {}
	//�X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime) override {}
	//�X�e�[�g����łĂ����Ƃ��̃��\�b�h
	virtual void Exit() override {}
};
/***********************************************************************************************/

template<typename TypeCharacter>
class IdleState : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	IdleState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~IdleState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class MoveState : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	MoveState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~MoveState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class DashState : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	DashState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~DashState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class JumpState: public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	JumpState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~JumpState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class LandState : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	LandState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~LandState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAttackState01 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WeakAttackState01(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~WeakAttackState01() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAttackState02 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WeakAttackState02(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~WeakAttackState02() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAttackState03 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WeakAttackState03(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~WeakAttackState03() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAttackState04 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WeakAttackState04(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~WeakAttackState04() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAttackState05 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WeakAttackState05(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~WeakAttackState05() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAttackState06 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WeakAttackState06(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~WeakAttackState06() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class DashToEnemyState : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	DashToEnemyState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~DashToEnemyState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class SkillSelectState :public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	SkillSelectState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~SkillSelectState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};


template<typename TypeCharacter>
class DamegeState : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	DamegeState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~DamegeState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class DeathState : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	DeathState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~DeathState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};
