#pragma once
#include "Graphics/Model.h"
#include "Enemy.h"

// ƒXƒ‰ƒCƒ€
class EnemySlime :public Enemy
{
public:
	EnemySlime();
	~EnemySlime() {};

	void Update(float elapsedTime) override;

	void Render(ID3D11DeviceContext* dc) override;

	// Ž€–S‚µ‚½‚Æ‚«‚ÉŒÄ‚Î‚ê‚é
	void OnDead() override;
private:
	std::unique_ptr<Model> model;
};