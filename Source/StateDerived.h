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
class AttackState :public HierarchicalState<TypeCharacter>
{
public:
	//�R���X�g���N�^
	AttackState(TypeCharacter* character) :HierarchicalState<TypeCharacter>(character) {}
	~AttackState()
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
class WeakAtkState01_1 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WeakAtkState01_1(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~WeakAtkState01_1() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState01_2 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WeakAtkState01_2(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~WeakAtkState01_2() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState01_3 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WeakAtkState01_3(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~WeakAtkState01_3() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState01_4 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WeakAtkState01_4(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~WeakAtkState01_4() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState01_5 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WeakAtkState01_5(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~WeakAtkState01_5() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState01_6 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WeakAtkState01_6(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~WeakAtkState01_6() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState01_7 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WeakAtkState01_7(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~WeakAtkState01_7() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState01_8 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WeakAtkState01_8(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~WeakAtkState01_8() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState02_1 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WeakAtkState02_1(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~WeakAtkState02_1() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState02_2 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WeakAtkState02_2(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~WeakAtkState02_2() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class StrongAtkState01 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	StrongAtkState01(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~StrongAtkState01() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class StrongAtkState02 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	StrongAtkState02(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~StrongAtkState02() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class StrongAttackState03 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	StrongAttackState03(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~StrongAttackState03() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class StrongAttackState04 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	StrongAttackState04(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~StrongAttackState04() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class StrongAttackState05 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	StrongAttackState05(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~StrongAttackState05() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class StrongAttackState06 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	StrongAttackState06(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~StrongAttackState06() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class StrongAttackState07 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	StrongAttackState07(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~StrongAttackState07() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {}
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {}
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class StrongAttackState08 : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	StrongAttackState08(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// �f�X�g���N�^
	~StrongAttackState08() {}
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
