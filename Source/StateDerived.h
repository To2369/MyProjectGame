#pragma once
#include "Player.h"

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

class BattleState :public HierarchicalState
{
public:
	//コンストラクタ
	BattleState(Player* enemy) :HierarchicalState(enemy) {}
	~BattleState();
	//ステートに入った時のメソッド
	void Enter();
	//ステートで実行するメソッド
	void Execute(float elapsedTime);
	//ステートからでていくときのメソッド
	void Exit();
};
//メタAIからメッセージ受信を受信したときに呼ばれる
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
class AttackState : public State
{
public:
	// コンストラクタ
	AttackState(Player* enemy) :State(enemy) {};
	// デストラクタ
	~AttackState() {}
	// ステートに入った時のメソッド
	void Enter()override;
	// ステートで実行するメソッド
	void Execute(float elapsedTime)override;
	// ステートから出ていくときのメソッド
	void Exit()override;
};

//他のエネミーから呼ばれたとき
class RecoverySkillEnergyState : public State
{
public:
	// コンストラクタ
	RecoverySkillEnergyState(Player* enemy) :State(enemy) {};
	// デストラクタ
	~RecoverySkillEnergyState() {}
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
