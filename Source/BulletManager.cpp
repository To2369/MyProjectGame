#include "BulletManager.h"

BulletManager::BulletManager()
{

}

BulletManager::~BulletManager()
{
   Clear();
}

// 更新処理
void BulletManager::Update(float elapsedTime)
{
    for (Bullet* bullet : bullets)
    {
        bullet->Update(elapsedTime);
    }

    // 破棄処理
    for (Bullet* bullet : removes)
    {
        auto itr = std::find(bullets.begin(), bullets.end(), bullet);
        if (itr != bullets.end())
        {
            bullets.erase(itr);
        }
        // bullets から削除
        delete bullet;
    }
    // 破棄リストをクリア
    removes.clear();
}

// 描画処理
void BulletManager::Render(ID3D11DeviceContext* dc)
{
    for (Bullet* bullet : bullets)
    {
        bullet->Render(dc);
    }
}

// デバッグプリミティブ描画
void BulletManager::DrawDebugPrimitive()
{
    for (Bullet* bullet : bullets)
    {
        bullet->DrawDebugPrimitive();
    }
}

// 弾登録
void BulletManager::Regist(Bullet* bullet)
{
    bullets.emplace_back(bullet);
}

// 弾全削除
void BulletManager::Clear()
{
    for (Bullet* bullet : bullets)
    {
        delete bullet;
    }
    bullets.clear();
}

// 弾削除
void BulletManager::Remove(Bullet* bullet)
{
    auto itr = std::find(removes.begin(), removes.end(), bullet);

    // 重複していたら追加しない
    if (itr != removes.end())
        return;

    // 破棄リストに追加
    removes.push_back(bullet);
}