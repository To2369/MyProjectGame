#pragma once
#include "Enemy.h"

// スライム
class EnemySlime :public Enemy
{
public:
	EnemySlime();
	~EnemySlime() {};

	void Update(float elapsedTime) override;

	void Render(ID3D11DeviceContext* dc) override;

	void DrawDebugGUI() override;

	// 死亡したときに呼ばれる
	void OnDead() override;
private:
};