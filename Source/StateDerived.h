#pragma once
#include "StateBase.h"

/*メインステート*/
template<typename TypeCharacter>
class MovementState :public HierarchicalState<TypeCharacter>
{
public:
	//コンストラクタ
	MovementState(TypeCharacter* character) :HierarchicalState<TypeCharacter>(character) {}
	~MovementState()
	{
		for (State<TypeCharacter>* state : this->subStatePool)
		{
			delete state;
		}
		this->subStatePool.clear();
	};
	//ステートに入った時のメソッド
	virtual void Enter() override { }
	//ステートで実行するメソッド
	virtual void Execute(float elapsedTime) override {}
	//ステートからでていくときのメソッド
	virtual void Exit() override {}
};

template<typename TypeCharacter>
class AttackState :public HierarchicalState<TypeCharacter>
{
public:
	//コンストラクタ
	AttackState(TypeCharacter* character) :HierarchicalState<TypeCharacter>(character) {}
	~AttackState()
	{
		for (State<TypeCharacter>* state : this->subStatePool)
		{
			delete state;
		}
		this->subStatePool.clear();
	};
	//ステートに入った時のメソッド
	virtual void Enter() override {}
	//ステートで実行するメソッド
	virtual void Execute(float elapsedTime) override {}
	//ステートからでていくときのメソッド
	virtual void Exit() override {}
};

template<typename TypeCharacter>
class UseSkillState :public HierarchicalState<TypeCharacter>
{
public:
	//コンストラクタ
	UseSkillState(TypeCharacter* character) :HierarchicalState<TypeCharacter>(character) {}
	~UseSkillState()
	{
		for (State<TypeCharacter>* state : this->subStatePool)
		{
			delete state;
		}
		this->subStatePool.clear();
	};
	//ステートに入った時のメソッド
	virtual void Enter() override {}
	//ステートで実行するメソッド
	virtual void Execute(float elapsedTime) override {}
	//ステートからでていくときのメソッド
	virtual void Exit() override {}
};

template<typename TypeCharacter>
class HitDamegeState :public HierarchicalState<TypeCharacter>
{
public:
	//コンストラクタ
	HitDamegeState(TypeCharacter* character) :HierarchicalState<TypeCharacter>(character) {}
	~HitDamegeState()
	{
		for (State<TypeCharacter>* state : this->subStatePool)
		{
			delete state;
		}
		this->subStatePool.clear();
	};
	//ステートに入った時のメソッド
	virtual void Enter() override {}
	//ステートで実行するメソッド
	virtual void Execute(float elapsedTime) override {}
	//ステートからでていくときのメソッド
	virtual void Exit() override {}
};
/***********************************************************************************************/

template<typename TypeCharacter>
class IdleState : public State<TypeCharacter>
{
public:
	// コンストラクタ
	IdleState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~IdleState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class MoveState : public State<TypeCharacter>
{
public:
	// コンストラクタ
	MoveState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~MoveState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class DashState : public State<TypeCharacter>
{
public:
	// コンストラクタ
	DashState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~DashState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class JumpState: public State<TypeCharacter>
{
public:
	// コンストラクタ
	JumpState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~JumpState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class LandState : public State<TypeCharacter>
{
public:
	// コンストラクタ
	LandState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~LandState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState01_1 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	WeakAtkState01_1(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~WeakAtkState01_1() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState01_2 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	WeakAtkState01_2(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~WeakAtkState01_2() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState01_3 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	WeakAtkState01_3(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~WeakAtkState01_3() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState01_4 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	WeakAtkState01_4(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~WeakAtkState01_4() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState01_5 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	WeakAtkState01_5(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~WeakAtkState01_5() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState01_6 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	WeakAtkState01_6(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~WeakAtkState01_6() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState01_7 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	WeakAtkState01_7(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~WeakAtkState01_7() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState01_8 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	WeakAtkState01_8(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~WeakAtkState01_8() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState02_1 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	WeakAtkState02_1(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~WeakAtkState02_1() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class WeakAtkState02_2 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	WeakAtkState02_2(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~WeakAtkState02_2() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class StrongAtkState01 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	StrongAtkState01(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~StrongAtkState01() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class StrongAtkState02 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	StrongAtkState02(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~StrongAtkState02() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class StrongAttackState03 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	StrongAttackState03(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~StrongAttackState03() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class StrongAttackState04 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	StrongAttackState04(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~StrongAttackState04() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class StrongAttackState05 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	StrongAttackState05(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~StrongAttackState05() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class StrongAttackState06 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	StrongAttackState06(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~StrongAttackState06() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class StrongAttackState07 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	StrongAttackState07(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~StrongAttackState07() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class StrongAttackState08 : public State<TypeCharacter>
{
public:
	// コンストラクタ
	StrongAttackState08(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~StrongAttackState08() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};
template<typename TypeCharacter>
class DashToEnemyState : public State<TypeCharacter>
{
public:
	// コンストラクタ
	DashToEnemyState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~DashToEnemyState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class SkillSelectState :public State<TypeCharacter>
{
public:
	// コンストラクタ
	SkillSelectState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~SkillSelectState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};


template<typename TypeCharacter>
class DamegeState : public State<TypeCharacter>
{
public:
	// コンストラクタ
	DamegeState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~DamegeState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};

template<typename TypeCharacter>
class DeathState : public State<TypeCharacter>
{
public:
	// コンストラクタ
	DeathState(TypeCharacter* character) :State<TypeCharacter>(character) {};
	// デストラクタ
	~DeathState() {}
	// ステートに入った時のメソッド
	virtual void Enter()override {}
	// ステートで実行するメソッド
	virtual void Execute(float elapsedTime)override {}
	// ステートから出ていくときのメソッド
	virtual void Exit()override {}
};
