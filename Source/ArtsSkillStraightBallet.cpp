#include "ArtsSkillStraightBallet.h"
#include "Graphics/Graphics.h"

ArtsSkillStraightBallet::ArtsSkillStraightBallet(ArtsManager* manager) : Arts(manager)
{
    height = 1.0f;

    prim = std::make_unique<GeometricCapsule>(Graphics::Instance()->GetDevice(), height, DirectX::XMFLOAT3{ radius,radius,radius }, 12, 6, 6, DirectX::XMFLOAT3{ 0,0,0 });
    const float scale_fcator = 1.0f;	// モデルが大きいのでスケール調整
    scale = { scale_fcator, scale_fcator, scale_fcator };
    position = { 1,5,1 };
    //angle.x = DirectX::XMConvertToRadians(90);
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
    //angle.x = DirectX::XMConvertToRadians(90);
    // 移動
    float speed = this->speed * elapsedTime;
    // 位置 += 方向 * 速さ
   /* position.x += direction.x * speed;
    position.y += direction.y * speed;
    position.z += direction.z * speed;*/
    // ワールド行列の更新
    UpdateTransform();
}

// 描画処理
void ArtsSkillStraightBallet::Render(ID3D11DeviceContext* dc)
{
    prim->Render(dc, transform, { 1.0f,1.0f,1.0f,1.0f });
}

//発射
void ArtsSkillStraightBallet::Launch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}