#include "ArtsSpiritExplosion.h"
#include "Graphics/Graphics.h"

ArtsSpiritExplosion::ArtsSpiritExplosion(ArtsManager* manager) : Arts(manager)
{
    model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".//Data//Model//Sword//Sword.fbx");
    const float scale_fcator = 0.0f;	// モデルが大きいのでスケール調整
    scale = { scale_fcator, scale_fcator, scale_fcator };
    useSpiritEnergy = 300;
    radius = 2;
    damage = 1;
    lifeTimer = 3.0f;
}

ArtsSpiritExplosion::~ArtsSpiritExplosion()
{
}

// デバッグプリミティブ描画
void ArtsSpiritExplosion::DrawDebugPrimitive()
{
    //衝突判定用のデバッグ球を描画
    DebugPrimitive* debugPrimitive = Graphics::Instance()->GetDebugPrimitive();
    debugPrimitive->DrawSphere(position, radius, { 0,0,1,1 });
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
    model->Render(dc, transform, { 1.0f,1.0f,1.0f,1.0f });
}

//発射
void ArtsSpiritExplosion::Launch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}