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

}