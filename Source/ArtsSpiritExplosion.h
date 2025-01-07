#pragma once

#include "Graphics/Model.h"
#include "Arts.h"

// �C�e�Z���i
class ArtsSpiritExplosion : public Arts
{
public:
    ArtsSpiritExplosion(ArtsManager* manager);
    ~ArtsSpiritExplosion()override;

    //void DrawDebugPrimitive() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render(ID3D11DeviceContext* dc) override;

    void Launch(const DirectX::XMFLOAT3& direction,
        const DirectX::XMFLOAT3& position);
private:
    std::unique_ptr<Model> model;
    BalletType GetType()override { return BalletType::SpiritExplosion; }
    float lifeTimer = 2.0f; // ����
};