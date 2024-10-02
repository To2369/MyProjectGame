#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
// ’eŠÇ—‚Ì‘O•ûéŒ¾
class ArtsManager;

// ’e
class  Arts
{
public:
    Arts(ArtsManager* manager);
    virtual ~Arts() {}

    virtual void Update(float elapsedTIme) = 0;

    virtual void Render(ID3D11DeviceContext* dc) = 0;

    // ƒfƒoƒbƒOƒvƒŠƒ~ƒeƒBƒu•`‰æ
    virtual void DrawDebugPrimitive();

    void Destroy();

public:
    const DirectX::XMFLOAT3& GetPosition() const { return position; }

    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

    // •ûŒüæ“¾
    const DirectX::XMFLOAT3& GetDirection() const { return direction; }

    // •ûŒü‚Ìİ’è
    void SetDirection(const DirectX::XMFLOAT3& direction) { this->direction = direction; }

    // Šg‘åk¬æ“¾
    const DirectX::XMFLOAT3& GetScale() const { return scale; }

    // ”¼Œaæ“¾
    float GetRadius() const { return radius; }

    int GetUseSpiritEnergy() const { return useSpiritEnergy; }
    int GetUseSkillEnergy() const { return useSkillEnergy; }
    int GetDamage() const { return damage; }
protected:
    // s—ñXVˆ—
    void UpdateTransform();

protected:
    DirectX::XMFLOAT3	position = { 0,0,0 };	// ˆÊ’u
    DirectX::XMFLOAT3	direction = { 0,0,1 };	// •ûŒü
    DirectX::XMFLOAT3	scale = { 1,1,1 };		// Šg‘åk¬
    // p¨s—ñ
    DirectX::XMFLOAT4X4	transform = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    ArtsManager* artsMgr = nullptr;

    float radius = 0.5f;	// ”¼Œa

    int useSpiritEnergy = 0;
    int useSkillEnergy = 0;
    int damage = 0;
};