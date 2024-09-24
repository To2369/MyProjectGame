#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"

void Enemy::Destroy()
{
    EnemyManager::Instance().Remove(this);
}

// �f�o�b�O�v���~�e�B�u�̕`��
void Enemy::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance()->GetDebugRenderer();

    // �Փ˔���p�̃f�o�b�O����`��
    //debugRenderer->DrawSphere(position, radius, { 0, 0, 0, 1 });

    // �Փ˔���p�̃f�o�b�O�p�~����`��
    debugRenderer->DrawCylinder(position, radius, height, { 0, 0, 0, 1 });
}