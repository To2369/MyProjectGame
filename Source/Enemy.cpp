#include "Enemy.h"
#include "Graphics/Graphics.h"

// デバッグプリミティブの描画
void Enemy::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance()->GetDebugRenderer();

    // 衝突判定用のデバッグ球を描画
    debugRenderer->DrawSphere(position, radius, { 0, 0, 0, 1 });
}