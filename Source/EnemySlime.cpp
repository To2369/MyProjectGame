#include "EnemySlime.h"
#include "Graphics/Graphics.h"

EnemySlime::EnemySlime()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".//Data//Model//Slime//Slime.fbx");

	const float scaleFactor = 0.01f;
	scale = { scaleFactor,scaleFactor,scaleFactor };
}

void EnemySlime::Update(float elapsedTime)
{
	// ‘¬“xˆ—XV
	UpdateVelocity(elapsedTime);

	UpdateTransform();
}

void EnemySlime::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}