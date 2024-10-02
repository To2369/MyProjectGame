#include "ArtsManager.h"

ArtsManager::ArtsManager()
{

}

ArtsManager::~ArtsManager()
{
    Clear();
}

// 更新処理
void ArtsManager::Update(float elapsedTime)
{
    for (Arts* arts : artses)
    {
        arts->Update(elapsedTime);
    }

    // 破棄処理
    for (Arts* arts : removes)
    {
        auto itr = std::find(artses.begin(), artses.end(), arts);
        if (itr != artses.end())
        {
            artses.erase(itr);
        }
        // bullets から削除
        delete arts;
    }
    // 破棄リストをクリア
    removes.clear();
}

// 描画処理
void ArtsManager::Render(ID3D11DeviceContext* dc)
{
    for (Arts* arts : artses)
    {
        arts->Render(dc);
    }
}

// デバッグプリミティブ描画
void ArtsManager::DrawDebugPrimitive()
{
    for (Arts* arts : artses)
    {
        arts->DrawDebugPrimitive();
    }
}

// 弾登録
void ArtsManager::Regist(Arts* arts)
{
    artses.emplace_back(arts);
}

// 弾全削除
void ArtsManager::Clear()
{
    for (Arts* arts : artses)
    {
        delete arts;
    }
    artses.clear();
}

// 弾削除
void ArtsManager::Remove(Arts* arts)
{
    auto itr = std::find(removes.begin(), removes.end(), arts);

    // 重複していたら追加しない
    if (itr != removes.end())
        return;

    // 破棄リストに追加
    removes.push_back(arts);
}