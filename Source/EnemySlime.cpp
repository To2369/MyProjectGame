#include "EnemySlime.h"
#include "Graphics/Graphics.h"

EnemySlime::EnemySlime()
{
	model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".//Data//Model//Slime//Slime.fbx");

	const float scaleFactor = 0.01f;
	scale = { scaleFactor,scaleFactor,scaleFactor };

	radius = 0.5f;  // ��
	height = 1.0f;  // ����
}

void EnemySlime::Update(float elapsedTime)
{
	// ���x�����X�V
	UpdateVelocity(elapsedTime);

	// ���G���Ԃ̍X�V
	UpdateInvincibleTimer(elapsedTime);

	UpdateTransform();
}

void EnemySlime::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}

// ���S�����Ƃ��ɌĂ΂��
void EnemySlime::OnDead()
{
	Destroy();
}