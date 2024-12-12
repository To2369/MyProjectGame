#pragma once
#include"StateBase.h"
template<typename TypeCharacter>
class SearchState : public HierarchicalState<TypeCharacter>
{
public:
	// �R���X�g���N�^
	SearchState(TypeCharacter* chara) :HierarchicalState<TypeCharacter>(chara) {}
	~SearchState()
	{
		for (State<TypeCharacter>* state : this->subStatePool)
		{
			delete state;
		}
		this->subStatePool.clear();
	};
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {this->owner->MainEnterSearch();};
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override { this->owner->MainExecutionSearch(elapsedTime); };
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {};
};
template<typename TypeCharacter>
class BattleState : public HierarchicalState<TypeCharacter>
{
public:
	// �R���X�g���N�^
	BattleState(TypeCharacter* chara) :HierarchicalState<TypeCharacter>(chara) {}
	~BattleState() {
		for (State<TypeCharacter>* state : this->subStatePool)
		{
			delete state;
		}
		this->subStatePool.clear();
	};
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override { this->owner->MainEnterBattle(); };
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override {this->owner->MainExecutionBattle(elapsedTime);};
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {};
};

template<typename TypeCharacter>
class RecieveState : public HierarchicalState<TypeCharacter>
{
public:
	// �R���X�g���N�^
	RecieveState(TypeCharacter*chara) :HierarchicalState<TypeCharacter>(chara) {}
	~RecieveState()
	{
		for (auto& state : this->subStatePool)
		{
			delete state;
		}
		this->subStatePool.clear();
	};
	// �S�Čp����Ŏ���������K�v�����邽�ߏ������z�֐��Ŏ���
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override { this->owner->MainEnterRecieve(); };
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override { this->owner->MainExecutionRecieve(elapsedTime); };
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {};
};
// �p�j�X�e�[�g�I�u�W�F�N�g
template<typename TypeCharacter>
class WanderState : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	WanderState(TypeCharacter* chara) :State<TypeCharacter>(chara) {};
	// �f�X�g���N�^
	~WanderState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override { this->owner->EnterWander(); }
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionWander(elapsedTime); };
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {};
};
// �ҋ@�X�e�[�g�I�u�W�F�N�g
template<typename TypeCharacter>
class IdleState : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	IdleState(TypeCharacter*chara) :State<TypeCharacter>(chara) {};
	// �f�X�g���N�^
	~IdleState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override {this->owner->EnterIdle();};
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionIdle(elapsedTime); };
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {};
};
// �ǐՃX�e�[�g�I�u�W�F�N�g
template<typename TypeCharacter>
class PursuitState : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	PursuitState(TypeCharacter*chara) :State<TypeCharacter>(chara) {};
	// �f�X�g���N�^
	~PursuitState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override { this->owner->EnterPursuitState(); };
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionPursuit(elapsedTime); };
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {};
};
// �U���X�e�[�g�I�u�W�F�N�g
template<typename TypeCharacter>
class AttackState : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	AttackState(TypeCharacter*chara) :State<TypeCharacter>(chara) {};
	// �f�X�g���N�^
	~AttackState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override { this->owner->EnterAttack(); }
	
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionAttack(elapsedTime); }
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {this->owner->ExitAttack();};
};
// �_���[�W�X�e�[�g�I�u�W�F�N�g
template<typename TypeCharacter>
class DamegeState : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	DamegeState(TypeCharacter* chara) :State<TypeCharacter>(chara) {};
	// �f�X�g���N�^
	~DamegeState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override { this->owner->EnterDamege(); };
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionDamege(elapsedTime); };
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {};
};
template<typename TypeCharacter>
class CalledState : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	CalledState(TypeCharacter*chara) :State<TypeCharacter>(chara) {};
	// �f�X�g���N�^
	~CalledState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override { this->owner->EnterCalled(); };
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionCalled(elapsedTime); };
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {};
};
// �퓬�ҋ@�X�e�[�g�I�u�W�F�N�g
template<typename TypeCharacter>
class StandbyState : public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	StandbyState(TypeCharacter*chara) :State<TypeCharacter>(chara) {};
	// �f�X�g���N�^
	~StandbyState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override { this->owner->EnterStandbyState(); };
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionStandbyState(elapsedTime); };
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {};
};

// �K�[�h�X�e�[�g(�ǂ������΂�����������Ȃ��̂ō��͂����ɏ����Ă���)�I�u�W�F�N�g
template<typename TypeCharacter>
class SkillState:public State<TypeCharacter>
{
public:
	// �R���X�g���N�^
	SkillState(TypeCharacter* chara) :State<TypeCharacter>(chara) {};
	// �f�X�g���N�^
	~SkillState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override { this->owner->EnterSkill(); };
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionSkill(); };
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {};
};
//�_�b�V������
template<typename TypeCharacter>
class RunState :public State<TypeCharacter>
{
public:
	RunState(TypeCharacter* chara) :State<TypeCharacter>(chara) {}
	~RunState() {}
	// �X�e�[�g�ɓ��������̃��\�b�h
	virtual void Enter()override { this->owner->EnterRun(); };
	// �X�e�[�g�Ŏ��s���郁�\�b�h
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionRun(elapsedTime); };
	// �X�e�[�g����o�Ă����Ƃ��̃��\�b�h
	virtual void Exit()override {};
};


