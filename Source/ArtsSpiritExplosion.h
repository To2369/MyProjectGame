#pragma once

#include "Graphics/Model.h"
#include "Arts.h"

// 気弾技直進
class ArtsSpiritExplosion : public Arts
{
public:
    ArtsSpiritExplosion(ArtsManager* manager);
    ~ArtsSpiritExplosion()override;

    //void DrawDebugPrimitive() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // 描画処理
    void Render(ID3D11DeviceContext* dc) override;

    void Launch(const DirectX::XMFLOAT3& direction,
        const DirectX::XMFLOAT3& position);
private:
    std::unique_ptr<Model> model;
    BalletType GetType()override { return BalletType::SpiritExplosion; }
};