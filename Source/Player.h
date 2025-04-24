#pragma once

#include "Character.h"
#include "ArtsManager.h"
#include "Graphics/GeometricPrimitive.h"
#include "StateMachine.h"
#include "Enemy.h"

//#include "Effect/Effect.h"


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

	//ステートマシン取得
	StateMachine<Player>* GetStateMachine() { return stateMachine.get(); }
public:
	static Player* Instance()
	{
		static Player ins;
		return &ins;
	}
	enum class State
	{
		Movement,
		WeakAttack,
		StrongAttack,
		UseSkill,
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
	enum class StrongAttack
	{
		StrongAttack01,
		StrongAttack02,
		StrongAttack03,
		StrongAttack04,
		StrongAttack05,
		StrongAttack06,
		StrongAttack07,
		StrongAttack08,
	};
	enum class WeakAttack
	{
		WeakAttack01,
		WeakAttack02,
		WeakAttack03,
		WeakAttack04,
		WeakAttack05,
		WeakAttack06,
	};

	enum class UseSkill
	{
		SkillSelect,
	};
	enum class HitDamege
	{
		Damege,
		Death,
	};

	//操作移動
	bool InputMove(float elapsedTime);

	// ジャンプ入力処理
	bool InputJump();

	bool InputArts(float elapsedTime);

	bool InputDashTowardsEnemy(float elapsedTime);

	bool InputDash(float elapsedTime);
	bool InputAttack();

	bool artSkillReady = false;
	bool artUltSkillReady = false;
	void InputFlying(float elapsedTime);
	DirectX::XMFLOAT3 GetMoveVec(const DirectX::XMFLOAT3& cameraRight, const DirectX::XMFLOAT3& cameraFront) const;

	void Lock();

	// プレイヤーと敵との衝突処理
	void CollisionPlayerAndEnemies();

	// 弾と敵の衝突処理
	void CollisionArtsAndEnemies();
	void CollisionPlayerAndArts();
	void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius, float invicibleTimer, int damage);

	void InputAttackNext(float currentAnimSeconds, float inputAcceptStartTime, float inputAcceptEndTime);
	void ActiveAttackCollider(float currentAnimSeconds, float hitStartTime, float hitEndTime, const char* boneName, float hitRadius, float invTimer, float damage);
	void TeleportBehindEnemy();
protected:
	// 着地したときに呼び出される
	void OnLanding() override;
private:
	std::unique_ptr<StateMachine<Player>> stateMachine;
	std::unique_ptr<GeometricPrimitive> geo;

	enum class LockonState
	{
		NotLocked,
		Locked,
	};
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
	int jumpLimit = 2;	// ジャンプ制限（最大ジャンプ数、ひとまず２段ジャンプ可）
	ArtsManager artsMgr;
	float elapsedTime_ = 0;

	enum class Skill
	{
		SpiritBlast,
		SkillMax,
	};
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


	int hit = 0;

	DirectX::XMFLOAT3	lockDirection;

	bool lockon = false;

	LockonState lockonState = LockonState::NotLocked;
	Enemy* lockonEnemy = nullptr;
};