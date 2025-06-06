#pragma once

#include "Graphics/CapsuleOneWay.h"
#include "Arts.h"
#include<memory>
// 気弾技直進
class ArtsSkillStraightBallet : public Arts
{
public:
    ArtsSkillStraightBallet(ArtsManager* manager);
    ~ArtsSkillStraightBallet()override;

    // 更新処理
    void Update(float elapsedTime) override;

    // 描画処理
    void Render(ID3D11DeviceContext* dc) override;

    void Launch(const DirectX::XMFLOAT3& direction,
        const DirectX::XMFLOAT3& position);
private:
    std::unique_ptr<CapsuleOneWay> geoPrimitive;
    BalletType GetType()override { return BalletType::SkillStraight; }
    float speed = 10.0f;
};