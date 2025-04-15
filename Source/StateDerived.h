#pragma once
#include "Player.h"
#include <optional>
struct AttackData
{
	float hitStartTime;
	float hitEndTime;
	float inputAcceptStartTime;
	float inputAcceptEndTime;
	float progressLimit;
	float hitRadius;
	const char* hitBoneName;

	/*アニメーションに二つ以上攻撃判定があるときの情報*/
	float secondHitStartTime = 0.0f;
	float secondHitEndTime = 0.0f;
	const char* secondHitBoneName = nullptr;
};
namespace AttackDatas
{
	extern const AttackData weakAttack01;
	extern const AttackData weakAttack02;
	extern const AttackData weakAttack03;
	extern const AttackData weakAttack04;
	extern const AttackData weakAttack05;
	extern const AttackData weakAttack06;
}

/*メインステート*/
class MovementState :public HierarchicalState
{
public:
	//コンストラクタ
	MovementState(Player* enemy) :HierarchicalState(enemy) {}
	~MovementState();
	//ステートに入った時のメソッド
	void Enter();
	//ステートで実行するメソッド
	void Execute(float elapsedTime);
	//ステートからでていくときのメソッド
	void Exit();
};

class WeakAttackState :public HierarchicalState
{
public:
	//コンストラクタ
	WeakAttackState(Player* enemy) :HierarchicalState(enemy) {}
	~WeakAttackState();
	//ステートに入った時のメソッド
	void Enter();
	//ステートで実行するメソッド
	void Execute(float elapsedTime);
	//ステートからでていくときのメソッド
	void Exit();
};

class UseSkillState :public HierarchicalState
{
public:
	//コンストラクタ
	UseSkillState(Player* enemy) :HierarchicalState(enemy) {}
	~UseSkillState();
	//ステートに入った時のメソッド
	void Enter();
	//ステートで実行するメソッド
	void Execute(float elapsedTime);
	//ステートからでていくときのメソッド
	void Exit();
};

class HitDamegeState :public HierarchicalState
{
public:
	//コンストラクタ
	HitDamegeState(Player* enemy) :HierarchicalState(enemy) {}
	~HitDamegeState();
	//ステートに入った時のメソッド
	void Enter();
	//ステートで実行するメソッド
	void Execute(float elapsedTime);
	//ステートからでていくときのメソッド
	void Exit();
};
/***********************************************************************************************/

// 待機ステートオブジェクト
class IdleState : public State
{
public:
	// コンストラクタ
	IdleState(Player* enemy) :State(enemy) {};
	// デストラクタ
	~IdleState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};

// 追跡ステートオブジェクト
class MoveState : public State
{
public:
	// コンストラクタ
	MoveState(Player* enemy) :State(enemy) {};
	// デストラクタ
	~MoveState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};

// 追跡ステートオブジェクト
class DashState : public State
{
public:
	// コンストラクタ
	DashState(Player* enemy) :State(enemy) {};
	// デストラクタ
	~DashState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};

// 追跡ステートオブジェクト
class JumpState: public State
{
public:
	// コンストラクタ
	JumpState(Player* enemy) :State(enemy) {};
	// デストラクタ
	~JumpState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};

// 追跡ステートオブジェクト
class LandState : public State
{
public:
	// コンストラクタ
	LandState(Player* enemy) :State(enemy) {};
	// デストラクタ
	~LandState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};

// 攻撃ステートオブジェクト
class WeakAttackState01 : public State
{
public:
	// コンストラクタ
	WeakAttackState01(Player* enemy) :State(enemy) {};
	// デストラクタ
	~WeakAttackState01() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};

// 攻撃ステートオブジェクト
class WeakAttackState02 : public State
{
public:
	// コンストラクタ
	WeakAttackState02(Player* enemy) :State(enemy) {};
	// デストラクタ
	~WeakAttackState02() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};
// 攻撃ステートオブジェクト
class WeakAttackState03 : public State
{
public:
	// コンストラクタ
	WeakAttackState03(Player* enemy) :State(enemy) {};
	// デストラクタ
	~WeakAttackState03() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};
// 攻撃ステートオブジェクト
class WeakAttackState04 : public State
{
public:
	// コンストラクタ
	WeakAttackState04(Player* enemy) :State(enemy) {};
	// デストラクタ
	~WeakAttackState04() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};
// 攻撃ステートオブジェクト
class WeakAttackState05 : public State
{
public:
	// コンストラクタ
	WeakAttackState05(Player* enemy) :State(enemy) {};
	// デストラクタ
	~WeakAttackState05() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};
// 攻撃ステートオブジェクト
class WeakAttackState06 : public State
{
public:
	// コンストラクタ
	WeakAttackState06(Player* enemy) :State(enemy) {};
	// デストラクタ
	~WeakAttackState06() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};
//戦闘待機
class DashToEnemyState : public State
{
public:
	// コンストラクタ
	DashToEnemyState(Player* enemy) :State(enemy) {};
	// デストラクタ
	~DashToEnemyState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};

class SkillSelectState :public State
{
public:
	// コンストラクタ
	SkillSelectState(Player* enemy) :State(enemy) {};
	// デストラクタ
	~SkillSelectState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};
//戦闘待機
class DamegeState : public State
{
public:
	// コンストラクタ
	DamegeState(Player* enemy) :State(enemy) {};
	// デストラクタ
	~DamegeState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};

class DeathState : public State
{
public:
	// コンストラクタ
	DeathState(Player* enemy) :State(enemy) {};
	// デストラクタ
	~DeathState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};
