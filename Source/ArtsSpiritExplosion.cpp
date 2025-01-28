#include "ArtsSpiritExplosion.h"
#include "Graphics/Graphics.h"

ArtsSpiritExplosion::ArtsSpiritExplosion(ArtsManager* manager) : Arts(manager)
{
    useSpiritEnergy = 300;
    radius = 2;
    damage = 1;
    lifeTimer = 3.0f;
}

ArtsSpiritExplosion::~ArtsSpiritExplosion()
{
}

void ArtsSpiritExplosion::Update(float elapsedTime)
{
    // 経過時間を引いていく
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0.0f)
    {
        // 時間が 0 以下になったら自分を破棄
        Destroy();
    }
    // ワールド行列の更新
    UpdateTransform();
}

// 描画処理
void ArtsSpiritExplosion::Render(ID3D11DeviceContext* dc)
{
    //衝突判定用のデバッグ球を描画
    DebugPrimitive* debugPrimitive = Graphics::Instance()->GetDebugPrimitive();
    debugPrimitive->DrawSphere(position, radius, { 0,0,1,1 });
}

//発射
void ArtsSpiritExplosion::Launch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}