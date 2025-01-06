#pragma once
#include "Graphics/GeometricPrimitive.h"
#include "Arts.h"
#include<memory>
// ãCíeãZíºêi
class NormalBallet : public Arts
{
public:
    NormalBallet(ArtsManager* manager);
    ~NormalBallet()override;

    // çXêVèàóù
    void Update(float elapsedTime) override;

    // ï`âÊèàóù
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
    std::unique_ptr<GeometricPrimitive> prim;
    BalletType GetType()override { return BalletType::Normal; }
    DirectX::XMFLOAT3 target = { 0,0,0 };
    float turnSpeed = DirectX::XMConvertToRadians(180);
    float speed = 10.0f;
    float lifeTimer = 3.0f; // éıñΩÅiÇRïbÅj
};