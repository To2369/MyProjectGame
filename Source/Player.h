#pragma once

#include "Graphics/Model.h"
#include "Character.h"
#include "BulletManager.h"
#include "ArtsManager.h"
#include "Graphics/GeometricPrimitive.h"
#include "StateMachine.h"
//#include "Effect/Effect.h"


//アニメーション
enum Animation
{
	AnimIdle,
	AnimConbatIdle,
	AnimConbatToIdle,
	AnimIdleToConbat,
	AnimConbo01_1,
	AnimConbo01_2,
	AnimConbo01_3,
	AnimConbo01_4,
	AnimConbo02_1,
	AnimConbo02_2,
	AnimConbo02_3,
	AnimConbo02_4,
	AnimConbo03_1,
	AnimConbo03_2,
	AnimConbo03_3,
	AnimConbo03_4,
	AnimConbo04_1,
	AnimConbo04_2,
	AnimConbo04_3,
	AnimConbo04_4,
	AnimBuff,
	AnimChargeAttackStart,
	AnimChargeAttackEnd,
	AnimWalk,
	AnimRun,
	AnimJumpStart,
	AnimJumpEnd,
	AnimStep,
	AnimHit,
	AnimDeathHit,
	AnimHitHeavyStart,
	AnimHitHeavyEnd,
	AnimGetUp,
	BlockHit,
	FullAttack
};
// プレイヤー
class Player :public Character
{
public:
	Player();
	~Player()override;

	void Update(float elapsedTime);

	void Render(ID3D11DeviceContext* dc);

	//デバッグ用GUI描画
	void DrawDebugGUI();

	// デバッグプリミティブ表示
	void DrawDebugPrimitive();
	Model* GetModel() { return model.get(); }
public:
	static Player* Instance()
	{
		static Player ins;
		return &ins;
	}
private:
	DirectX::XMFLOAT3 GetMoveVec() const;

	void Lock();
	//操作移動
	bool InputMove(float elapsedTime);

	// ジャンプ入力処理
	void InputJump();

	// 入力による弾発射処理
	void InputLaunchBullet();

	void InputArts();

	// プレイヤーと敵との衝突処理
	void CollisionPlayerAndEnemies();

	// 弾と敵の衝突処理
	void CollisionBulletsAndEnemies();

	void CollisionArtsAndEnemies();
	void CollisionPlayerAndArts();
	void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

	void TeleportBehindEnemy();

	bool InputDashTowardsEnemy(float elapsedTime);

	bool InputRecoverySkillEnergy(float elapsedTime);

	bool InputDash(float elapsedTime);
	bool InputAttack();

	void UpdateAnimation(float elapsedTime);

private:
	//待機ステート
	void TransitionIdleState();

	//待機ステート更新処理
	void UpdateIdleState(float elapsedTime);

	//移動ステート
	void TransitionMoveState();

	//移動ステート更新処理
	void UpdateMoveState(float elapsedTime);

	//ダッシュステート
	void TransitionDashState();

	//ダッシュステート更新処理
	void UpdateDashState(float elapsedTime);

	//敵へダッシュステート
	void TransitionDashToEnemyState();

	//敵へダッシュステート更新処理
	void UpdateDashToEnemyState(float elapsedTime);

	// 技力回復
	void TransitionRecoverySkillEnergy();
	// 技力回復更新処理
	void UpdateRecoverySkillEnergyState(float elapsedTime);

	void TransitionAttackState();
	void UpdateAttackState(float elapsedTime);
protected:
	// 着地したときに呼び出される
	void OnLanding() override;
private:
	std::unique_ptr<Model> model;
	std::unique_ptr<GeometricPrimitive> geo;
	// 移動スピード
	float moveSpeed = 5.0f;
	// 旋回スピード
	float turnSpeed = DirectX::XMConvertToRadians(720);

	// ジャンプ力
	float jumpSpeed = 20.0f;
	// 重力
	float gravity = -1.0f;
	// 速度
	DirectX::XMFLOAT3 velocity = { 0,0,0 };

	int jumpCount = 0;	// ジャンプ回数
	int jumpLimit = 1;	// ジャンプ制限（最大ジャンプ数、ひとまず２段ジャンプ可）
	//std::unique_ptr<BulletManager> bulletMgr;
	BulletManager bulletMgr;
	ArtsManager artsMgr;
	float elapsedTime_ = 0;
	enum class State
	{
		Movement,
		Battle,
		HitDamege,
	};
	enum class Movement
	{
		Idle,
		Move,
		Dash,
		Jump,
		Land,
	};
	enum class Battle
	{
		Attack,
		RecoverySkillEnergy,
		DashToEnemy,
	};
	enum class HitDamege
	{
		Damege,
		Death,
	};
	enum class Skill
	{
		SpiritBlast,
		SkillMax,
	};
	std::unique_ptr<StateMachine<Player>> stateMachine;
	//State state = State::Idle;
	Skill skill = Skill::SkillMax;
	bool dashTowardEnemyFlag = false;
	// ヒットエフェクト
	//std::unique_ptr<Effect> hitEffect;
	float PIDIV180 = 0.017452f;


	DirectX::XMFLOAT3 p = {0,0,0};
	DirectX::XMFLOAT3 s = { 1,1,1 };
	DirectX::XMFLOAT4X4 t = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};


	float totalAnimationTime = 0;
};