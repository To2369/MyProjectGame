#pragma once

#include "Graphics/Model.h"
#include "Character.h"
#include "BulletManager.h"
//#include "Effect/Effect.h"

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
public:
	static Player* Instance()
	{
		static Player ins;
		return &ins;
	}
private:
	DirectX::XMFLOAT3 GetMoveVec() const;

	//操作移動
	void InputMove(float elapsedTime);

	// ジャンプ入力処理
	void InputJump();

	// 入力による弾発射処理
	void InputLaunchBullet();

	// プレイヤーと敵との衝突処理
	void CollisionPlayerAndEnemies();

	// 弾と敵の衝突処理
	void CollisionBulletsAndEnemies();

	void InputTeleportBehindEnemy();

	void InputDashTowardsEnemy(float elapsedTime);
private:
	//待機ステートへ遷移
	void TransitionIdleState();

	//待機ステート更新処理
	void UpdateIdleState(float elapsedTime);
protected:
	// 着地したときに呼び出される
	void OnLanding() override;
private:
	std::unique_ptr<Model> model;
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

	bool dash = false;
	//std::unique_ptr<BulletManager> bulletMgr;
	BulletManager bulletMgr;
	float elapsedTime_ = 0;
	//アニメーション
	enum Animation
	{
		Anim_Attack,
		Anim_Death,
		Anim_Falling,
		Anim_GetHit1,
		Anim_GetHit2,
		Anim_Idle,
		Anim_jump,
		Anim_jump_Flip,
		Anim_Landing,
		Anim_Revive,
		Anim_Running,
		Anim_Walking
	};
	enum class State
	{
		Idle,
		Move,
		Jump,
		Land,
		Attack,
		Damage,
		Death,
		Revive,
	};
	State state = State::Idle;
	// ヒットエフェクト
	//std::unique_ptr<Effect> hitEffect;
};