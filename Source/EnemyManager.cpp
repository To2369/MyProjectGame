#include "EnemyManager.h"
#include "Collision.h"
void EnemyManager::Update(float elapsedTime)
{
    for (const auto& enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

    //�j������
    for (Enemy* enemy : removes)
    {
        auto itr = std::find_if(enemies.begin(), enemies.end(),
            [&enemy](const std::unique_ptr<Enemy>& ptr) {
                return ptr.get() == enemy;
            });
        if (itr != enemies.end())
        {

            enemies.erase(itr);
        }
        //delete enemy;
    }
    //�j�����X�g���N���A
    removes.clear();

    //�G���m�̏Փˏ���
    //CollisionEnemyVsEnemies();
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

//�G�l�~�[�S�폜
void EnemyManager::Clear()
{
    enemies.clear();
}

void EnemyManager::CollisionEnemiesAndEnemies()
{
    //�S�Ă̓G�Ƒ�������ŏՓˏ���
    int enemyCount = GetEnemyCount();
    for (int i = 0; i < enemyCount; i++)
    {
        Enemy* enemyA = enemies.at(i).get();
        for (int j = i + 1; j < enemyCount; ++j)
        {
            Enemy* enemyB = enemies.at(j).get();
            // �Փ˕���
            DirectX::XMFLOAT3 outVec;

            //�Փˏ���
            if (Collision::IntersectCylinderVsCylinder(
                (enemyA->GetPosition()),
                enemyA->GetRadius(),
                enemyA->GetHeight(),
                (enemyB->GetPosition()),
                enemyB->GetRadius(),
                enemyB->GetHeight(),
                outVec))
            {
                //OutputDebugStringA("�Փ�\n");

                //���a�̍��v
                float range = enemyA->GetRadius() + enemyB->GetRadius();

                //�Փ˕����̃x�N�g���𔼌a�̍��v�����ɃX�P�[�����O
                DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), range);

                //���̃x�N�g�����Փ˂��鑤�̈ʒu�ɑ���
                vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&enemyA->GetPosition()), vec);

                //���̈ʒu�x�N�g�����Փ˂���鑤�ɐݒ肷��
                DirectX::XMFLOAT3 pos;
                DirectX::XMStoreFloat3(&pos, vec);
                enemyB->SetPosition(pos);
            }
        }
    }
}

//�f�o�b�O�pGUI�`��
void EnemyManager::DrawDebugGUI()
{
    for (const auto& enemy : enemies)
    {
        enemy->DrawDebugGUI();
    }
}

// �f�o�b�O�v���~�e�B�u�`��
void EnemyManager::DrawDebugPrimitive()
{
	for (const auto& enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}

void EnemyManager::Remove(Enemy* enemy)
{
    //�j�����X�g�ɒǉ�
    removes.insert(enemy);
}