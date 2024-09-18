#pragma once
#include "Graphics/Model.h"
#include "Enemy.h"

// �X���C��
class EnemySlime :public Enemy
{
public:
	EnemySlime();
	~EnemySlime() {};

	void Update(float elapsedTime) override;

	void Render(ID3D11DeviceContext* dc) override;

private:
	std::unique_ptr<Model> model;
};