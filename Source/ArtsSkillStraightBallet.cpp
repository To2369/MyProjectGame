#include "ArtsSkillStraightBallet.h"
#include "Graphics/Graphics.h"

ArtsSkillStraightBallet::ArtsSkillStraightBallet(ArtsManager* manager) : Arts(manager)
{
    model = std::make_unique<Model>(Graphics::Instance()->GetDevice(), ".//Data//Model//SpikeBall//SpikeBall.fbx");
    const float scale_fcator = 1.0f;	// モデルが大きいのでスケール調整
    scale = { scale_fcator, scale_fcator, scale_fcator };
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
    // 移動
    float speed = this->speed * elapsedTime;

    // 位置 += 方向 * 速さ
    position.x += direction.x * speed;
    position.y += direction.y * speed;
    position.z += direction.z * speed;

    // ワールド行列の更新
    UpdateTransform();
}

// 描画処理
void ArtsSkillStraightBallet::Render(ID3D11DeviceContext* dc)
{
    model->Render(dc, transform, { 1.0f,1.0f,1.0f,1.0f });
}

//発射
void ArtsSkillStraightBallet::Launch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}