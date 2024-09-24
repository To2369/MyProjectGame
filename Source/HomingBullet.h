#pragma once

#include "Graphics/Model.h"
#include "Bullet.h"

// 追尾弾
class HomingBullet : public Bullet
{
public:
    HomingBullet(BulletManager* manager);
    ~HomingBullet() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // 描画処理
    void Render(ID3D11DeviceContext* dc) override;

    // 追尾のターゲットの設定
    void LockonTarget(DirectX::XMFLOAT3 target)
    {
        this->target = target;

        // ターゲットが変更した後に姿勢が変更されるのでこのタイミングで姿勢行列の更新が必要
        UpdateTransform();
    }

    //発射
    void Launch(const DirectX::XMFLOAT3& direction,
        const DirectX::XMFLOAT3& position,
        const DirectX::XMFLOAT3& target);

private:
    std::unique_ptr<Model> model;
    DirectX::XMFLOAT3 target = { 0,0,0 };	// 追尾ターゲット
    float speed = 10.0f; // 移動速度
    float turnSpeed = DirectX::XMConvertToRadians(180); // 最大回転速度
    float lifeTimer = 3.0f; // 寿命(３秒)
};