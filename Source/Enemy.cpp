#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"

void Enemy::Destroy()
{
    EnemyManager::Instance().Remove(this);
}

// デバッグプリミティブの描画
void Enemy::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance()->GetDebugRenderer();

    // 衝突判定用のデバッグ球を描画
    //debugRenderer->DrawSphere(position, radius, { 0, 0, 0, 1 });

    // 衝突判定用のデバッグ用円柱を描画
    debugRenderer->DrawCylinder(position, radius, height, { 0, 0, 0, 1 });
}