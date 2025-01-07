#pragma once
#include "Arts.h"
#include<memory>
// �C�e�Z���i
class NormalBallet : public Arts
{
public:
    NormalBallet(ArtsManager* manager);
    ~NormalBallet()override;

    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render(ID3D11DeviceContext* dc) override;

    void Launch(const DirectX::XMFLOAT3& direction,
        const DirectX::XMFLOAT3& position,
        const DirectX::XMFLOAT3& target);

    void LockonTarget(DirectX::XMFLOAT3 target)
    {
        this->target = target;

        UpdateTransform();
    }
private:
    BalletType GetType()override { return BalletType::Normal; }
    DirectX::XMFLOAT3 target = { 0,0,0 };
    float turnSpeed = DirectX::XMConvertToRadians(180);
    float speed = 10.0f;
    float lifeTimer = 3.0f; // �����i�R�b�j
};