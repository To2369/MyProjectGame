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

	void Moving(float elapsedTime, float speedRate);
	bool SearchPlayer();
	void Lockon();
	void SetRandomTargetPosition();
	//ナワバリ設定
	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);
	// 死亡したときに呼ばれる
	void OnDead() override;

	void Test(float elapsedTime);
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
};