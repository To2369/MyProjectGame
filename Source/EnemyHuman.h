#pragma once
#include "Enemy.h"
#include "Player.h"

class EnemyHuman :public Enemy
{
public:
	EnemyHuman();
	~EnemyHuman() {};

	void Update(float elapsedTime) override;

	void Render(ID3D11DeviceContext* dc) override;

	void DrawDebugGUI() override;

	bool SearchPlayer();
	void SetRandomTargetPosition();
	//ナワバリ設定
	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);
	// 死亡したときに呼ばれる
	void OnDead() override;

private:
	
	Player* lockonPlayer = nullptr;
	float moveSpeed = 3.0f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	DirectX::XMFLOAT3 ppos;
	DirectX::XMFLOAT3 targetPosition = { 0,0,0 };
	DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
	float territoryRange = 10.0f;
	float reset = 3;
	float dist = 0.0f;
	float searchRange = 5.0f;
	float stateTimer = 0.0f;

	//アニメーション
	enum AnimationNum
	{
		AnimTpose,
		AnimIdleNormal,
		AnimConbatIdle,
		AnimConbo01_1,
		AnimConbo01_2,
		AnimConbo01_3,
		AnimConbo01_4,
		AnimConbo01_5,
		AnimConbo01_6,
		AnimConbo02_1,
		AnimConbo02_2,
		AnimConbo02_3,
		AnimAttack01,
		AnimAttack02,
		AnimAttack03,
		AnimAttack04,
		AnimAttack05,
		AnimAttack06,
		AnimAttack07,
		AnimAttack08,
		AnimDefense,
		AnimDead,
		AnimDownEnd,
		AnimDownStart,
		AnimHitBack,
		AnimHitFront,
		AnimHitLeft,
		AnimHitRight,
		AnimJumpFalling,
		AnimJumpLanding,
		AnimJumpStart,
		AnimRun,
		AnimWalk,
		AnimRise,
		AnimUnequip,
	};
private:

	//目標地点へ移動
	void MoveToTarget(float elapsedTime, float speedRate);


	//ノードとプレイヤーの衝突処理
	void CollisionNodeVsPlayer(const char* nodeName, float boneRadius);

	//俳諧ステートへ遷移
	void TransitionWanderState();

	//俳諧ステート更新処理
	void UpdateWanderState(float elapsedTime);

	//待機ステートへ遷移
	void TransitionIdleState();

	//待機ステート更新処理
	void UpdateIdleState(float elapsedTime);

	//追跡ステートへ遷移
	void TransitionPursuitState();

	//追跡ステート更新処理
	void UpdatePursuitState(float elapsedTime);

	//攻撃ステートへ遷移
	void TransitionAttackState();

	//攻撃ステート更新処理
	void UpdateAttackState(float elapsedTime);

	//戦闘待機ステートへ遷移
	void TransitionIdleBattleState();

	//戦闘待機ステート更新処理
	void UpdateIdleBattleState(float elapsedTime);

	//ダメージステートへ遷移
	void TransitionDamageState();

	//ダメージステート更新処理
	void UpdateDamageState(float elapsedTime);

	//死亡ステートへ遷移
	void TransitionDeathState();

	//死亡ステート更新処理
	void UpdateDeathState(float elapsedTime);
	//ステート
	enum class State
	{
		Wander,
		Idle,
		Pursuit,
		Attack,
		IdleBattle,
		Damage,
		Death,
	};
	State state = State::Wander;
	float attackRange = 1.5f;
};