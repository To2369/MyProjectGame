#include "EnemyManager.h"
#include "Collision.h"
void EnemyManager::Update(float elapsedTime)
{
    for (const auto& enemy : enemies)
	{
		enemy->Update(elapsedTime);
	}

    //破棄処理
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
    //破棄リストをクリア
    removes.clear();

    //敵同士の衝突処理
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

//エネミー全削除
void EnemyManager::Clear()
{
    enemies.clear();
}

void EnemyManager::CollisionEnemiesAndEnemies()
{
    //全ての敵と総当たりで衝突処理
    int enemyCount = GetEnemyCount();
    for (int i = 0; i < enemyCount; i++)
    {
        Enemy* enemyA = enemies.at(i).get();
        for (int j = i + 1; j < enemyCount; ++j)
        {
            Enemy* enemyB = enemies.at(j).get();
            // 衝突方向
            DirectX::XMFLOAT3 outVec;

            //衝突処理
            if (Collision::IntersectCylinderAndCylinder(
                (enemyA->GetPosition()),
                enemyA->GetRadius(),
                enemyA->GetHeight(),
                (enemyB->GetPosition()),
                enemyB->GetRadius(),
                enemyB->GetHeight(),
                outVec))
            {
                //OutputDebugStringA("衝突\n");

                //半径の合計
                float range = enemyA->GetRadius() + enemyB->GetRadius();

                //衝突方向のベクトルを半径の合計長さにスケーリング
                DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), range);

                //そのベクトルを衝突する側の位置に足す
                vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&enemyA->GetPosition()), vec);

                //その位置ベクトルを衝突される側に設定する
                DirectX::XMFLOAT3 pos;
                DirectX::XMStoreFloat3(&pos, vec);
                enemyB->SetPosition(pos);
            }
        }
    }
}

//デバッグ用GUI描画
void EnemyManager::DrawDebugGUI()
{
    for (const auto& enemy : enemies)
    {
        enemy->DrawDebugGUI();
    }
}

// デバッグプリミティブ描画
void EnemyManager::DrawDebugPrimitive()
{
	for (const auto& enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}
}

void EnemyManager::Remove(Enemy* enemy)
{
    //破棄リストに追加
    removes.insert(enemy);
}