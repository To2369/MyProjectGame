#pragma once

#include "Graphics/Model.h"
#include "Bullet.h"

// ���i�e
class StraightBullet : public Bullet
{
public:
    StraightBullet(BulletManager* manager);
    ~StraightBullet()override;

    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render(ID3D11DeviceContext* dc) override;

    void Launch(const DirectX::XMFLOAT3& direction,
        const DirectX::XMFLOAT3& position);
private:
    std::unique_ptr<Model> model;
    float speed = 10.0f;
    float lifeTimer = 3.0f; // �����i�R�b�j
};