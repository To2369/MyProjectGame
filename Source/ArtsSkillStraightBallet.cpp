#include "ArtsSkillStraightBallet.h"
#include "Graphics/Graphics.h"

ArtsSkillStraightBallet::ArtsSkillStraightBallet(ArtsManager* manager) : Arts(manager)
{
    position = { 3,0,3 };
    damage = 1;
    useSkillEnergy = 100;
    direction = { 0,1,0 };
    height = 5;
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
    //float speed = this->speed * elapsedTime;
    //// 位置 += 方向 * 速さ
    //position.x += direction.x * speed;
    //position.y += direction.y * speed;
    //position.z += direction.z * speed;
    // ワールド行列の更新
    UpdateTransform();
}

// 描画処理
void ArtsSkillStraightBallet::Render(ID3D11DeviceContext* dc)
{
    //衝突判定用のデバッグ球を描画
    DebugPrimitive* debugPrimitive = Graphics::Instance()->GetDebugPrimitive();
    debugPrimitive->DrawCapsule(position, angle,radius,height, { 1, 0, 0, 1 });
}

//発射
void ArtsSkillStraightBallet::Launch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}