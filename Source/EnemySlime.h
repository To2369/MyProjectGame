#pragma once
#include "Enemy.h"

// �X���C��
class EnemySlime :public Enemy
{
public:
	EnemySlime();
	~EnemySlime() {};

	void Update(float elapsedTime) override;

	void Render(ID3D11DeviceContext* dc) override;

	void DrawDebugGUI() override;

	// ���S�����Ƃ��ɌĂ΂��
	void OnDead() override;
private:
};