#include "EnemyManager.h"

void EnemyManager::Update(float elapsedTime)
{
	for (const auto& enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}
}

void EnemyManager::Render(ID3D11DeviceContext* dc)
{
	for (const auto& enemy : enemies)
	{
		enemy->Render(dc);
	}
}

void EnemyManager::Regist(std::unique_ptr<Enemy> enemy)
{
	enemies.emplace_back(std::move(enemy));
}

// �f�o�b�O�v���~�e�B�u�`��
void EnemyManager::DrawDebugPrimitive()
{
	for (const auto& enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}