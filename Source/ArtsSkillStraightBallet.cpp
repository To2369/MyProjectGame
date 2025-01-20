#include "ArtsSkillStraightBallet.h"
#include "Graphics/Graphics.h"
ArtsSkillStraightBallet::ArtsSkillStraightBallet(ArtsManager* manager) : Arts(manager)
{
    height = 1;
    geoPrimitive =
        std::make_unique<CapsuleOneWay>(Graphics::Instance()->GetDevice(), height, radius, 12, 6, 6);
    geoPrimitive->SetGrowthRate(15.0f);
    position = { 3,0,3 };
    damage = 1;
    useSkillEnergy = 100;
    direction = { 0,0,0 };
}

ArtsSkillStraightBallet::~ArtsSkillStraightBallet()
{
}

void ArtsSkillStraightBallet::Update(float elapsedTime)
{
    // 経過時間を引いていく
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0.0f)
    {
        // 時間が 0 以下になったら自分を破棄
        Destroy();
    }
    currentCapsuleHeight = geoPrimitive->GetCurrentHeight();
    geoPrimitive->Update(elapsedTime);
    // ワールド行列の更新
    UpdateTransform();
}

// 描画処理
void ArtsSkillStraightBallet::Render(ID3D11DeviceContext* dc)
{
    geoPrimitive->Render(dc, transform, { 1,0,0,1 });
}

//発射
void ArtsSkillStraightBallet::Launch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}