#pragma once
#include "Enemy.h"

class EnemyHuman :public Enemy
{
public:
	EnemyHuman();
	~EnemyHuman() {};

	void Update(float elapsedTime) override;

	void Render(ID3D11DeviceContext* dc) override;

	void DrawDebugGUI() override;

	// Ž€–S‚µ‚½‚Æ‚«‚ÉŒÄ‚Î‚ê‚é
	void OnDead() override;
private:
};