#include "EnemySlime.h"
#include "Graphics/Graphics.h"

EnemySlime::EnemySlime()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".//Data//Model//Slime//Slime.fbx");

	const float scaleFactor = 0.01f;
	scale = { scaleFactor,scaleFactor,scaleFactor };

	radius = 0.5f;  // 幅
	height = 1.0f;  // 高さ
}

void EnemySlime::Update(float elapsedTime)
{
	// 速度処理更新
	UpdateVelocity(elapsedTime);

	// 無敵時間の更新
	UpdateInvincibleTimer(elapsedTime);

	UpdateTransform();
}

void EnemySlime::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}

// 死亡したときに呼ばれる
void EnemySlime::OnDead()
{
	Destroy();
}