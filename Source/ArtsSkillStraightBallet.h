#pragma once

#include "Graphics/CapsuleOneWay.h"
#include "Arts.h"
#include<memory>
// �C�e�Z���i
class ArtsSkillStraightBallet : public Arts
{
public:
    ArtsSkillStraightBallet(ArtsManager* manager);
    ~ArtsSkillStraightBallet()override;

    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render(ID3D11DeviceContext* dc) override;

    void Launch(const DirectX::XMFLOAT3& direction,
        const DirectX::XMFLOAT3& position);
private:
    std::unique_ptr<CapsuleOneWay> geoPrimitive;
    BalletType GetType()override { return BalletType::SkillStraight; }
    float speed = 10.0f;
    float lifeTimer = 3.0f; // �����i�R�b�j
};