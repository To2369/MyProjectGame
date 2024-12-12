#pragma once
#include"StateBase.h"
template<typename TypeCharacter>
class SearchState : public HierarchicalState<TypeCharacter>
{
public:
	// コンストラクタ
	SearchState(TypeCharacter* chara) :HierarchicalState<TypeCharacter>(chara) {}
	~SearchState()
	{
		for (State<TypeCharacter>* state : this->subStatePool)
		{
			delete state;
		}
		this->subStatePool.clear();
	};
	// ステートに入った時のメソッド
	virtual void Enter()override {this->owner->MainEnterSearch();};
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override { this->owner->MainExecutionSearch(elapsedTime); };
	// ステートから出ていくときのメソッド
	virtual void Exit()override {};
};
template<typename TypeCharacter>
class BattleState : public HierarchicalState<TypeCharacter>
{
public:
	// コンストラクタ
	BattleState(TypeCharacter* chara) :HierarchicalState<TypeCharacter>(chara) {}
	~BattleState() {
		for (State<TypeCharacter>* state : this->subStatePool)
		{
			delete state;
		}
		this->subStatePool.clear();
	};
	// ステートに入った時のメソッド
	virtual void Enter()override { this->owner->MainEnterBattle(); };
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {this->owner->MainExecutionBattle(elapsedTime);};
	// ステートから出ていくときのメソッド
	virtual void Exit()override {};
};

template<typename TypeCharacter>
class RecieveState : public HierarchicalState<TypeCharacter>
{
public:
	// コンストラクタ
	RecieveState(TypeCharacter*chara) :HierarchicalState<TypeCharacter>(chara) {}
	~RecieveState()
	{
		for (auto& state : this->subStatePool)
		{
			delete state;
		}
		this->subStatePool.clear();
	};
	// 全て継承先で実装させる必要があるため純粋仮想関数で実装
	// ステートに入った時のメソッド
	virtual void Enter()override { this->owner->MainEnterRecieve(); };
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override { this->owner->MainExecutionRecieve(elapsedTime); };
	// ステートから出ていくときのメソッド
	virtual void Exit()override {};
};
// 徘徊ステートオブジェクト
template<typename TypeCharacter>
class WanderState : public State<TypeCharacter>
{
public:
	// コンストラクタ
	WanderState(TypeCharacter* chara) :State<TypeCharacter>(chara) {};
	// デストラクタ
	~WanderState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override { this->owner->EnterWander(); }
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionWander(elapsedTime); };
	// ステートから出ていくときのメソッド
	virtual void Exit()override {};
};
// 待機ステートオブジェクト
template<typename TypeCharacter>
class IdleState : public State<TypeCharacter>
{
public:
	// コンストラクタ
	IdleState(TypeCharacter*chara) :State<TypeCharacter>(chara) {};
	// デストラクタ
	~IdleState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {this->owner->EnterIdle();};
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionIdle(elapsedTime); };
	// ステートから出ていくときのメソッド
	virtual void Exit()override {};
};
// 追跡ステートオブジェクト
template<typename TypeCharacter>
class PursuitState : public State<TypeCharacter>
{
public:
	// コンストラクタ
	PursuitState(TypeCharacter*chara) :State<TypeCharacter>(chara) {};
	// デストラクタ
	~PursuitState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override { this->owner->EnterPursuitState(); };
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionPursuit(elapsedTime); };
	// ステートから出ていくときのメソッド
	virtual void Exit()override {};
};
// 攻撃ステートオブジェクト
template<typename TypeCharacter>
class AttackState : public State<TypeCharacter>
{
public:
	// コンストラクタ
	AttackState(TypeCharacter*chara) :State<TypeCharacter>(chara) {};
	// デストラクタ
	~AttackState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override { this->owner->EnterAttack(); }
	
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionAttack(elapsedTime); }
	// ステートから出ていくときのメソッド
	virtual void Exit()override {this->owner->ExitAttack();};
};
// ダメージステートオブジェクト
template<typename TypeCharacter>
class DamegeState : public State<TypeCharacter>
{
public:
	// コンストラクタ
	DamegeState(TypeCharacter* chara) :State<TypeCharacter>(chara) {};
	// デストラクタ
	~DamegeState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override { this->owner->EnterDamege(); };
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionDamege(elapsedTime); };
	// ステートから出ていくときのメソッド
	virtual void Exit()override {};
};
template<typename TypeCharacter>
class CalledState : public State<TypeCharacter>
{
public:
	// コンストラクタ
	CalledState(TypeCharacter*chara) :State<TypeCharacter>(chara) {};
	// デストラクタ
	~CalledState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override { this->owner->EnterCalled(); };
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionCalled(elapsedTime); };
	// ステートから出ていくときのメソッド
	virtual void Exit()override {};
};
// 戦闘待機ステートオブジェクト
template<typename TypeCharacter>
class StandbyState : public State<TypeCharacter>
{
public:
	// コンストラクタ
	StandbyState(TypeCharacter*chara) :State<TypeCharacter>(chara) {};
	// デストラクタ
	~StandbyState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override { this->owner->EnterStandbyState(); };
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionStandbyState(elapsedTime); };
	// ステートから出ていくときのメソッド
	virtual void Exit()override {};
};

// ガードステート(どう書けばいいか分からないので今はここに書いておく)オブジェクト
template<typename TypeCharacter>
class SkillState:public State<TypeCharacter>
{
public:
	// コンストラクタ
	SkillState(TypeCharacter* chara) :State<TypeCharacter>(chara) {};
	// デストラクタ
	~SkillState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override { this->owner->EnterSkill(); };
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionSkill(); };
	// ステートから出ていくときのメソッド
	virtual void Exit()override {};
};
//ダッシュ処理
template<typename TypeCharacter>
class RunState :public State<TypeCharacter>
{
public:
	RunState(TypeCharacter* chara) :State<TypeCharacter>(chara) {}
	~RunState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override { this->owner->EnterRun(); };
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override { this->owner->ExecutionRun(elapsedTime); };
	// ステートから出ていくときのメソッド
	virtual void Exit()override {};
};


