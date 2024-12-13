#pragma once
#include"StateBase.h"
//template<typename TypeCharacter>
//class SearchState : public HierarchicalState<TypeCharacter>
//{
//public:
//	// コンストラクタ
//	SearchState(TypeCharacter* character) :HierarchicalState<TypeCharacter>(character) {}
//	~SearchState()
//	{
//		for (State<TypeCharacter>* state : this->subStatePool)
//		{
//			delete state;
//		}
//		this->subStatePool.clear();
//	};
//	// ステートに入った時のメソッド
//	virtual void Enter()override {this->owner->MainEnterSearch();};
//	// ステートで実行するメソッド
//	virtual void Execute(float elapsedTime)override { this->owner->MainExecutionSearch(elapsedTime); };
//	// ステートから出ていくときのメソッド
//	virtual void Exit()override {};
//};

template<typename TypeCharacter>
class MovementState :public HierarchicalState<TypeCharacter>
{
public:
	MovementState(TypeCharacter* character) :HierarchicalState<TypeCharacter>(character) {}
	~MovementState() {};
	virtual void Enter() override {};
	virtual void Execute(float elapsedTime) override {};
	virtual void Exit()override {};
};

template<typename TypeCharacter>
class BattleState :public HierarchicalState<TypeCharacter>
{
public:
	BattleState(TypeCharacter*character):HierarchicalState<TypeCharacter>(character){}
	~BattleState() {};
	virtual void Enter() override {};
	virtual void Execute(float elapsedTime) override {};
	virtual void Exit()override {};
};

template<typename TypeCharacter>
class HitDamegeState :public HierarchicalState<TypeCharacter>
{
public:
	HitDamegeState(TypeCharacter* character) :HierarchicalState<TypeCharacter>(character) {}
	~HitDamegeState() {};
	virtual void Enter() override {};
	virtual void Execute(float elapsedTime) override {};
	virtual void Exit()override {};
};

template<typename TypeCharacter>
class IdleState : public State<TypeCharacter>
{
public:
	IdleState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	~IdleState() {}
	virtual void Enter()override {};
	virtual void Execute(float elapsedTime)override {};
	virtual void Exit()override {};
};

template<typename TypeCharacter>
class MoveState : public State<TypeCharacter>
{
public:
	MoveState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	~MoveState() {}
	virtual void Enter()override {};
	virtual void Execute(float elapsedTime)override {};
	virtual void Exit()override {};
};

template<typename TypeCharacter>
class DashState : public State<TypeCharacter>
{
public:
	DashState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	~DashState() {}
	virtual void Enter()override {};
	virtual void Execute(float elapsedTime)override {};
	virtual void Exit()override {};
};

template<typename TypeCharacter>
class JumpState : public State<TypeCharacter>
{
public:
	JumpState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	~JumpState() {}
	virtual void Enter()override {};
	virtual void Execute(float elapsedTime)override {};
	virtual void Exit()override {};
};

template<typename TypeCharacter>
class LandState : public State<TypeCharacter>
{
public:
	LandState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	~LandState() {}
	virtual void Enter()override {};
	virtual void Execute(float elapsedTime)override {};
	virtual void Exit()override {};
};

template<typename TypeCharacter>
class AttackState : public State<TypeCharacter>
{
public:
	AttackState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	~AttackState() {}
	virtual void Enter()override {};
	virtual void Execute(float elapsedTime)override {};
	virtual void Exit()override {};
};

template<typename TypeCharacter>
class RecoverySkillEnergyState : public State<TypeCharacter>
{
public:
	RecoverySkillEnergyState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	~RecoverySkillEnergyState() {}
	virtual void Enter()override {};
	virtual void Execute(float elapsedTime)override {};
	virtual void Exit()override {};
};

template<typename TypeCharacter>
class DashToEnemyState : public State<TypeCharacter>
{
public:
	DashToEnemyState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	~DashToEnemyState() {}
	virtual void Enter()override {};
	virtual void Execute(float elapsedTime)override {};
	virtual void Exit()override {};
};

template<typename TypeCharacter>
class DamegeState : public State<TypeCharacter>
{
public:
	DamegeState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	~DamegeState() {}
	virtual void Enter()override {};
	virtual void Execute(float elapsedTime)override {};
	virtual void Exit()override {};
};