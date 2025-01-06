#pragma once

#include "Graphics/GeometricPrimitive.h"
#include "Arts.h"
#include<memory>
// ‹C’e‹Z’¼i
class ArtsSkillStraightBallet : public Arts
{
public:
    ArtsSkillStraightBallet(ArtsManager* manager);
    ~ArtsSkillStraightBallet()override;

    // XVˆ—
    void Update(float elapsedTime) override;

    // •`‰æˆ—
    void Render(ID3D11DeviceContext* dc) override;

    void Launch(const DirectX::XMFLOAT3& direction,
        const DirectX::XMFLOAT3& position);
private:
    std::unique_ptr<GeometricPrimitive> prim;
    BalletType GetType()override { return BalletType::SkillStraight; }
    float speed = 10.0f;
    float lifeTimer = 3.0f; // õ–½i‚R•bj
};