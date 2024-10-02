#pragma once

#include "Graphics/Model.h"
#include "Arts.h"

// �C�e�Z���i
class ArtsSpiritExplosion : public Arts
{
public:
    ArtsSpiritExplosion(ArtsManager* manager);
    ~ArtsSpiritExplosion()override;

    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render(ID3D11DeviceContext* dc) override;

    void Launch(const DirectX::XMFLOAT3& direction,
        const DirectX::XMFLOAT3& position);
private:
    std::unique_ptr<Model> model;
    float lifeTimer = 2.0f; // ����
};