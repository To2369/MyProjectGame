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

// デバッグプリミティブ描画
void EnemyManager::DrawDebugPrimitive()
{
	for (const auto& enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}